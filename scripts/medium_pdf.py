#!/usr/bin/env python3
# Requires poppler-utils (pdftohtml) and Pillow. Review the images afterwards: Medium ad banners
# and duplicated images at page breaks sometimes come through and need deleting.
"""Convert a Medium 'print to PDF' story into Markdown + images, using pdftohtml -xml layout data.
usage: python3 convert.py story.pdf outdir prefix
writes outdir/story.md, outdir/media/<prefix>-NN.(jpg|png), prints medium URL on first line of stdout."""
import sys, re, subprocess, html, shutil
from pathlib import Path
from xml.etree import ElementTree as ET
from PIL import Image

pdf, outdir, prefix = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[3]
work = outdir / "_xml"; work.mkdir(parents=True, exist_ok=True)
subprocess.run(["pdftohtml", "-xml", "-q", "-zoom", "1.5", str(pdf), str(work / "out")], check=True,
               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
raw = (work / "out.xml").read_text(errors="ignore")
raw = re.sub(r"<!DOCTYPE[^>]*>", "", raw)
root = ET.fromstring(raw)
fonts = {}
for fs in root.iter("fontspec"):
    fonts[fs.get("id")] = (int(fs.get("size")), fs.get("family"), fs.get("color"))

def inner_md(el):
    """text element -> markdown with links, bold/italic dropped except links"""
    s = ET.tostring(el, encoding="unicode")
    s = re.sub(r"^<text[^>]*>|</text>\s*$", "", s)
    s = re.sub(r'<a href="([^"]+)">(.*?)</a>', lambda m: f"[{re.sub('<[^>]+>','',m.group(2)).strip()}]({html.unescape(m.group(1))})"
               if "medium.com/plans" not in m.group(1) else re.sub('<[^>]+>','',m.group(2)), s, flags=re.S)
    s = re.sub(r"<[^>]+>", "", s)
    return html.unescape(s)

medium_url = None
items = []  # (page, top, kind, payload)
for page in root.iter("page"):
    pn = int(page.get("number")); ph = int(page.get("height"))
    for el in page:
        top = int(el.get("top", 0))
        if el.tag == "image":
            w, h = int(el.get("width")), int(el.get("height"))
            if w >= 250 and top + h <= ph - 30:
                items.append((pn, top, "img", (el.get("src"), w, h, int(el.get("left")))))
        elif el.tag == "text":
            size, fam, color = fonts.get(el.get("font"), (0, "", ""))
            txt = inner_md(el)
            plain = "".join(el.itertext())
            if "Times" in fam:
                m = re.match(r"https://medium\.com/@\S+", plain.strip())
                if m and not medium_url: medium_url = m.group(0)
                continue
            items.append((pn, top, "txt", (int(el.get("left")), size, fam, color, txt, plain)))

items.sort(key=lambda x: (x[0], x[1], x[3][0] if x[2] == "txt" else 0))
# group text into lines
lines = []
for it in items:
    pn, top, kind, p = it
    if kind == "img":
        lines.append({"page": pn, "top": top, "kind": "img", "p": p}); continue
    left, size, fam, color, txt, plain = p
    if lines and lines[-1]["kind"] == "txt" and lines[-1]["page"] == pn and abs(lines[-1]["top"] - top) <= 4:
        L = lines[-1]; L["parts"].append((left, txt, plain, size, fam, color))
    else:
        lines.append({"page": pn, "top": top, "kind": "txt", "parts": [(left, txt, plain, size, fam, color)]})
for L in lines:
    if L["kind"] != "txt": continue
    seen=set(); uniq=[]
    for x in sorted(L["parts"], key=lambda x: x[0]):
        k=(x[0],x[2])
        if k in seen: continue
        seen.add(k); uniq.append(x)
    L["parts"]=uniq
    L["left"] = L["parts"][0][0]
    L["md"] = "".join(x[1] for x in L["parts"])
    L["plain"] = "".join(x[2] for x in L["parts"])
    L["size"] = max(x[3] for x in L["parts"])
    L["code"] = all("HAAAAA" in x[4] or not x[2].strip() for x in L["parts"]) and L["size"] <= 13
    L["pre"] = 165 <= L["left"] <= 169 and L["size"] < 20
    L["roboto"] = any("Roboto" in x[4] for x in L["parts"])
    L["grey"] = all(x[5] in ("#6a6a6a",) for x in L["parts"] if x[2].strip())

# trim to story body
start = next(i for i, L in enumerate(lines) if L["kind"] == "txt" and re.match(r"Week \d+, Entry \d+", L["plain"].strip()))
end = next(i for i, L in enumerate(lines) if L["kind"] == "txt" and L["plain"].strip().startswith("Written by"))
body = lines[start:end]
# drop trailing tag line(s) and promo lines
TAGS = {"Physcomp", "Uc Berkeley", "PhysComp"}
def is_tagline(L):
    if L["kind"] != "txt": return False
    urls = re.findall(r"\]\((https?://[^)]+)\)", L["md"])
    return bool(urls) and all("/tag/" in u for u in urls)
body = [L for L in body if not is_tagline(L) and not (L["kind"] == "txt" and ("Subscribe to Pro" in L["plain"] or "Academic Discount" in L["plain"]))]

media = outdir / "media"; media.mkdir(parents=True, exist_ok=True)
pre_buf = []
def emit_pre():
    global pre_buf
    if any(re.search(r"[|\[\]→{};]", s) for s in pre_buf):
        out.append("```"); out.extend(pre_buf); out.append("```"); out.append("")
    else:
        for s in pre_buf:
            out.append((f"**{s.strip()}**" if re.fullmatch(r"\d{4}", s.strip()) else s.strip()) + "  ")
        out.append("")
    pre_buf = []
out, para, prev, n_img, videos_seen, in_code = [], [], None, 0, set(), False
def flush():
    global para
    if para:
        out.append(" ".join(para).replace("  ", " ").strip()); out.append("")
    para = []
BODY_LEFT = 164
for L in body:
    if L["kind"] == "img":
        if pre_buf: emit_pre()
        if in_code: out.append("```"); out.append(""); in_code = False
        flush()
        src = Path(L["p"][0])
        n_img += 1
        im = Image.open(src)
        name = f"{prefix}-{n_img:02d}"
        if im.mode in ("RGBA", "P") and src.suffix == ".png" and im.size[0] * im.size[1] < 900_000:
            dest = media / f"{name}.png"; im.save(dest, optimize=True)
        else:
            dest = media / f"{name}.jpg"; im.convert("RGB").save(dest, quality=85, optimize=True)
        out.append(f"![](media/{dest.name})"); out.append("")
        prev = L; continue
    plain = L["plain"].strip()
    if L["roboto"]:
        m = re.search(r"\((https://www\.youtube\.com/watch\?v=[^)]+)\)", L["md"])
        if m and m.group(1) not in videos_seen and plain not in ("Watch on",):
            flush(); videos_seen.add(m.group(1)); out.append(f"[Video: {plain}]({m.group(1)})"); out.append("")
        prev = L; continue
    if L.get("pre") and not L["code"]:
        flush()
        if in_code: out.append("```"); out.append(""); in_code = False
        pre_buf.append(L["plain"].rstrip()); prev = L; continue
    if pre_buf: emit_pre()
    if L["code"]:
        flush()
        if not in_code: out.append("```"); in_code = True
        indent = 0 if L.get("pre") else max(0, round((L["left"] - 193) / 7))
        out.append(" " * indent + L["plain"].rstrip()); prev = L; continue
    if in_code: out.append("```"); out.append(""); in_code = False
    gap = (L["top"] - prev["top"]) if (prev and prev["page"] == L["page"] and prev["kind"] == "txt") else 99
    if re.match(r"Week \d+, Entry \d+", plain):
        out.append(f"**{plain}**  "); prev = L; continue
    if plain.startswith("Technology Design Foundations"):
        out.append(f"_{plain}_"); out.append(""); prev = L; continue
    if L["size"] >= 20:
        flush(); out.append(f"## {plain}"); out.append(""); prev = L; continue
    if L["grey"] and L["size"] <= 15:
        flush()
        if prev is not None and prev.get("grey") and prev.get("kind") == "txt" and out and out[-1] == "" and out[-2].startswith("_"):
            out[-2] = out[-2][:-1] + " " + L['md'].strip() + "_"
        else:
            out.append(f"_{L['md'].strip()}_"); out.append("")
        prev = L; continue
    indented = L["left"] >= BODY_LEFT + 8
    numbered = re.match(r"\d+\.\s", plain) and L["left"] >= BODY_LEFT + 6
    new_par = gap > 34 or (indented and (numbered or gap > 30)) or (prev and prev.get("kind") == "txt" and prev.get("left", 0) >= BODY_LEFT + 8) != indented and not (prev and prev.get("left",0) >= BODY_LEFT+8 and indented)
    if numbered:
        flush(); para = [L["md"].strip()]
    elif indented and (gap > 30 or not para):
        flush(); para = ["- " + L["md"].strip()]
    elif gap > 34 and not (gap == 99 and para and not re.search(r"[.!?:)\"”]$", para[-1])):
        flush(); para = [L["md"].strip()]
    else:
        if para and para[-1].endswith("-"):
            para[-1] = para[-1] + L["md"].strip()
        else:
            para.append(L["md"].strip())
    prev = L
flush()
if pre_buf: emit_pre()
if in_code: out.append("```")
md = "\n".join(out)
md = re.sub(r"\n{3,}", "\n\n", md)
md = re.sub(r"(\w)\[(?=[^\]]+\]\(http)", r"\1 [", md)
md = re.sub(r"\[ +", "[", md)
md = re.sub(r"\]\(([^)]+)\)\s*\[([^\]]+)\]\(\1\)", r" \2](\1)", md)  # merge split links
(outdir / "story.md").write_text(md)
shutil.rmtree(work)
print(medium_url or "")

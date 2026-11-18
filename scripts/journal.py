#!/usr/bin/env python3
"""TDF PhysComp design journal tooling. Standard library only.

  python3 scripts/journal.py new [--date YYYY-MM-DD] [--title "..."]
      Create today's entry (or the given date) from templates/entry.md.
      Tuesdays are electronics entries, Thursdays fabrication. Skips other
      days, no-class dates and dates outside the course unless --force.

  python3 scripts/journal.py index
      Rebuild the entry table in README.md between the INDEX markers.

  python3 scripts/journal.py check
      Report which entries are missing required parts (video, images,
      code, circuit diagram, write-up). Exit code 0 always; prints a report.

  python3 scripts/journal.py export project-1|project-2|all [--out build]
      Combine entries in that submission's date range into one Markdown
      file (build/<name>.md) ready for pandoc -> HTML/PDF.
"""
import argparse, datetime as dt, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CFG = json.loads((ROOT / "journal.config.json").read_text())
ENTRIES = ROOT / "entries"
TEMPLATE = ROOT / "templates" / "entry.md"
README = ROOT / "README.md"
INDEX_START, INDEX_END = "<!-- INDEX:START -->", "<!-- INDEX:END -->"
CHECK_START, CHECK_END = "<!-- CHECK:START -->", "<!-- CHECK:END -->"
SLIDES_DOC = "sources/lecture-slides.md"

TYPE_LABEL = {"electronics": "Electronics", "fabrication": "Fabrication",
              "project": "Project", "analysis": "Diagrammatic analysis"}


def d(s):
    return dt.date.fromisoformat(s)


def week_of(date):
    return (date - d(CFG["first_class"])).days // 7 + 1


def project_for(date):
    hit = [p for p in CFG["projects"] if d(p["start"]) <= date <= d(p["end"])]
    return hit[-1]["id"] if hit else "ambient-display"


def parse_front(text):
    m = re.match(r"^---\n(.*?)\n---\n", text, re.S)
    meta = {}
    if m:
        for line in m.group(1).splitlines():
            if ":" in line:
                k, v = line.split(":", 1)
                meta[k.strip()] = v.strip().strip('"')
    body = text[m.end():] if m else text
    return meta, body


def entries():
    out = []
    for f in sorted(ENTRIES.glob("*/README.md")):
        meta, body = parse_front(f.read_text())
        meta["_dir"] = f.parent
        meta["_body"] = body
        out.append(meta)
    return sorted(out, key=lambda m: (m.get("date", ""), m["_dir"].name))


def slug(s):
    return re.sub(r"[^a-z0-9]+", "-", s.lower()).strip("-")[:48]


# ---------- new ----------
def cmd_new(a):
    tz_today = dt.datetime.now(dt.timezone(dt.timedelta(hours=-8))).date()
    date = d(a.date) if a.date else tz_today
    if not a.force:
        if not (d(CFG["first_class"]) <= date <= d(CFG["last_class"])):
            print(f"{date} is outside the course dates; nothing to do."); return
        if date.isoformat() in CFG["no_class_dates"]:
            print(f"{date} is a no-class day; nothing to do."); return
    wd = date.strftime("%A").lower()
    etype = a.type or CFG["entry_types"].get(wd)
    if not etype:
        print(f"{date} is a {wd}; entries are created on class days only."); return
    week = week_of(date)
    title = a.title or f"Week {week} {TYPE_LABEL.get(etype, etype)}"
    folder = ENTRIES / f"{date.isoformat()}-w{week}-{etype}"
    if any(p.name.startswith(date.isoformat()) and p.name.endswith(etype)
           for p in ENTRIES.iterdir() if p.is_dir()):
        print(f"An {etype} entry for {date} already exists; nothing to do."); return
    key = CFG["slides_by_date"].get(date.isoformat())
    slides_ref = f"[{key}]({'../../' + SLIDES_DOC})" if key else \
        f"_add this session's deck to {SLIDES_DOC} and cite its key_"
    text = TEMPLATE.read_text()
    for k, v in {
        "title": title, "date": date.isoformat(), "week": str(week), "type": etype,
        "project": project_for(date), "slides": key or "",
        "type_label": TYPE_LABEL.get(etype, etype),
        "date_label": date.strftime("%a %b %-d, %Y"), "slides_ref": slides_ref,
    }.items():
        text = text.replace("{{" + k + "}}", v)
    (folder / "media").mkdir(parents=True, exist_ok=True)
    (folder / "code").mkdir(exist_ok=True)
    for sub in ("media", "code"):
        (folder / sub / ".gitkeep").touch()
    (folder / "README.md").write_text(text)
    print(f"Created {folder.relative_to(ROOT)}")


# ---------- check ----------
REQUIRED = [
    ("video", re.compile(r"\.(mp4|mov|webm|gif)\b|youtu|vimeo|drive\.google\.com/file", re.I)),
    ("images", re.compile(r"!\[[^\]]*\]\([^)]+\.(jpe?g|png|webp|gif)\)", re.I)),
    ("code", None),
    ("circuit", re.compile(r"circuit|schematic|fritzing|tinkercad|wiring", re.I)),
    ("write-up", None),
]


def missing_parts(m):
    body, folder = m["_body"], m["_dir"]
    etype = m.get("type")
    miss = []
    if etype == "analysis":
        if not re.search(r"```mermaid|!\[", body):
            miss.append("diagram")
        return miss
    for name, rx in REQUIRED:
        if name == "code":
            files = [p for p in (folder / "code").glob("**/*") if p.is_file() and p.name != ".gitkeep"] \
                if (folder / "code").exists() else []
            if not files and "```" not in body and "github.com" not in body:
                miss.append("code")
        elif name == "write-up":
            sec = re.search(r"## Results and reflection\n(.*?)(\n## |\Z)", body, re.S)
            txt = re.sub(r"<!--.*?-->", "", sec.group(1), flags=re.S).strip() if sec else ""
            if len(txt.split()) < 40:
                miss.append("write-up")
        elif name == "circuit" and etype == "fabrication":
            continue
        elif name == "code" and etype == "fabrication":
            continue
        elif not rx.search(re.sub(r"<!--.*?-->", "", body, flags=re.S)):
            miss.append(name)
    return miss


def check_rows():
    rows = []
    for m in entries():
        miss = missing_parts(m)
        rows.append((m, miss))
    return rows


def cmd_check(a):
    for m, miss in check_rows():
        mark = "OK " if not miss else "-- "
        print(f"{mark}{m['_dir'].name}: {'complete' if not miss else 'missing ' + ', '.join(miss)}")


# ---------- index ----------
def cmd_index(a):
    rows = ["| Date | Wk | Type | Entry | Slides | Status | Missing |",
            "|---|---|---|---|---|---|---|"]
    for m, miss in check_rows():
        rel = m["_dir"].relative_to(ROOT).as_posix()
        status = m.get("status", "draft")
        badge = {"ready": "ready", "submitted": "submitted"}.get(status, "draft")
        med = f" · [Medium]({m['medium']})" if m.get("medium") else ""
        rows.append(f"| {m.get('date','')} | {m.get('week','')} | {TYPE_LABEL.get(m.get('type'), m.get('type',''))} | "
                    f"[{m.get('title', m['_dir'].name)}]({rel}/README.md){med} | {m.get('slides','')} | "
                    f"{badge} | {', '.join(miss) if miss else 'none'} |")
    table = "\n".join(rows)
    text = README.read_text()
    new = re.sub(re.escape(INDEX_START) + r".*?" + re.escape(INDEX_END),
                 INDEX_START + "\n" + table + "\n" + INDEX_END, text, flags=re.S)
    if new != text:
        README.write_text(new); print("README index updated.")
    else:
        print("README index already current.")


# ---------- export ----------
def cmd_export(a):
    sub = CFG["submissions"][a.submission]
    lo, hi = d(sub["from"]), d(sub["to"])
    picked = [m for m in entries() if lo <= d(m["date"]) <= hi]
    out = ROOT / a.out
    out.mkdir(exist_ok=True)
    parts = [f"---\ntitle: \"{sub['title']}\"\nauthor: \"{CFG['author']}\"\n"
             f"subtitle: \"{CFG['course']} · {CFG['term']}\"\ndate: \"{dt.date.today():%B %-d, %Y}\"\n---\n"]
    for m in picked:
        body = re.sub(r"<!--.*?-->\n?", "", m["_body"], flags=re.S)
        rel = m["_dir"].relative_to(ROOT).as_posix()
        # make relative links work from the repo root
        body = re.sub(r"\]\((?!https?://|#|/)([^)]+)\)", lambda x: f"]({rel}/{x.group(1)})", body)
        body = body.replace(f"]({rel}/../../", "](")
        body = re.sub(r"^# ", "# ", body, count=1, flags=re.M)
        parts.append(body.strip() + '\n\n<div class="newpage"></div>\n')
    parts.append((ROOT / SLIDES_DOC).read_text())
    name = f"journal-{a.submission}"
    (out / f"{name}.md").write_text("\n\n".join(parts))
    print(f"Wrote {a.out}/{name}.md with {len(picked)} entries.")


def main():
    p = argparse.ArgumentParser()
    s = p.add_subparsers(dest="cmd", required=True)
    n = s.add_parser("new"); n.add_argument("--date"); n.add_argument("--title")
    n.add_argument("--type"); n.add_argument("--force", action="store_true")
    s.add_parser("index"); s.add_parser("check")
    e = s.add_parser("export"); e.add_argument("submission", choices=list(CFG["submissions"]))
    e.add_argument("--out", default="build")
    a = p.parse_args()
    {"new": cmd_new, "index": cmd_index, "check": cmd_check, "export": cmd_export}[a.cmd](a)


if __name__ == "__main__":
    main()

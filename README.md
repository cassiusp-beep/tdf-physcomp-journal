# TDF Physical Computing: Design Journal

Cassius Palacio · DESINV 202 Technology Design Foundations, Physical Computing section · UC Berkeley MDes · Fall 2026
Instructors: Chris Myers, Sudhu Tewari · GSI: Alistair Vizuet

Two entries a week (Tuesday electronics, Thursday fabrication). Electronics and project entries need a video, images, code and a circuit diagram, and a short write-up [C17, slide 28]; fabrication entries need images and a write-up [C16; C18]. One diagrammatic analysis of a system per submission [C17, slide 7]. Lecture slides are cited by key; see [sources/lecture-slides.md](sources/lecture-slides.md). The Expressive Mechanics build lives in its own repo, [turntable-dragon](https://github.com/cassiusp-beep/turntable-dragon) [R-DRAGON].

## Entries

The table rebuilds itself on every push. "Missing" lists the required parts an entry doesn't have yet.

<!-- INDEX:START -->
| Date | Wk | Type | Entry | Slides | Status | Missing |
|---|---|---|---|---|---|---|
| 2026-10-20 | 1 | Electronics | [W1 E1: Kickoff, the kit and first circuits](entries/2026-10-20-w1-electronics/README.md) · [Medium](https://medium.com/@cassiusp/w1-e1-tdf-physcomp-23bf40edd487) | C15 | ready | none |
| 2026-10-22 | 1 | Fabrication | [W1 E2: Laser cutting, joinery and the 737 ring](entries/2026-10-22-w1-fabrication/README.md) · [Medium](https://medium.com/@cassiusp/w1-e2-tdf-physcomp-ea0f7cbf61ea) | C16 | ready | none |
| 2026-10-27 | 2 | Electronics | [W2 E1: Reading the board and the voltage divider](entries/2026-10-27-w2-electronics/README.md) · [Medium](https://medium.com/@cassiusp/w2-e1-tdf-physcomp-f884e30fba15) | C17 | ready | video |
| 2026-10-29 | 2 | Fabrication | [W2 E2: Ring review, mechanisms and 3D printing](entries/2026-10-29-w2-fabrication/README.md) · [Medium](https://medium.com/@cassiusp/w2-e2-tdf-physcomp-eac642b391b3) | C18 | ready | none |
| 2026-11-03 | 3 | Electronics | [W3 E1: The Personal Space Servo](entries/2026-11-03-w3-electronics/README.md) | C19 | ready | none |
| 2026-11-05 | 3 | Fabrication | [W3 E2: 3D printing the 737 ring](entries/2026-11-05-w3-fabrication/README.md) | C20 | ready | none |
| 2026-11-10 | 4 | Project | [W4 E1: Expressive Mechanics, the camshaft dragon automaton](entries/2026-11-10-w4-project/README.md) | C19 | ready | none |
| 2026-11-12 | 4 | Diagrammatic analysis | [W4 analysis: the turntable dragon as a system](entries/2026-11-12-w4-analysis/README.md) | C17 | ready | none |
| 2026-11-12 | 4 | Project | [W4 E2: Expressive Mechanics, the smile-controlled turntable dragon](entries/2026-11-12-w4-project/README.md) | C07 | ready | none |
| 2026-11-17 | 5 | Electronics | [W5 E1: ESP32 and web APIs](entries/2026-11-17-w5-electronics/README.md) | C23 | ready | none |
| 2026-11-19 | 5 | Fabrication | [W5 E2: Enclosures and heat-set inserts](entries/2026-11-19-w5-fabrication/README.md) | C24 | ready | none |
| 2026-11-24 | 6 | Electronics | [W6 E1: Getting the ESP32 online](entries/2026-11-24-w6-electronics/README.md) | C25 | ready | video |
<!-- INDEX:END -->

## Deadlines

| Date | What |
|---|---|
| Thu Nov 12, 2026 | Project 1: Expressive Mechanics presented |
| **Thu Nov 19, 2026** | **Project 1 documentation due on bCourses (extended)** [C23, slide 4] |
| Tue Nov 24, 2026 | ESP32 setup and web API homework [C23, slides 17 and 19] |
| Thu Dec 3, 2026 | Project 2: working prototype demo [C24] |
| Tue Dec 8, 2026 | Jacobs Design Showcase, Jacobs 310, 12:00 to 3:30 pm [C25] |
| Tue Dec 15, 2026 | Project 2: Ambient Display documentation due on bCourses [C24; C25] |

## AI use

Following the course AI policy [C01, slide 46], I used Claude as a drafting and documentation aid in this journal: organizing entries, converting my Medium posts, citing the lecture slides, preparing media, and drafting parts of some write-ups from my own build notes, code and decisions, which I then reviewed. The design work, builds, code decisions and critiques are my own.

## How the automation works

| Workflow | When it runs | What it does |
|---|---|---|
| **New entry** (`new-entry.yml`) | Tue and Thu at 9 PM Pacific, plus a manual button | Creates `entries/<date>-w<week>-<type>/` from the template, with the right week, type and lecture citation |
| **Index** (`index.yml`) | Every push | Rebuilds the table above and flags missing parts |
| **Export** (`export.yml`) | Manual button: pick `project-1`, `project-2` or `all` | Builds a PDF and an HTML file of that submission's entries plus the sources page, and attaches them to a GitHub Release you can download and upload to bCourses |

To run one by hand: **Actions** tab, pick the workflow, **Run workflow**.

On your own machine:

```bash
python3 scripts/journal.py new --date 2026-11-19   # create an entry
python3 scripts/journal.py check                   # what's missing
python3 scripts/journal.py index                   # rebuild the table
python3 scripts/journal.py export project-2        # build/journal-project-2.md, then pandoc it
```

## Writing an entry

1. Open the entry's `README.md`. The Assignment section is pre-filled from the slides where possible.
2. Drop photos and video in `media/`, code and Tinkercad exports in `code/`.
3. Write **Results and reflection** in your own words. The course allows AI as a learning and productivity aid, not for submitting AI-generated work as your own [C01, slide 46].
4. Change `status: draft` to `status: ready` in the front matter.

Medium articles: weeks 1 and 2 are reproduced from the published Medium stories, with the original PDFs in `sources/medium-pdfs/`. To bring in a new one, see [sources/medium.md](sources/medium.md).

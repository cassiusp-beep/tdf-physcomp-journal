# Medium articles

Published under [@cassiusp](https://medium.com/@cassiusp). Each story is reproduced in its journal entry, and the original print-to-PDF is kept in [`medium-pdfs/`](medium-pdfs/).

| Entry | Medium story | PDF |
|---|---|---|
| [W1 E1: Kickoff, the kit and first circuits](../entries/2026-10-20-w1-electronics/README.md) | [W1 E1](https://medium.com/@cassiusp/w1-e1-tdf-physcomp-23bf40edd487) | [PDF](medium-pdfs/W1-E1-tdf-physcomp.pdf) |
| [W1 E2: Laser cutting, joinery and the 737 ring](../entries/2026-10-22-w1-fabrication/README.md) | [W1 E2](https://medium.com/@cassiusp/w1-e2-tdf-physcomp-ea0f7cbf61ea) | [PDF](medium-pdfs/W1-E2-tdf-physcomp.pdf) |
| [W2 E1: Reading the board and the voltage divider](../entries/2026-10-27-w2-electronics/README.md) | [W2 E1](https://medium.com/@cassiusp/w2-e1-tdf-physcomp-f884e30fba15) | [PDF](medium-pdfs/W2-E1-tdf-physcomp.pdf) |
| [W2 E2: Ring review, mechanisms and 3D printing](../entries/2026-10-29-w2-fabrication/README.md) | [W2 E2](https://medium.com/@cassiusp/w2-e2-tdf-physcomp-eac642b391b3) | [PDF](medium-pdfs/W2-E2-tdf-physcomp.pdf) |
| [W3 E1: Personal space, a PIR build and an ultrasonic sign](../entries/2026-11-03-w3-electronics/README.md) | _not published yet_ | |

## Adding a new Medium story

In Medium, open the story and print it to PDF, then:

```bash
python3 scripts/medium_pdf.py story.pdf /tmp/story w3-e1
# copy /tmp/story/story.md into the entry under "## Journal entry", and /tmp/story/media/* into the entry's media/
```

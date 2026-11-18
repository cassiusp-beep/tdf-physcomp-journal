---
title: W4 analysis: the turntable dragon as a system
date: 2026-11-12
week: 4
type: analysis
project: expressive-mechanics
slides: C17
medium:
status: draft
---

# W4 analysis: the turntable dragon as a system

**Required once per journal submission** [C17, slide 7]. This one covers the Project 1 submission.

The diagram follows one smile from face to motion: sensing, interpretation, a timer that holds the state, power conversion, and a mechanical reduction. Feedback runs through the person, who sees the dragon move and smiles again.

```mermaid
flowchart TB
  subgraph Person
    F[Face and smile]
    E[Sees the dragon spin]
  end
  subgraph Laptop
    W[Webcam] --> M[MediaPipe FaceMesh]
    M --> R["Smile ratio: mouth width 61/291 / face width 234/454"]
    R --> C{"Above +10% of relaxed baseline for 0.15 s?"}
    C -- yes --> T["Hold timer: 10 s after last smile"]
    C -- no --> T
    T --> S["Serial S100 or S0, 115200 baud"]
  end
  subgraph Electronics
    U[Elegoo Uno R3] --> H[L298N H-bridge]
    P["6 V adapter"] --> H
    H -- "about 4 V" --> G[Mini-300 gearmotor]
    U -. "watchdog: stop after 2 s silence" .-> H
  end
  subgraph Mechanism
    G --> PN["28T pinion"] --> RG["80T internal ring gear, 2.86:1"] --> PL["150 mm platter"] --> D[Flexi Dragon]
  end
  F --> W
  S -- USB --> U
  D --> E --> F
```

<!-- Optional: export this as an image (or redraw it by hand) and add your own annotations about where the system can fail: no face in view, weak USB power, IN2 not reaching the H-bridge. -->

## Sources

- [C17] class 17 deck, journal requirements (diagrammatic analysis)
- System details from the [turntable-dragon](https://github.com/cassiusp-beep/turntable-dragon) README

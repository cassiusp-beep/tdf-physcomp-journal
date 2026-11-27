---
title: "W5 E2: Enclosures and heat-set inserts"
date: 2026-11-19
week: 5
type: fabrication
project: ambient-display
slides: C24
medium:
status: ready
---

# W5 E2: Enclosures and heat-set inserts

**Week 5 · Fabrication · Thu Nov 19, 2026**
Lecture: [C24](../../sources/lecture-slides.md)

## Assignment

Class 24 was an hour of lecture followed by a work session [C24]. The lecture covered advanced enclosures for Project 2, the Ambient Display: combining 3D printing (the body or sides, curvilinear shapes, a way to show design refinement) with laser cutting (front and back, or top and bottom, as fast sheet parts), 3 mm brass heat-set threaded inserts so an enclosure can be opened and closed repeatedly, a practice hole print to size holes in CAD, and snap fits, bosses or posts, and modularity [C24]. The Project 2 timeline: working prototype demo Thu Dec 3, Design Showcase Tue Dec 8, documentation due on bCourses Tue Dec 15 [C24].

## What I did

In class on Nov 19 I practiced installing heat-set inserts in a 3D-printed test plate. The plate had five brass inserts, each in a different mounting style: two in raised cylindrical bosses, one flush in the flat surface, and two in recessed or counterbored pockets. The test print was the class's, not something I designed. I installed the inserts with a soldering iron fitted with an insert tip, pressing each one into its hole.

## Media

![A hand holding a flat grey 3D-printed test plate with rounded corners and a notch at one end, with five brass heat-set inserts set in it. From the left, two are in raised cylindrical bosses, one is flush in the flat surface, and two are in recessed pockets. A wooden workbench, a black machine upright, other students' hands and a black printed test piece with rows of holes are in the background.](media/heat-set-insert-test-print.jpg)

## Code and circuit

Not required for a fabrication entry.

## Results and reflection

The useful part of the session was seeing five mounting styles side by side in one print instead of reading about them. For the Ambient Display this matters because I want an enclosure I can open, so I can reach the ESP32 and its USB-C port for reprogramming and power. Screws into heat-set inserts can be opened and closed again and again, which a snap fit or glue would not allow as well. My project rules also say to keep USB-C reachable and to print a fit coupon first, and the practice hole print from the lecture is how I would do that for the board and the insert holes [C24].

All five mounting styles held well (raised bosses, flush and recessed), and none failed, so this test did not give me a clear winner. I would pick based on what the enclosure needs, such as a raised boss where a part must stand off the wall.

Next steps: design the enclosure body around the Feather and the NeoPixel strip, choose which parts are printed and which are laser cut, and print a fit coupon before the full part, ahead of the Thu Dec 3 prototype demo [C24].

## Sources

- [C24] 2026 TDF - PhysComp class 24 - Work Session

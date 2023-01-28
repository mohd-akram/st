# Space Travel

This is a C port of Ken Thompson's
[Space Travel](https://en.wikipedia.org/wiki/Space_Travel_\(video_game\)),
ported from PDP-7 assembly. The original source files are in `src/cmd/st*.s`
in the [pdp7-unix](https://github.com/DoctorWkt/pdp7-unix) repository.

## Install

The SDL2 and SDL2_ttf libraries are required.

Run `make` to build `st`.

## Controls

| Key | Action       |
|-----|--------------|
| 1   | Quit         |
| 2   | New game     |
| 3   | Thrust down  |
| 4   | Thrust up    |
| 5   | Yaw right    |
| 6   | Yaw left     |
| 7   | Scale up     |
| 8   | Scale down   |

You can also use the arrow keys for yaw and scale.

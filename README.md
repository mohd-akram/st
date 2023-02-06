# Space Travel

This is a C port of Ken Thompson's
[Space Travel](https://en.wikipedia.org/wiki/Space_Travel_\(video_game\)),
ported from PDP-7 assembly. The original source files are in `src/cmd/st*.s`
in the [pdp7-unix](https://github.com/DoctorWkt/pdp7-unix) repository.

## Install

The SDL2 library is required.

Run `make` to build `st`.

Run `make install` to install to `/usr/local/bin`.

### Build and Run on OSX

    brew install sdl2
    gcc -I/opt/homebrew/Cellar/sdl2/2.26.2/include -D_THREAD_SAFE -L/opt/homebrew/Cellar/sdl2/2.26.2/lib -lSDL2 st.c
    ./a.out

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

## Gameplay

> For controls, there were several buttons: jet forward, jet back, turn left,
> turn right; go up scale on the display, go down scale. The acceleration of
> the ship was fixed, but it scaled with the display scale--it was thus fixed
> in terms of pixels/sec/sec. Normally the ship was in the center of the screen
> and always pointed up; the display showed the plan view of the solar system,
> so that the "rotate" controls rotated the solar system around you on the
> display.
>
> No relativity; scale up enough and you travel to Pluto in a few seconds. But
> don't scale up too much, or you might not find the Solar System again without
> restarting.
>
> The object of the game was simply to fly around, get into orbits, land.
> "Land" meant to cross the surface with a small enough speed.
>
> *[Space Travel: Exploring the solar system and the
> PDP-7](https://www.bell-labs.com/usr/dmr/www/spacetravel.html) by Dennis
> Ritchie*

The current dominating attractor, scale, and landing status (L/CL for
landed/crash landed) are displayed at the bottom. The moving dot on the line
below the main view represents the horizontal velocity relative to the current
dominating attractor.

# Assignment 2 (10 points): Animated 3D Octopus Sculpture

## Overview

This assignment implements an animated 3D "octopus" sculpture using OpenGL. The scene demonstrates hierarchical modelling, tentacle animation, multiple light sources (directional, point, and spot lights), textured materials, and a fullscreen background. The octopus body and head are drawn from a cube mesh with different texture regions; the tentacles are generated as tapered cylinder segments and animated procedurally.

## Features

- Procedural tentacle mesh generation (tapered cylinder segments)
- Hierarchical segment animation for smooth tentacle motion (oscillation + outward bend)
- Textured body, face, and tentacles with diffuse & specular maps
- Multiple lights: directional, four point lights, and a carriable spotlight attached to the camera
- Background rendered as a fullscreen textured quad (deep ocean)
- Camera movement (WASD + mouse look)
- Emissive/glow and bioluminescence-style parameters exposed in shader uniforms
- "Breathing" pulsing animation for the octopus body/head to give it a living motion

## Files of interest

- `main.cpp` entry point (scene setup, animation, rendering loop)
- `lights.vs`, `lights.fs` shader pair used for lit objects (body, head, tentacles)
- `bg.vs`, `bg.fs` background shader pair
- `resources/textures/` textures used by the scene (octopus face, body, tentacles, deep-ocean)

## Controls

- W / A / S / D move camera forward / left / back / right
- Mouse look around (first-person camera)
- Esc or window close exit

## How it works (short technical notes)

- A unit cube mesh (36 vertices) is used for the octopus body and head; separate textures and shader uniforms control whether the face texture is applied or the body textures are used.
- Tentacles are generated once as a tapered cylinder mesh. Each tentacle in the scene is drawn as a series of scaled/rotated segments where each segment is a transformed instance of the same base mesh. The animation uses per-segment decay and phase offsets for natural variation.
- Breathing / pulsing: the octopus body and head use a time-based pulse multiplier (a small sinusoidal scale) to simulate a breathing motion. The C++ code computes a `pulse` value per-frame and applies it to the body/head model scales so the sculpture subtly expands and contracts.
- Lighting: The shader implements a directional light, an array of 4 point lights, and a camera-attached spotlight. Material properties use diffuse and specular textures plus a shininess constant.

## Assets

Textures used by this assignment are stored in:

`src/assignment/assignment_2/resources/textures/`

## Visualize

| Visualize                                                                                  |
| ------------------------------------------------------------------------------------------ |
| <img src="/.github/images/assignment_2_3d_sculpture.png" alt="3d sculpture" width="900" /> |
| <img src="/.github/images/assignment_2_3d_sculpture.gif" alt="3d sculpture" width="900" /> |

https://github.com/user-attachments/assets/673367c5-f5ae-4adc-ab35-bab34f70ad17

## Credits

- Background image: Deepocean [Unsplash](https://unsplash.com/photos/blue-water-with-white-bubbles-3o5oUjrD90w)
- Textures drawn using: [Pixilart](https://www.pixilart.com/draw)

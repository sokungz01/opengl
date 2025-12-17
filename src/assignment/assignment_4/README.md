# Assignment 4 — Character Animation Control

## Overview

This assignment implements a third-person character controller with skeletal animation blending in OpenGL. It demonstrates smooth animation transitions between idle, walk, run, punch, and kick states using animation blending techniques. The character movement is controlled via keyboard and mouse input, with a camera system that follows behind the character.

## Features

- **Skeletal Animation System**: Full skeletal mesh animation using bone transformations
- **Smooth Animation Blending**: Seamless transitions between different animation states
- **Character Movement States**:
  - Idle animation (default state)
  - Walk animation (WASD movement)
  - Run animation (Walk + Shift key)
  - Punch animation (Left mouse button)
  - Kick animation (Right mouse button)
- **Third-Person Camera**: Dynamic camera that follows behind the character
- **Character Rotation**:
  - Mouse horizontal movement rotates the character
  - A/D keys provide continuous left/right turning
  - S key performs instant 180° turn
- **Ground Plane**: Simple platform rendering underneath the character
- **Light Blue Sky**: Atmospheric background color

Picture and Video

<img width="636" height="484" alt="Screenshot 2568-12-18 at 00 26 40" src="https://github.com/user-attachments/assets/576bab5e-457a-4e1b-8215-09870d56cf53" />


https://github.com/user-attachments/assets/70ecc2f1-d7ee-421d-947b-557e9ee74f6b




## Controls

### Movement

- **W** — Move forward (walk)
- **Left Shift** — Run (hold while moving with WASD)

### Combat

- **Left Mouse Button** — Punch attack
- **Right Mouse Button** — Kick attack

### Camera

- **Mouse Movement (Horizontal)** — Rotate character and camera view
- **Mouse Scroll** — Zoom in/out (if enabled)

### System

- **Esc** — Exit application

## Implementation Details

### Animation Blending

The animation system uses linear interpolation between two animations:

- Blend rate: ~0.0125 per frame (adjustable)
- Transitions complete when blend amount > 0.9
- Animation timing is preserved during state changes to prevent restart/jumping

### Assets Location

- Model: `resources/objects/lewis/lewis.dae`
- Animations:
  - `resources/objects/lewis/idle.dae`
  - `resources/objects/lewis/walk.dae`
  - `resources/objects/lewis/run.dae`
  - `resources/objects/lewis/punch.dae`
  - `resources/objects/lewis/kick.dae`
  - `resources/objects/lewis/left-turn.dae`
  - `resources/objects/lewis/right-turn.dae`

## Model and Animation Credits

- **Character Model**: Lewis character from Mixamo
- **Animations**: All animations from Mixamo
- **Source**: https://www.mixamo.com/

Mixamo provides free, high-quality 3D characters and animations for game development and animation projects.

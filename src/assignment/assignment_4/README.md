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

## Controls

### Movement

- **W** — Move forward (walk)
- **S** — Turn around 180° and move forward
- **A** — Rotate character left while moving
- **D** — Rotate character right while moving
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

### Animation State Machine

The character uses a state machine with the following states:

- `IDLE` — Default standing position
- `IDLE_WALK` / `WALK_IDLE` — Blending between idle and walk
- `WALK` — Walking animation loop
- `RUN` — Running animation loop
- `WALK_RUN` / `RUN_WALK` — Smooth blending between walk and run speeds
- `IDLE_PUNCH` / `PUNCH_IDLE` — Punch animation with blending
- `IDLE_KICK` / `KICK_IDLE` — Kick animation with blending

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

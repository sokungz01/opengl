# Assignment 3 — Car, Coins, and Ground (OpenGL)

## Overview

This assignment implements a small OpenGL scene containing a controllable car, coins to collect, and a ground plane. It demonstrates basic rendering, simple scene logic, and shader usage. The code is organized into small C++ modules and GLSL shaders

## Run / Controls

- Keyboard: WASD — move / steer the car (W forward, S backward, A turn left, D turn right)
- Camera View: C (cycle Rear / Left / Right) — V (hold for Front view)
- Esc: quit

While running, the objective is to drive around (or move) and collect coins; when all coins are collected a simple win condition may be triggered.

## Implement Model (Car and Coins)

This section documents the model and interaction requirements implemented for the assignment.

- Collision checker

  - Car <-> Coins: use a simple distance-based collision test (radius or bounding-sphere) to detect when the car collects a coin. When distance <= threshold, mark the coin as collected and update score/visibility.
  - Car <-> Border: detect when the car crosses the play-area boundary and respond (stop at border, bounce, or clamp position). A simple AABB or planar boundary test is sufficient for this assignment.

- Camera View
  - Toggle or cycle camera modes with `C`:
    - Rear view (follow behind the car)
    - Left side view
    - Right side view
  - Hold `V` for Front view (while `V` is pressed, switch to a front-facing camera; release to revert to the last camera mode).
  - Implementation tip: compute camera position relative to the car's position and orientation (offset behind/above for rear view; offset to the side for left/right). Lerp or smooth the camera for nicer motion.

## Model Credits

Models used or referenced for this assignment:

- Gold coin model: https://www.cgtrader.com/free-3d-models/various/various-models/gold-coin-hand-painted
- Car model: https://free3d.com/3d-model/ferrari-formula-1-72527.html

## Demo

<img width="900" alt="visualize" src="https://github.com/user-attachments/assets/5b9cdb2f-68b0-4074-810c-6cb841e92a06" />

https://github.com/user-attachments/assets/5e05f459-bdf0-4aca-a572-aeec108db1fb


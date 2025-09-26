# Assignment 0 (0 point): 2D drawing — Sierpinski triangle

## Overview

This small draws a Sierpinski triangle in 2D. The program creates a recursive Sierpinski object that generates triangle vertices at varying recursion depths. The goal of the assignment is to explore recursive geometry generation and simple GPU rendering for 2D shapes.

The demo supports increasing and decreasing the recursion depth at runtime using keyboard controls. It is intentionally minimal so you can focus on the recursive algorithm and how vertex updates are passed to the GPU.

## Features

- Recursive generation of Sierpinski triangle vertices
- Runtime controls to change recursion depth
- Simple 2D rendering pipeline (vertex buffer updates, shader usage)

## Controls

- q: increase recursion depth (adds one level)
- e: decrease recursion depth (removes one level, not below 0)
- Esc / window close: quit the application

## Visualize

<img src="/.github/images/assignment_0_image.png" alt="Sierpinski image" width="900" />

<img src="/.github/images/assignment_0_gif.gif" alt="Sierpinski gif" width="900" />

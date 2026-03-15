# rEngine | Software 3D Renderer

A small **software 3D engine written in C**.
The renderer runs hardware-accelerated and implements the basic parts of a 3D graphics pipeline without relying on the 3D OpenGL API. 
It uses the SDL2 Library only for Window and Keybind support.

The project focuses on manually implementing common rendering systems such as transformations, projection, and triangle rasterization.

## Current Features

* 3D object loading
* Camera movement and transformations
* Perspective projection
* Triangle rendering
* Face sorting
* Basic rendering pipeline

## Work In Progress

* Full triangle rasterization / scanline filling
* Depth buffering
* Texture mapping

## Status

Development is currently **paused** while other projects are being worked on.

## Build

Instructions will depend on your platform and compiler.
Recommended to use the makefile provided.

Example using bash:

```
make
```

## Notes

This project is mainly an exploration of **how software rendering works internally**, including the math behind 3D graphics and the steps involved in transforming 3D geometry into a 2D image.

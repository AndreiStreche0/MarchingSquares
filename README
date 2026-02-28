# Marching Squares Contouring Tool

A C-based image processing utility that implements the Marching Squares algorithm to generate contours from an input image. The tool specifically handles Netpbm (P3) format images.

## Features
- Load/Write: Handles P3 PPM image formats.
- Resize: Upscales images by a factor of 4 using a pixel replication method.
- Grid Generation: Calculates a binary grid based on a luminance threshold (PRAG).
- Marching Squares: Applies the algorithm to generate contour lines based on the calculated grid.
- Custom Base Converter: Includes a utility to switch between binary and decimal for cell configuration mapping.

## How to Build
Use the provided Makefile to compile the project:
Command: make build
The resulting executable will be named: marching_squares

## Usage
The program reads commands from stdin:
- READ <filename>: Load an image.
- RESIZE: Scale up the image.
- GRID: Generate and display the binary grid.
- MARCH: Apply the Marching Squares algorithm.
- WRITE: Output the image data.
- EXIT: Free memory and close the program.

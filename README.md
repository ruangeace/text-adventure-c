#Text-Adventure-C
Text-based adventure game written in C as a university project.

The project focuses on low-level programming concepts and manual memory management.
The game runs entirely in the terminal and emphasizes logic, data structures, and system design rather than graphics.

Overview

The gameplay is inspired by classic dungeon exploration games.
The player moves between interconnected rooms, encounters enemies, and progresses through a dynamically generated environment.

The game world is built at runtime using dynamic data structures instead of static arrays.

#Technical Concepts#

This project demonstrates practical use of:

-Pointers
-Dynamic memory allocation (malloc, free)
-Singly linked lists
-Struct-based data modeling
-Modular programming (multiple source files)
-Game state management
-Manual memory lifecycle control

Rooms, enemies, and progression are represented through linked structures.
Each room contains directional connections implemented through pointers, requiring careful navigation and memory handling.

Purpose:
The goal of this project was to strengthen:

- Understanding of memory in C
- Control over dynamic data structures
- Program organization in medium-sized projects
- Logical thinking in system design
#This project represents a solid foundation in core C programming and systems-level development.

To compile:
gcc main.c gamelib.c

To run:
./a.out

# CPP Game Programming Projects

This repository contains multiple game programming projects and assessments, each focusing on different aspects of game development. Below is an overview of the functionality and requirements of each project:

## Directory Overview

### `assessment1/`

This project introduces the basics of game programming. It focuses on:

- Setting up a simple game loop.
- Managing configurations through a `config.txt` file.
- Rendering basic graphics and handling user input.
- Using fonts and other assets for visual elements.

### `assessment2/`

This project expands on the fundamentals by introducing:

- Entity-Component-System (ECS) architecture for managing game objects.
- Configuration-driven gameplay, where game settings and entities are defined in a `config.txt` file.
- Implementation of player movement, enemy spawning, and collision detection.
- Scoring system and visual effects for enhanced gameplay.

### `assessment3/`

This project focuses on creating a more complex game with the following features:

- Asset management for textures, animations, and fonts.
- Level creation using a configuration file (`level.txt`) to define entities, tiles, and decorations.
- Advanced player mechanics, including animations for different states (e.g., running, jumping).
- Implementation of special effects and bonus features for extra credit.

### `assessment4/`

This project builds on the previous assessments and introduces:

- Room-based level design with a camera system that alternates between follow mode and room mode.
- NPC behaviors such as patrolling and following the player.
- Advanced tile interactions, including health restoration and teleportation mechanics.
- Custom level creation with multiple rooms and interesting gameplay scenarios.

### `drag-drop/`

This project focuses on implementing a drag-and-drop game mechanic. Key features include:

- A grid-based level system with tiles and decorations.
- Player movement and interactions with the environment.
- Debugging tools such as toggling grid and bounding box visibility.
- Custom level creation and configuration for unique gameplay.

### `intro/`

This directory contains introductory examples and materials for learning C++ game programming. It includes:

- Basic examples of C++ programs (`lecture2.cpp`, `lecture3.cpp`).
- Simple demonstrations of game programming concepts.
- Supporting files for students to practice and experiment.

### `particle_system/`

This project likely involves creating and managing particle systems. It may include:

- Implementation of particle effects for visual enhancements.
- Customizable parameters for particle behavior, such as lifespan, speed, and color.

### `sfml_intro/`

This directory introduces the Simple and Fast Multimedia Library (SFML) for game development. It likely covers:

- Setting up an SFML project.
- Basics of rendering, input handling, and window management.
- Simple examples to demonstrate SFML's capabilities.

---

## How to Use

1. Navigate to the desired project directory.
2. Follow the instructions in the `readme.md` file within the directory.
3. Use the provided `Makefile` to build the project by running:
   ```sh
   make
   ```

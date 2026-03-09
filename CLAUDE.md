# CLAUDE.md

Guidelines and context for Claude Code working on the `Morpheus` repository.

## Project Overview

**Morpheus** is a C++17 arcade game prototype modeled after the classic Atari Asteroids, rendered with SDL3 using a wireframe aesthetic.

**Gameplay features:**
- Wireframe ship with rotation, thrust, and toroidal screen wrapping
- Randomly spawned asteroids with random velocities and slow rotation
- Circle-based collision detection (ship-asteroid and asteroid-asteroid)
- Asteroid fragmentation: LARGE → 3 MEDIUM → 3 SMALL → destroyed
- Game state: score, high score, ships remaining (default 3)

**Controls:** ESC quit, Left/Right arrows rotate, Up arrow thrust.

## Tech Stack

- **Language:** C++17
- **Build system:** CMake ≥ 3.15
- **Primary library:** SDL3 3.4.2+
- **Compiler warnings:** `-Wall -Wextra -Wpedantic` (GCC/Clang), `/W4` (MSVC)

## Repository Layout

```
src/            # Implementation files
include/        # Header files and configuration
assets/         # Runtime assets (copied to build output automatically)
CMakeLists.txt  # Build definition
README.md       # User-facing setup instructions
AGENTS.md       # Agent working agreement (overlaps with this file)
build/          # Generated build output — do not edit
cmake-build-*/  # IDE-generated build directories — do not edit
```

Key files:
- `src/main.cpp` — SDL init, game loop, input handling, collision detection, rendering
- `src/Game.cpp` / `include/Game.h` — game state, collision logic, asteroid spawning and fragmentation
- `src/Ship.cpp` / `include/Ship.h` — ship physics, thrust, screen wrapping, wireframe rendering
- `src/Asteroid.cpp` / `include/Asteroid.h` — asteroid physics, rotation, wireframe rendering
- `include/config.h` — all gameplay tuning constants (speeds, radii, frame rate, etc.)
- `include/colors.h` — RGBA color palette constants
- `include/shapes.h` — asteroid vertex definitions for the two shape variants

## Build and Run

```bash
cmake -S . -B build
cmake --build build
./build/Morpheus
```

Validation after any code change:

```bash
cmake --build build
```

Then do a quick smoke test: launch the executable, verify startup, input, and rendering work.

## Coding Guidelines

- Follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) naming conventions:
  - Classes: `PascalCase`
  - Member variables: `m_camelCase` prefix
  - Constants: `UPPER_CASE` or `kCamelCase`
  - Free functions and methods: `camelCase`
- Use `#pragma once` for header guards
- Keep changes minimal and targeted — do not refactor unrelated code in the same change
- Add comments only where logic is non-obvious
- Prefer small, composable constants in `include/config.h` for gameplay tuning
- Use OOP principles; extend with new classes when adding new entity types
- Preserve existing style in all touched files
- Avoid non-ASCII characters unless already present and required

## Safety Rules

Never edit generated files or directories:
- `build/`, `cmake-build-*/`, `CMakeFiles/`, `CMakeCache.txt`, `compile_commands.json`

Do not remove or overwrite user changes unrelated to the current task. If unexpected modifications appear, pause and ask for guidance.

## Near-Term Backlog

- Projectile shooting (space bar) with asteroid collision detection
- Scoring display: large=20pts, medium=50pts, small=100pts
- On-screen HUD (score, lives) via SDL rendered text or simple graphics
- Ship respawn invulnerability (flashing effect ~2–3 seconds)
- Game-over state with restart option when ships reach 0
- Enemy UFO with AI behavior and shooting
- Sound effects (thrust, laser, collision, explosion)
- Progressive difficulty scaling across waves

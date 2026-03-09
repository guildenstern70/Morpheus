# AGENTS

Agent onboarding and working agreement for the `Morpheus` repository.

## 1) Project Snapshot

- **Name**: `Morpheus`
- **Type**: C++ game prototype
- **Style goal**: Atari Asteroids-like 2D gameplay
- **Current gameplay state**:
  - A green wireframe ship shaped like an `A`, centered on screen
  - Ship rotation with Left/Right arrow keys (180°/sec turning speed)
  - Thrust with Up arrow, accelerating ship in facing direction (200 units/sec acceleration)
  - Thrust flame visual at ship rear when accelerating
  - Screen wraps toroidally (edges loop around like a sphere)
  - 5 asteroids spawn randomly on screen with random velocities (2-6 units/sec)
  - Asteroids rotate slowly at 20°/sec
  - **Collision detection system**:
    - Circle-based collision detection (ship radius: 10px, asteroid radius varies by size)
    - Ship-asteroid collisions: ship loses a life and respawns at center, asteroid breaks into fragments
    - Asteroid-asteroid collisions: both asteroids break into fragments
    - Asteroid fragmentation: LARGE → 3 MEDIUM pieces, MEDIUM → 3 SMALL pieces, SMALL → destroyed
    - Fragments inherit parent position and receive impulse-based velocity (40 units/sec spread at ±45°)
  - Game state tracks score, high score, and ships remaining (default: 3)

## 2) Tech Stack

- **Language**: C++17
- **Build system**: CMake (minimum 3.15)
- **Primary library**: SDL3 (`find_package(SDL3 3.4.2 REQUIRED)`)
- **Compiler warnings**:
  - MSVC: `/W4`
  - Others: `-Wall -Wextra -Wpedantic`

## 3) Repository Layout

- `src/` - C++ source files (current game loop lives in `src/main.cpp`)
- `include/` - headers (currently minimal/empty)
- `assets/` - runtime assets copied to executable output dir after build
  - `images/ship.png` is a visual reference for ship shape
- `CMakeLists.txt` - build definition and SDL3 linkage
- `README.md` - user-facing project setup notes
- `build/`, `cmake-build-*` - generated build output (do not hand-edit)

## 4) Build and Run

Preferred from repository root:

```bash
cmake -S . -B build
cmake --build build
./build/Morpheus
```

Alternative (as documented in README):

```bash
cd build
cmake ..
cmake --build .
./Morpheus
```

## 5) Runtime and Render Model

- Single executable target: `Morpheus`
- Single-threaded SDL event/update/render loop in `mainLoop`
- Frame flow:
  1. Poll input events
  2. Compute `deltaSeconds`
  3. Update ship angle
  4. Render scene
  5. Delay remaining frame budget for 60 FPS cap

## 6) Controls (Current)

- `ESC`: Quit
- `Left Arrow`: Turn left (counterclockwise)
- `Right Arrow`: Turn right (clockwise)
- `Up Arrow`: Apply thrust (accelerate in facing direction)

## 7) Source-of-Truth Notes

- SDL version in code/CMake is **SDL3**.

## 8) Coding Guidelines for Agents

- Keep changes minimal and targeted.
- Prefer small, composable constants for gameplay tuning.
- Use Object Oriented Programming (OOP) principles for future extensibility (e.g. `Ship` class).
- Follow naming style as in Google C++ Style guide - https://google.github.io/styleguide/cppguide.html#General_Naming_Rules
- Preserve the existing style in touched files.
- Avoid introducing non-ASCII unless already present and needed.
- Add comments only where logic is non-obvious.
- Do not refactor unrelated code in the same change.

## 9) Safety Rules for Editing

- Never edit generated files or directories:
  - `build/`
  - `cmake-build-*/`
  - `CMakeFiles/`, `CMakeCache.txt`, `compile_commands.json`
- Do not remove or overwrite user changes unrelated to the current task.
- If unexpected modifications appear during work, pause and request guidance.

## 10) Validation Checklist for Code Changes

When code is modified, run:

```bash
cmake --build build
```

Then perform a quick run smoke test to ensure startup and input/render still work.

## 11) Near-Term Backlog Suggestions

- Add projectile shooting (space bar) with collision detection against asteroids
- Add scoring system (points for destroying asteroids: large=20, medium=50, small=100)
- Add score/lives UI rendering on screen using SDL text or simple graphics
- Implement ship respawn invulnerability period (flashing effect for 2-3 seconds)
- Implement game-over state when ships reach 0 with restart option
- Add enemy UFO spawning with AI behavior and shooting
- Add sound effects (thrust, laser, collision, explosion)
- Implement difficulty scaling with progressive waves


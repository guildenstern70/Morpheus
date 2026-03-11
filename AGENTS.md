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
  - Shooting with Space bar (bolts inherit ship velocity, with a fire cooldown)
  - Thrust flame visual at ship rear when accelerating
  - Screen wraps toroidally (edges loop around like a sphere)
  - 5 asteroids spawn randomly on screen with random velocities (2-6 units/sec)
  - Progressive waves: each cleared level increases asteroid count (`+2` per level)
  - Asteroids rotate slowly at 20°/sec
  - Ship explosion animation (~2 seconds) before respawn
  - Safe deployment and safe respawn checks with waiting messages and asteroid regeneration fallback
  - HUD rendered in-game (title, score, lives icons, high score)
  - Game-over state/message when ships reach 0
  - **Collision detection system**:
    - Circle-based collision detection (ship radius: 10px, asteroid radius varies by size)
    - Ship-asteroid collisions: ship loses a life and respawns at center, asteroid breaks into fragments
    - Asteroid-asteroid collisions: elastic bounce (velocity exchange along collision normal)
    - Bolt-asteroid collisions: asteroid breaks into fragments and awards score
    - Asteroid fragmentation: LARGE → 3 MEDIUM pieces, MEDIUM → 5 SMALL pieces, SMALL → destroyed
    - Fragments inherit parent position and receive impulse-based velocity (~40 units/sec)
  - Game state tracks score, high score, ships remaining (default: 3), and current level

## 2) Tech Stack

- **Language**: C++17
- **Build system**: CMake (minimum 3.15)
- **Primary library**: SDL3 (`find_package(SDL3 3.4.2 REQUIRED)`)
- **Compiler warnings**:
  - MSVC: `/W4`
  - Others: `-Wall -Wextra -Wpedantic`

## 3) Repository Layout

- `src/` - C++ source files (current game loop lives in `src/main.cpp`)
- `include/` - headers for game entities and systems (`Game`, `Ship`, `Asteroid`, `Bolt`, `ShipExplosion`, `TextRenderer`, plus config/colors/shapes)
- `assets/` - runtime assets copied to executable output dir after build
  - `images/Screenshot.png` contains a gameplay screenshot
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
  3. Update game state (ship, asteroids, bolts, explosions, respawn/level transitions)
  4. Resolve collisions (`bolt-asteroid`, `ship-asteroid`, `asteroid-asteroid`)
  5. Render HUD, entities, and state messages
  6. Delay remaining frame budget for 60 FPS cap

## 6) Controls (Current)

- `ESC`: Quit
- `Left Arrow`: Turn left (counterclockwise)
- `Right Arrow`: Turn right (clockwise)
- `Up Arrow`: Apply thrust (accelerate in facing direction)
- `Space`: Fire bolt

## 7) Source-of-Truth Notes

- SDL version in code/CMake is **SDL3**.

## 8) Coding Guidelines for Agents

- Keep changes minimal and targeted.
- Prefer small, composable constants for gameplay tuning.
- Use Object-Oriented Programming (OOP) principles for future extensibility (e.g. `Ship` class).
- Follow naming style as in Google C++ Style guide - https://google.github.io/styleguide/cppguide.html#General_Naming_Rules
    - Classes: `PascalCase`
    - Member variables: `m_camelCase` prefix
    - Constants: `UPPER_CASE` or `kCamelCase`
    - Free functions and methods: `camelCase`
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

- Implement ship respawn invulnerability period (flashing effect for 2-3 seconds)
- Implement restart option after game-over
- Add enemy UFO spawning with AI behavior and shooting
- Add sound effects (thrust, laser, collision, explosion)
- Implement difficulty scaling with progressive waves


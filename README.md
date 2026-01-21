# Morpheus

A cross-platform 2D game built with SDL2 and C++.

## Prerequisites

### macOS
```bash
brew install sdl2
```

### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev
```

### Windows
Download SDL2 development libraries from [libsdl.org](https://www.libsdl.org/download-2.0.php)

## Building

```bash
cd build
cmake ..
cmake --build .
```

## Running

```bash
./Morpheus
```

## Controls

- **ESC** - Exit game

## Project Structure

```
SDL2Game/
├── src/           # Source files (.cpp)
├── include/       # Header files (.h)
├── assets/        # Game assets
│   ├── textures/  # Images and sprites
│   ├── sounds/    # Audio files
│   └── fonts/     # Font files
├── build/         # Build output (gitignored)
└── CMakeLists.txt # CMake configuration
```

## Next Steps

1. Add SDL2_image for texture loading: `brew install sdl2_image`
2. Add SDL2_mixer for audio: `brew install sdl2_mixer`
3. Add SDL2_ttf for fonts: `brew install sdl2_ttf`
4. Implement game entities, physics, and collision detection
5. Create a game loop with fixed timestep

## Resources

- [SDL2 Documentation](https://wiki.libsdl.org/)
- [Lazy Foo's SDL Tutorials](https://lazyfoo.net/tutorials/SDL/)

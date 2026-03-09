//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include <SDL3/SDL.h>
#include <array>
#include <cmath>
#include <iostream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
auto WINDOW_TITLE = "Morpheus";

constexpr float SHIP_TURN_SPEED_DEG_PER_SEC = 180.0f;
constexpr float SHIP_CENTER_X = SCREEN_WIDTH / 2.0f;
constexpr float SHIP_CENTER_Y = SCREEN_HEIGHT / 2.0f;
constexpr int TARGET_FPS = 60;
constexpr float TARGET_FRAME_TIME_MS = 1000.0f / static_cast<float>(TARGET_FPS);

void mainLoop(SDL_Window* window,
              SDL_Renderer* renderer) {

    bool running = true;
    bool turningLeft = false;
    bool turningRight = false;
    float shipAngleDegrees = 0.0f;
    Uint64 previousTicks = SDL_GetTicks();
    SDL_Event event;

    while (running) {
        const Uint64 frameStartTicks = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    } else if (event.key.key == SDLK_LEFT) {
                        turningLeft = true;
                    } else if (event.key.key == SDLK_RIGHT) {
                        turningRight = true;
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (event.key.key == SDLK_LEFT) {
                        turningLeft = false;
                    } else if (event.key.key == SDLK_RIGHT) {
                        turningRight = false;
                    }
                    break;
                default:
                    break;
            }
        }

        const Uint64 currentTicks = SDL_GetTicks();
        const float deltaSeconds = static_cast<float>(currentTicks - previousTicks) / 1000.0f;
        previousTicks = currentTicks;

        if (turningRight) {
            shipAngleDegrees += SHIP_TURN_SPEED_DEG_PER_SEC * deltaSeconds;
        }
        if (turningLeft) {
            shipAngleDegrees -= SHIP_TURN_SPEED_DEG_PER_SEC * deltaSeconds;
        }

        shipAngleDegrees = std::fmod(shipAngleDegrees, 360.0f);
        if (shipAngleDegrees < 0.0f) {
            shipAngleDegrees += 360.0f;
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        // Local points form an "A"-like ship silhouette centered at origin.
        std::array<SDL_FPoint, 5> localPoints = {{{0.0f, -26.0f}, {-16.0f, 20.0f}, {0.0f, 8.0f}, {16.0f, 20.0f}, {0.0f, -26.0f}}};

        const float angleRadians = shipAngleDegrees * (static_cast<float>(M_PI) / 180.0f);
        const float cosA = std::cos(angleRadians);
        const float sinA = std::sin(angleRadians);

        std::array<SDL_FPoint, 5> transformedPoints{};
        for (size_t i = 0; i < localPoints.size(); ++i) {
            const float x = localPoints[i].x;
            const float y = localPoints[i].y;
            transformedPoints[i].x = SHIP_CENTER_X + (x * cosA - y * sinA);
            transformedPoints[i].y = SHIP_CENTER_Y + (x * sinA + y * cosA);
        }

        for (size_t i = 0; i + 1 < transformedPoints.size(); ++i) {
            SDL_RenderLine(renderer,
                           transformedPoints[i].x,
                           transformedPoints[i].y,
                           transformedPoints[i + 1].x,
                           transformedPoints[i + 1].y);
        }

        SDL_RenderPresent(renderer);

        const Uint64 frameEndTicks = SDL_GetTicks();
        const float frameElapsedMs = static_cast<float>(frameEndTicks - frameStartTicks);
        if (frameElapsedMs < TARGET_FRAME_TIME_MS) {
            SDL_Delay(static_cast<Uint32>(TARGET_FRAME_TIME_MS - frameElapsedMs));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    mainLoop(window, renderer);
    return 0;
}

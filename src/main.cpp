//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

#include "colors.h"
#include "config.h"
#include "Asteroid.h"
#include "Game.h"
#include "Ship.h"


void mainLoop(SDL_Window* window,
              SDL_Renderer* renderer) {

    bool running = true;
    bool turningLeft = false;
    bool turningRight = false;
    bool thrusting = false;
    Game game;
    Ship ship(SHIP_CENTER_X, SHIP_CENTER_Y);
    Uint64 previousTicks = SDL_GetTicks();
    SDL_Event event;

    std::vector<Asteroid> asteroids;
    game.populateAsteroids(asteroids, INITIAL_ASTEROID_COUNT);

    constexpr Colors::Color BACKGROUND_COLOR = Colors::BLACK;
    constexpr Colors::Color SHIP_COLOR = Colors::SILVER;

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
                    } else if (event.key.key == SDLK_UP) {
                        thrusting = true;
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (event.key.key == SDLK_LEFT) {
                        turningLeft = false;
                    } else if (event.key.key == SDLK_RIGHT) {
                        turningRight = false;
                    } else if (event.key.key == SDLK_UP) {
                        thrusting = false;
                    }
                    break;
                default:
                    break;
            }
        }

        const Uint64 currentTicks = SDL_GetTicks();
        const float deltaSeconds = static_cast<float>(currentTicks - previousTicks) / 1000.0f;
        previousTicks = currentTicks;

        ship.update(turningLeft,
                    turningRight,
                    thrusting,
                    deltaSeconds,
                    SHIP_TURN_SPEED_DEG_PER_SEC,
                    SHIP_THRUST_ACCELERATION,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT);

        // Update asteroids
        for (auto& asteroid : asteroids) {
            asteroid.update(deltaSeconds, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // Collision detection - ship vs asteroids
        for (size_t i = 0; i < asteroids.size(); ++i) {
            if (game.checkShipAsteroidCollision(ship, asteroids[i])) {
                game.loseShip();
                // Create fragments from destroyed asteroid
                auto fragments = game.createFragments(asteroids[i]);
                asteroids.erase(asteroids.begin() + i);
                asteroids.insert(asteroids.end(), fragments.begin(), fragments.end());
                // Reset ship position
                ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                break;  // Only one collision per frame
            }
        }

        // Collision detection - asteroid vs asteroid
        for (size_t i = 0; i < asteroids.size(); ++i) {
            for (size_t j = i + 1; j < asteroids.size(); ++j) {
                if (game.checkCircleCollision(
                    asteroids[i].getX(), asteroids[i].getY(), asteroids[i].getRadius(),
                    asteroids[j].getX(), asteroids[j].getY(), asteroids[j].getRadius())) {

                    // Create fragments from both asteroids
                    auto fragments1 = game.createFragments(asteroids[i]);
                    auto fragments2 = game.createFragments(asteroids[j]);

                    // Remove colliding asteroids (remove larger index first)
                    asteroids.erase(asteroids.begin() + j);
                    asteroids.erase(asteroids.begin() + i);

                    // Add fragments
                    asteroids.insert(asteroids.end(), fragments1.begin(), fragments1.end());
                    asteroids.insert(asteroids.end(), fragments2.begin(), fragments2.end());

                    // Adjust loop indices after removal
                    --i;
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer,
            BACKGROUND_COLOR.r,
            BACKGROUND_COLOR.g,
            BACKGROUND_COLOR.b,
            BACKGROUND_COLOR.a);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,
            SHIP_COLOR.r,
            SHIP_COLOR.g,
            SHIP_COLOR.b,
            SHIP_COLOR.a);
        ship.render(renderer, thrusting);

        for (auto& asteroid : asteroids) {
            asteroid.render(renderer, Colors::GRAY);
        }

        SDL_RenderPresent(renderer);

        const Uint64 frameEndTicks = SDL_GetTicks();
        const auto frameElapsedMs = static_cast<float>(frameEndTicks - frameStartTicks);
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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    mainLoop(window, renderer);
    return 0;
}

//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>

#include "colors.h"
#include "config.h"
#include "Asteroid.h"
#include "Game.h"
#include "Ship.h"
#include "ShipExplosion.h"
#include "TextRenderer.h"

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

    // Explosion state
    std::unique_ptr<ShipExplosion> shipExplosion = nullptr;
    bool shipDestroyed = false;
    bool waitingToRespawn = false;

    // Game start safety - check if initial spawn position is safe
    constexpr float SPAWN_SAFETY_RADIUS = 50.0f;
    bool gameStarted = game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids);

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

        // Update explosion animation if active
        if (shipExplosion) {
            shipExplosion->update(deltaSeconds);
            if (shipExplosion->isFinished()) {
                // Explosion finished
                shipExplosion = nullptr;
                shipDestroyed = false;

                // Only reset ship if there are lives remaining
                if (game.getShipsRemaining() > 0) {
                    // Check if spawn position is safe
                    constexpr float SPAWN_SAFETY_RADIUS = 50.0f;  // Safe area around spawn point
                    if (game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids)) {
                        // Safe to respawn immediately
                        ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                        waitingToRespawn = false;
                    } else {
                        // Wait until the spawn area is clear
                        waitingToRespawn = true;
                    }
                }
            }
        }

        // If waiting to respawn, check if position is safe now
        if (waitingToRespawn && game.getShipsRemaining() > 0) {
            constexpr float SPAWN_SAFETY_RADIUS = 50.0f;
            if (game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids)) {
                ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                waitingToRespawn = false;
            }
        }

        // If game hasn't started yet (initial spawn check), check if position is safe now
        if (!gameStarted) {
            if (game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids)) {
                gameStarted = true;
            }
        }

        // Check if game is over
        const bool gameOver = (game.getShipsRemaining() == 0 && !shipExplosion);

        // Only update ship if game has started, not destroyed, not waiting to respawn, and game is not over
        if (gameStarted && !shipDestroyed && !waitingToRespawn && !gameOver) {
            ship.update(turningLeft,
                        turningRight,
                        thrusting,
                        deltaSeconds,
                        SHIP_TURN_SPEED_DEG_PER_SEC,
                        SHIP_THRUST_ACCELERATION,
                        SCREEN_WIDTH,
                        SCREEN_HEIGHT);
        }

        // Update asteroids
        for (auto& asteroid : asteroids) {
            asteroid.update(deltaSeconds, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // Collision detection - ship vs asteroids (only if game started, ship is not destroyed, not waiting, and game not over)
        if (gameStarted && !shipDestroyed && !waitingToRespawn && !gameOver) {
            if (game.handleShipAsteroidCollisions(ship, asteroids, SHIP_CENTER_X, SHIP_CENTER_Y)) {
                // Collision occurred - create explosion
                shipExplosion = std::make_unique<ShipExplosion>(ship.getX(), ship.getY(), ship.getOrientation());
                shipDestroyed = true;
            }
        }

        // Collision detection - asteroid vs asteroid (elastic bounce)
        game.handleAsteroidAsteroidCollisions(asteroids);

        SDL_SetRenderDrawColor(renderer,
            BACKGROUND_COLOR.r,
            BACKGROUND_COLOR.g,
            BACKGROUND_COLOR.b,
            BACKGROUND_COLOR.a);
        SDL_RenderClear(renderer);

        // Render header
        constexpr float HEADER_SCALE = 2.5f;
        constexpr float SUBHEADER_SCALE = 1.5f;
        constexpr float TITLE_Y = 20.0f;
        constexpr float INFO_Y = 45.0f;
        constexpr float MARGIN = 20.0f;

        // First line: Title centered
        TextRenderer::renderText(renderer, "MORPHEUS",
                                SCREEN_WIDTH / 2.0f, TITLE_Y,
                                HEADER_SCALE, Colors::SILVER,
                                TextRenderer::Alignment::CENTER);

        // Second line: Score (left), Lives (center), High Score (right)
        std::string scoreText = "SCORE " + std::to_string(game.getScore());
        TextRenderer::renderText(renderer, scoreText,
                                MARGIN, INFO_Y,
                                SUBHEADER_SCALE, Colors::SILVER,
                                TextRenderer::Alignment::LEFT);

        // Lives: Draw ship icons centered
        const int shipsRemaining = game.getShipsRemaining();
        constexpr float SHIP_ICON_SCALE = 0.8f;
        constexpr float SHIP_ICON_SPACING = 20.0f;
        const float totalIconWidth = static_cast<float>(shipsRemaining) * SHIP_ICON_SPACING;
        const float startX = (SCREEN_WIDTH / 2.0f) - (totalIconWidth / 2.0f) + (SHIP_ICON_SPACING / 2.0f);

        SDL_SetRenderDrawColor(renderer,
            SHIP_COLOR.r,
            SHIP_COLOR.g,
            SHIP_COLOR.b,
            SHIP_COLOR.a);

        for (int i = 0; i < shipsRemaining; ++i) {
            Ship::renderIcon(renderer, startX + static_cast<float>(i) * SHIP_ICON_SPACING, INFO_Y + 10.0f, SHIP_ICON_SCALE);
        }

        std::string highScoreText = "HIGH " + std::to_string(game.getHighScore());
        TextRenderer::renderText(renderer, highScoreText,
                                SCREEN_WIDTH - MARGIN, INFO_Y,
                                SUBHEADER_SCALE, Colors::SILVER,
                                TextRenderer::Alignment::RIGHT);

        // Render ship or explosion or game over
        if (shipsRemaining == 0 && !shipExplosion) {
            // Game Over message
            constexpr float GAME_OVER_SCALE = 5.0f;
            TextRenderer::renderText(renderer, "GAME OVER",
                                    SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 25.0f,
                                    GAME_OVER_SCALE, Colors::SILVER,
                                    TextRenderer::Alignment::CENTER);
        } else if (shipExplosion) {
            shipExplosion->render(renderer, SHIP_COLOR);
        } else if (gameStarted && !waitingToRespawn) {
            // Only render ship if game has started and not waiting to respawn
            SDL_SetRenderDrawColor(renderer,
                SHIP_COLOR.r,
                SHIP_COLOR.g,
                SHIP_COLOR.b,
                SHIP_COLOR.a);
            ship.render(renderer, thrusting);
        }

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

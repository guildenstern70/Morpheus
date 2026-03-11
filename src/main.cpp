//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>

#include "colors.h"
#include "config.h"
#include "Asteroid.h"
#include "Bolt.h"
#include "Game.h"
#include "Ship.h"
#include "ShipExplosion.h"
#include "TextRenderer.h"

int renderHeader(SDL_Renderer *renderer, Game game, const Colors::Color SHIP_COLOR) {
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
    return shipsRemaining;
}

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
    game.populateAsteroids(asteroids, game.getAsteroidCountForCurrentLevel());

    // Bolts
    std::vector<Bolt> bolts;
    constexpr float FIRE_COOLDOWN = 0.25f;  // Quarter second between shots
    float timeSinceLastFire = FIRE_COOLDOWN;  // Start ready to fire

    // Explosion state
    std::unique_ptr<ShipExplosion> shipExplosion = nullptr;
    bool shipDestroyed = false;
    bool waitingToRespawn = false;
    float respawnWaitingMessageBlinkTimer = 0.0f;
    float gameStartWaitingMessageBlinkTimer = 0.0f;

    // Waiting timers for asteroid regeneration (3 seconds before regenerating)
    float respawnWaitingTime = 0.0f;  // Accumulated waiting time for respawn
    float gameStartWaitingTime = 0.0f;  // Accumulated waiting time for game start

    // INSERT COIN screen state (shown at startup and after game over)
    bool insertCoinScreen = true;
    float insertCoinBlinkTimer = 0.0f;
    float gameOverDisplayTimer = 0.0f;

    // Level transition state
    bool levelClearedMessageVisible = false;
    float levelClearedMessageTimer = 0.0f;

    // Snapshot of asteroids at the moment respawn waiting starts.
    std::vector<Asteroid> respawnAsteroidProfile;

    // Game start safety - check if initial spawn position is safe
    constexpr float SPAWN_SAFETY_RADIUS = 50.0f;
    bool gameStarted = game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids);

    constexpr Colors::Color BACKGROUND_COLOR = Colors::BLACK;
    constexpr Colors::Color SHIP_COLOR = Colors::SILVER;

    while (running) {
        const Uint64 frameStartTicks = SDL_GetTicks();

        // Check if game is over (needed for event handling)
        const bool gameOver = (game.getShipsRemaining() == 0 && !shipExplosion && !insertCoinScreen);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    } else if (event.key.key == SDLK_C) {
                        if (insertCoinScreen) {
                            // Preserve high score across resets
                            const int savedHighScore = game.getHighScore();
                            game = Game();
                            game.setHighScore(savedHighScore);
                            ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                            asteroids.clear();
                            game.populateAsteroids(asteroids, game.getAsteroidCountForCurrentLevel());
                            bolts.clear();
                            shipExplosion = nullptr;
                            shipDestroyed = false;
                            waitingToRespawn = false;
                            levelClearedMessageVisible = false;
                            levelClearedMessageTimer = 0.0f;
                            timeSinceLastFire = FIRE_COOLDOWN;
                            gameOverDisplayTimer = 0.0f;
                            insertCoinBlinkTimer = 0.0f;
                            respawnWaitingTime = 0.0f;
                            gameStartWaitingTime = 0.0f;
                            respawnWaitingMessageBlinkTimer = 0.0f;
                            gameStartWaitingMessageBlinkTimer = 0.0f;
                            respawnAsteroidProfile.clear();
                            gameStarted = game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids);
                            insertCoinScreen = false;
                        }
                    } else if (event.key.key == SDLK_LEFT) {
                        turningLeft = true;
                    } else if (event.key.key == SDLK_RIGHT) {
                        turningRight = true;
                    } else if (event.key.key == SDLK_UP) {
                        thrusting = true;
                    } else if (event.key.key == SDLK_SPACE) {
                        // Fire bolt if game started, not destroyed, not waiting, not in level transition, and cooldown ready
                        if (gameStarted && !shipDestroyed && !waitingToRespawn && !levelClearedMessageVisible && !gameOver && timeSinceLastFire >= FIRE_COOLDOWN) {
                            // Calculate bolt starting position at ship's nose
                            // Ship's forward direction: sin(angle) for X, -cos(angle) for Y
                            const float shipAngleRad = ship.getOrientation() * (PI / 180.0f);
                            constexpr float SHIP_NOSE_OFFSET = 12.0f;  // Distance from center to nose
                            const float boltStartX = ship.getX() + std::sin(shipAngleRad) * SHIP_NOSE_OFFSET;
                            const float boltStartY = ship.getY() + (-std::cos(shipAngleRad)) * SHIP_NOSE_OFFSET;

                            bolts.emplace_back(boltStartX, boltStartY, ship.getOrientation(),
                                             ship.getVelocityX(), ship.getVelocityY());
                            timeSinceLastFire = 0.0f;
                        }
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

        // Update fire cooldown timer
        timeSinceLastFire += deltaSeconds;

        // Update INSERT COIN blink timer
        insertCoinBlinkTimer += deltaSeconds;

        // After game over, wait a few seconds then transition to INSERT COIN screen
        if (gameOver) {
            gameOverDisplayTimer += deltaSeconds;
            if (gameOverDisplayTimer >= GAME_OVER_DISPLAY_DURATION) {
                const int savedHighScore = game.getHighScore();
                game = Game();
                game.setHighScore(savedHighScore);
                ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                asteroids.clear();
                game.populateAsteroids(asteroids, game.getAsteroidCountForCurrentLevel());
                bolts.clear();
                shipExplosion = nullptr;
                shipDestroyed = false;
                waitingToRespawn = false;
                levelClearedMessageVisible = false;
                levelClearedMessageTimer = 0.0f;
                timeSinceLastFire = FIRE_COOLDOWN;
                gameOverDisplayTimer = 0.0f;
                insertCoinBlinkTimer = 0.0f;
                respawnWaitingTime = 0.0f;
                gameStartWaitingTime = 0.0f;
                respawnWaitingMessageBlinkTimer = 0.0f;
                gameStartWaitingMessageBlinkTimer = 0.0f;
                respawnAsteroidProfile.clear();
                gameStarted = game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids);
                insertCoinScreen = true;
            }
        }

        if (levelClearedMessageVisible) {
            levelClearedMessageTimer += deltaSeconds;
            if (levelClearedMessageTimer >= LEVEL_CLEARED_MESSAGE_DURATION) {
                levelClearedMessageVisible = false;
                levelClearedMessageTimer = 0.0f;
                game.advanceLevel();
                game.populateAsteroids(asteroids, game.getAsteroidCountForCurrentLevel());

                // Re-run safe deployment check for the new wave.
                gameStarted = game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids);
                if (!gameStarted) {
                    gameStartWaitingMessageBlinkTimer = 0.0f;
                    gameStartWaitingTime = 0.0f;
                }
            }
        }

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
                    if (game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids)) {
                        // Safe to respawn immediately
                        ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                        waitingToRespawn = false;
                    } else {
                        // Wait until the spawn area is clear
                        waitingToRespawn = true;
                        respawnAsteroidProfile = asteroids;
                    }
                }
            }
        }

        // If waiting to respawn, check if position is safe now
        if (waitingToRespawn && game.getShipsRemaining() > 0) {
            if (game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids)) {
                ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                waitingToRespawn = false;
                respawnWaitingMessageBlinkTimer = 0.0f;  // Reset timer when respawning
                respawnWaitingTime = 0.0f;  // Reset waiting time counter
                respawnAsteroidProfile.clear();
            } else {
                // Update blink timer
                respawnWaitingMessageBlinkTimer += deltaSeconds;
                // Accumulate waiting time
                respawnWaitingTime += deltaSeconds;

                // If we've been waiting for 3+ seconds, regenerate asteroids
                if (respawnWaitingTime >= WAITING_REGENERATE_THRESHOLD) {
                    if (!respawnAsteroidProfile.empty()) {
                        game.repopulateAsteroidsPreservingProfile(asteroids, respawnAsteroidProfile);
                    } else {
                        game.populateAsteroids(asteroids, game.getAsteroidCountForCurrentLevel());
                    }
                    respawnWaitingTime = 0.0f;  // Reset counter to try again
                }
            }
        }

        // If game hasn't started yet (initial spawn check), check if position is safe now
        if (!gameStarted && !insertCoinScreen) {
            if (game.isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, asteroids)) {
                gameStarted = true;
                gameStartWaitingMessageBlinkTimer = 0.0f;  // Reset timer when game starts
                gameStartWaitingTime = 0.0f;  // Reset waiting time counter
            } else {
                // Update blink timer while waiting
                gameStartWaitingMessageBlinkTimer += deltaSeconds;
                // Accumulate waiting time
                gameStartWaitingTime += deltaSeconds;

                // If we've been waiting for 3+ seconds, regenerate asteroids
                if (gameStartWaitingTime >= WAITING_REGENERATE_THRESHOLD) {
                    game.populateAsteroids(asteroids, game.getAsteroidCountForCurrentLevel());
                    gameStartWaitingTime = 0.0f;  // Reset counter to try again
                }
            }
        }

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

        // Update bolts and remove off-screen ones
        for (auto& bolt : bolts) {
            bolt.update(deltaSeconds, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        bolts.erase(
            std::remove_if(bolts.begin(), bolts.end(),
                [](const Bolt& bolt) { return bolt.isOffScreen(SCREEN_WIDTH, SCREEN_HEIGHT); }),
            bolts.end()
        );

        // Collision detection - bolts vs asteroids
        if (gameStarted && !gameOver) {
            game.handleBoltAsteroidCollisions(bolts, asteroids);
        }

        // Collision detection - ship vs asteroids (only if game started, ship is not destroyed, not waiting, and game not over)
        if (gameStarted && !shipDestroyed && !waitingToRespawn && !gameOver && !insertCoinScreen) {
            if (game.handleShipAsteroidCollisions(ship, asteroids, SHIP_CENTER_X, SHIP_CENTER_Y)) {
                // Collision occurred - create explosion
                shipExplosion = std::make_unique<ShipExplosion>(ship.getX(), ship.getY(), ship.getOrientation());
                shipDestroyed = true;
            }
        }

        if (gameStarted && !gameOver && !shipDestroyed && !waitingToRespawn && !levelClearedMessageVisible && !insertCoinScreen && asteroids.empty()) {
            const int clearedLevel = game.getCurrentLevel();
            game.addScore(100 * clearedLevel);
            levelClearedMessageVisible = true;
            levelClearedMessageTimer = 0.0f;
            bolts.clear();
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
        const int shipsRemaining = renderHeader(renderer, game, SHIP_COLOR);

        // Render ship or explosion or game over
        if (insertCoinScreen) {
            // INSERT COIN attract screen
            constexpr float INSERT_COIN_SCALE = 4.0f;
            constexpr float SUBTITLE_SCALE = 1.5f;
            constexpr float BLINK_PERIOD = 1.0f;

            // Blink "INSERT COIN" on a 1-second cycle
            if (std::fmod(insertCoinBlinkTimer, BLINK_PERIOD) < BLINK_PERIOD / 2.0f) {
                TextRenderer::renderText(renderer, "INSERT COIN",
                                        SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 20.0f,
                                        INSERT_COIN_SCALE, Colors::SILVER,
                                        TextRenderer::Alignment::CENTER);
            }
            TextRenderer::renderText(renderer, "PRESS 'C' TO INSERT COIN",
                                     SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 50.0f,
                                     SUBTITLE_SCALE, Colors::SILVER,
                                     TextRenderer::Alignment::CENTER);
        } else if (shipsRemaining == 0 && !shipExplosion) {
            // Game Over message
            constexpr float GAME_OVER_SCALE = 5.0f;
            TextRenderer::renderText(renderer, "GAME OVER",
                                    SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 25.0f,
                                    GAME_OVER_SCALE, Colors::SILVER,
                                    TextRenderer::Alignment::CENTER);
        } else if (levelClearedMessageVisible) {
            constexpr float LEVEL_CLEARED_SCALE = 4.0f;
            TextRenderer::renderText(renderer, "LEVEL CLEARED",
                                    SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 25.0f,
                                    LEVEL_CLEARED_SCALE, Colors::SILVER,
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

        // Render bolts
        for (const auto& bolt : bolts) {
            bolt.render(renderer, Colors::SILVER);
        }

        // Render blinking "WAITING TO SAFELY RESPAWN" message if applicable
        if (waitingToRespawn) {
            constexpr float BLINK_PERIOD = 0.8f;  // Blink cycle duration (0.4s visible, 0.4s hidden)
            constexpr float MESSAGE_Y = SCREEN_HEIGHT - 50.0f;
            constexpr float MESSAGE_SCALE = 1.2f;

            // Toggle visibility every half period
            if (std::fmod(respawnWaitingMessageBlinkTimer, BLINK_PERIOD) < BLINK_PERIOD / 2.0f) {
                TextRenderer::renderText(renderer, "WAITING TO SAFELY RESPAWN",
                                        SCREEN_WIDTH / 2.0f, MESSAGE_Y,
                                        MESSAGE_SCALE, Colors::SILVER,
                                        TextRenderer::Alignment::CENTER);
            }
        }

        // Render blinking "WAIT FOR SAFE DEPLOYMENT" message when game is waiting to start
        if (!gameStarted && !insertCoinScreen) {
            constexpr float BLINK_PERIOD = 0.8f;  // Blink cycle duration (0.4s visible, 0.4s hidden)
            constexpr float MESSAGE_Y = SCREEN_HEIGHT - 50.0f;
            constexpr float MESSAGE_SCALE = 1.2f;

            // Toggle visibility every half period
            if (std::fmod(gameStartWaitingMessageBlinkTimer, BLINK_PERIOD) < BLINK_PERIOD / 2.0f) {
                TextRenderer::renderText(renderer, "WAIT FOR SAFE DEPLOYMENT",
                                        SCREEN_WIDTH / 2.0f, MESSAGE_Y,
                                        MESSAGE_SCALE, Colors::SILVER,
                                        TextRenderer::Alignment::CENTER);
            }
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

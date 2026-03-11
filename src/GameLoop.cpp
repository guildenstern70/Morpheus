//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//


#include "GameLoop.h"
#include "colors.h"
#include "config.h"
#include "TextRenderer.h"
#include <algorithm>
#include <cmath>

GameLoop::GameLoop()
    : m_ship(SHIP_CENTER_X, SHIP_CENTER_Y) {

    // Initialize asteroid field for the first level
    m_game.populateAsteroids(m_asteroids, m_game.getAsteroidCountForCurrentLevel());

    // Initial safe-deploy check
    constexpr float SPAWN_SAFETY_RADIUS = 50.0f;
    m_gameStarted = Game::isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, SPAWN_SAFETY_RADIUS, m_asteroids);
}

void GameLoop::resetGamePreserveHighScore(bool showInsertCoin) {
    const int savedHighScore = m_game.getHighScore();
    m_game = Game();
    m_game.setHighScore(savedHighScore);
    m_ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
    m_asteroids.clear();
    m_game.populateAsteroids(m_asteroids, m_game.getAsteroidCountForCurrentLevel());
    m_bolts.clear();
    m_shipExplosion = nullptr;
    m_shipDestroyed = false;
    m_waitingToRespawn = false;
    m_levelClearedMessageVisible = false;
    m_levelClearedMessageTimer = 0.0f;
    m_timeSinceLastFire = m_fireCooldown;
    m_gameOverDisplayTimer = 0.0f;
    m_insertCoinBlinkTimer = 0.0f;
    m_respawnWaitingTime = 0.0f;
    m_gameStartWaitingTime = 0.0f;
    m_respawnWaitingMessageBlinkTimer = 0.0f;
    m_gameStartWaitingMessageBlinkTimer = 0.0f;
    m_respawnAsteroidProfile.clear();
    m_gameStarted = Game::isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, 50.0f, m_asteroids);
    m_insertCoinScreen = showInsertCoin;
    // Reset any invulnerability state
    m_shipInvulnerable = false;
    m_shipInvulnerabilityTimer = 0.0f;
    m_shipInvulnerabilityBlinkTimer = 0.0f;
}

int GameLoop::renderHeader(SDL_Renderer* renderer, const Colors::Color& SHIP_COLOR) const {
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
    const std::string scoreText = "SCORE " + std::to_string(m_game.getScore());
    TextRenderer::renderText(renderer, scoreText,
                             MARGIN, INFO_Y,
                             SUBHEADER_SCALE, Colors::SILVER,
                             TextRenderer::Alignment::LEFT);

    // Lives: Draw ship icons centered
    const int shipsRemaining = m_game.getShipsRemaining();
    constexpr float SHIP_ICON_SPACING = 20.0f;
    const float totalIconWidth = static_cast<float>(shipsRemaining) * SHIP_ICON_SPACING;
    const float startX = (SCREEN_WIDTH / 2.0f) - (totalIconWidth / 2.0f) + (SHIP_ICON_SPACING / 2.0f);

    SDL_SetRenderDrawColor(renderer,
                           SHIP_COLOR.r,
                           SHIP_COLOR.g,
                           SHIP_COLOR.b,
                           SHIP_COLOR.a);

    for (int i = 0; i < shipsRemaining; ++i) {
        constexpr float SHIP_ICON_SCALE = 0.8f;
        Ship::renderIcon(renderer, startX + static_cast<float>(i) * SHIP_ICON_SPACING, INFO_Y + 10.0f, SHIP_ICON_SCALE);
    }

    const std::string highScoreText = "HIGH " + std::to_string(m_game.getHighScore());
    TextRenderer::renderText(renderer, highScoreText,
                             SCREEN_WIDTH - MARGIN, INFO_Y,
                             SUBHEADER_SCALE, Colors::SILVER,
                             TextRenderer::Alignment::RIGHT);
    return shipsRemaining;
}

void GameLoop::mainLoop(SDL_Window* window,
                       SDL_Renderer* renderer) {
    Uint64 previousTicks = SDL_GetTicks();

    // Local constant reused from original mainLoop
    constexpr float FIRE_COOLDOWN = 0.25f;
    m_timeSinceLastFire = m_fireCooldown; // start ready to fire

    // Main loop
    while (m_running) {
        const Uint64 frameStartTicks = SDL_GetTicks();

        // Check if game is over (needed for event handling)
        const bool gameOver = (m_game.getShipsRemaining() == 0 && !m_shipExplosion && !m_insertCoinScreen);

        while (SDL_PollEvent(&m_event)) {
            switch (m_event.type) {
                case SDL_EVENT_QUIT:
                    m_running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (m_event.key.key == SDLK_ESCAPE) {
                        m_running = false;
                    } else if (m_event.key.key == SDLK_C) {
                            if (m_insertCoinScreen) {
                                resetGamePreserveHighScore(false);
                            }
                    } else if (m_event.key.key == SDLK_LEFT) {
                        m_turningLeft = true;
                    } else if (m_event.key.key == SDLK_RIGHT) {
                        m_turningRight = true;
                    } else if (m_event.key.key == SDLK_UP) {
                        m_thrusting = true;
                    } else if (m_event.key.key == SDLK_SPACE) {
                        // Fire bolt if game started, not destroyed, not waiting, not in level transition, and cooldown ready
                        if (m_gameStarted && !m_shipDestroyed && !m_waitingToRespawn && !m_levelClearedMessageVisible && !gameOver && m_timeSinceLastFire >= FIRE_COOLDOWN) {
                            // Calculate bolt starting position at ship's nose
                            const float shipAngleRad = m_ship.getOrientation() * (PI / 180.0f);
                            constexpr float SHIP_NOSE_OFFSET = 12.0f;  // Distance from center to nose
                            const float boltStartX = m_ship.getX() + std::sin(shipAngleRad) * SHIP_NOSE_OFFSET;
                            const float boltStartY = m_ship.getY() + (-std::cos(shipAngleRad)) * SHIP_NOSE_OFFSET;

                            m_bolts.emplace_back(boltStartX, boltStartY, m_ship.getOrientation(),
                                                 m_ship.getVelocityX(), m_ship.getVelocityY());
                            m_timeSinceLastFire = 0.0f;
                        }
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (m_event.key.key == SDLK_LEFT) {
                        m_turningLeft = false;
                    } else if (m_event.key.key == SDLK_RIGHT) {
                        m_turningRight = false;
                    } else if (m_event.key.key == SDLK_UP) {
                        m_thrusting = false;
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
        m_timeSinceLastFire += deltaSeconds;

        // Update INSERT COIN blink timer
        m_insertCoinBlinkTimer += deltaSeconds;

        // Update ship invulnerability timers (if active)
        if (m_shipInvulnerable) {
            m_shipInvulnerabilityTimer -= deltaSeconds;
            m_shipInvulnerabilityBlinkTimer += deltaSeconds;
            if (m_shipInvulnerabilityTimer <= 0.0f) {
                m_shipInvulnerable = false;
                m_shipInvulnerabilityTimer = 0.0f;
                m_shipInvulnerabilityBlinkTimer = 0.0f;
            }
        }

        // After game over, wait a few seconds then transition to INSERT COIN screen
        if (gameOver) {
            m_gameOverDisplayTimer += deltaSeconds;
            if (m_gameOverDisplayTimer >= GAME_OVER_DISPLAY_DURATION) {
                resetGamePreserveHighScore(true);
            }
        }

        if (m_levelClearedMessageVisible) {
            m_levelClearedMessageTimer += deltaSeconds;
            if (m_levelClearedMessageTimer >= LEVEL_CLEARED_MESSAGE_DURATION) {
                m_levelClearedMessageVisible = false;
                m_levelClearedMessageTimer = 0.0f;
                m_game.advanceLevel();
                m_game.populateAsteroids(m_asteroids, m_game.getAsteroidCountForCurrentLevel());

                // Re-run safe deployment check for the new wave.
                m_gameStarted = Game::isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, 50.0f, m_asteroids);
                if (!m_gameStarted) {
                    m_gameStartWaitingMessageBlinkTimer = 0.0f;
                    m_gameStartWaitingTime = 0.0f;
                }
            }
        }

        // Update explosion animation if active
        if (m_shipExplosion) {
            m_shipExplosion->update(deltaSeconds);
            if (m_shipExplosion->isFinished()) {
                // Explosion finished
                m_shipExplosion = nullptr;
                m_shipDestroyed = false;

                // Only reset ship if there are lives remaining
                if (m_game.getShipsRemaining() > 0) {
                    // Check if spawn position is safe
                    if (Game::isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, 50.0f, m_asteroids)) {
                        // Safe to respawn immediately
                        m_ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                        m_waitingToRespawn = false;
                        // Grant brief invulnerability to the newly spawned ship
                        m_shipInvulnerable = true;
                        m_shipInvulnerabilityTimer = SHIP_INVULNERABILITY_DURATION;
                        m_shipInvulnerabilityBlinkTimer = 0.0f;
                    } else {
                        // Wait until the spawn area is clear
                        m_waitingToRespawn = true;
                        m_respawnAsteroidProfile = m_asteroids;
                    }
                }
            }
        }

        // If waiting to respawn, check if position is safe now
        if (m_waitingToRespawn && m_game.getShipsRemaining() > 0) {
                if (Game::isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, 50.0f, m_asteroids)) {
                m_ship = Ship(SHIP_CENTER_X, SHIP_CENTER_Y);
                m_waitingToRespawn = false;
                m_respawnWaitingMessageBlinkTimer = 0.0f;  // Reset timer when respawning
                m_respawnWaitingTime = 0.0f;  // Reset waiting time counter
                m_respawnAsteroidProfile.clear();
                // Grant brief invulnerability to the newly spawned ship
                m_shipInvulnerable = true;
                m_shipInvulnerabilityTimer = SHIP_INVULNERABILITY_DURATION;
                m_shipInvulnerabilityBlinkTimer = 0.0f;
            } else {
                // Update blink timer
                m_respawnWaitingMessageBlinkTimer += deltaSeconds;
                // Accumulate waiting time
                m_respawnWaitingTime += deltaSeconds;

                // If we've been waiting for 3+ seconds, regenerate asteroids
                if (m_respawnWaitingTime >= WAITING_REGENERATE_THRESHOLD) {
                    if (!m_respawnAsteroidProfile.empty()) {
                        m_game.repopulateAsteroidsPreservingProfile(m_asteroids, m_respawnAsteroidProfile);
                    } else {
                        m_game.populateAsteroids(m_asteroids, m_game.getAsteroidCountForCurrentLevel());
                    }
                    m_respawnWaitingTime = 0.0f;  // Reset counter to try again
                }
            }
        }

        // If game hasn't started yet (initial spawn check), check if position is safe now
        if (!m_gameStarted && !m_insertCoinScreen) {
            if (Game::isPositionSafe(SHIP_CENTER_X, SHIP_CENTER_Y, 50.0f, m_asteroids)) {
                m_gameStarted = true;
                m_gameStartWaitingMessageBlinkTimer = 0.0f;  // Reset timer when game starts
                m_gameStartWaitingTime = 0.0f;  // Reset waiting time counter
            } else {
                // Update blink timer while waiting
                m_gameStartWaitingMessageBlinkTimer += deltaSeconds;
                // Accumulate waiting time
                m_gameStartWaitingTime += deltaSeconds;

                // If we've been waiting for 3+ seconds, regenerate asteroids
                if (m_gameStartWaitingTime >= WAITING_REGENERATE_THRESHOLD) {
                    m_game.populateAsteroids(m_asteroids, m_game.getAsteroidCountForCurrentLevel());
                    m_gameStartWaitingTime = 0.0f;  // Reset counter to try again
                }
            }
        }

        // Only update ship if game has started, not destroyed, not waiting to respawn, and game is not over
        if (m_gameStarted && !m_shipDestroyed && !m_waitingToRespawn && !gameOver) {
            m_ship.update(m_turningLeft,
                          m_turningRight,
                          m_thrusting,
                          deltaSeconds,
                          SHIP_TURN_SPEED_DEG_PER_SEC,
                          SHIP_THRUST_ACCELERATION,
                          SCREEN_WIDTH,
                          SCREEN_HEIGHT);
        }

        // Update asteroids
        for (auto& asteroid : m_asteroids) {
            asteroid.update(deltaSeconds, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // Update bolts and remove off-screen ones
        for (auto& bolt : m_bolts) {
            bolt.update(deltaSeconds, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        m_bolts.erase(
            std::remove_if(m_bolts.begin(), m_bolts.end(),
                [](const Bolt& bolt) { return bolt.isOffScreen(SCREEN_WIDTH, SCREEN_HEIGHT); }),
            m_bolts.end()
        );

        // Collision detection - bolts vs asteroids
        if (m_gameStarted && !gameOver) {
            m_game.handleBoltAsteroidCollisions(m_bolts, m_asteroids);
        }

        // Collision detection - ship vs asteroids
        // Only check ship collisions if not invulnerable
        if (m_gameStarted && !m_shipDestroyed && !m_waitingToRespawn && !gameOver && !m_insertCoinScreen && !m_shipInvulnerable) {
            if (m_game.handleShipAsteroidCollisions(m_ship, m_asteroids, SHIP_CENTER_X, SHIP_CENTER_Y)) {
                // Collision occurred - create explosion
                m_shipExplosion = std::make_unique<ShipExplosion>(m_ship.getX(), m_ship.getY(), m_ship.getOrientation());
                m_shipDestroyed = true;
            }
        }

        if (m_gameStarted && !gameOver && !m_shipDestroyed && !m_waitingToRespawn && !m_levelClearedMessageVisible && !m_insertCoinScreen && m_asteroids.empty()) {
            const int clearedLevel = m_game.getCurrentLevel();
            m_game.addScore(100 * clearedLevel);
            m_levelClearedMessageVisible = true;
            m_levelClearedMessageTimer = 0.0f;
            m_bolts.clear();
        }

        // Collision detection - asteroid vs asteroid (elastic bounce)
        Game::handleAsteroidAsteroidCollisions(m_asteroids);

        SDL_SetRenderDrawColor(renderer,
            BACKGROUND_COLOR.r,
            BACKGROUND_COLOR.g,
            BACKGROUND_COLOR.b,
            BACKGROUND_COLOR.a);
        SDL_RenderClear(renderer);

        // Render header
        const int shipsRemaining = renderHeader(renderer, SHIP_COLOR);

        // Render ship or explosion or game over
        if (m_insertCoinScreen) {
            // INSERT COIN attract screen
            constexpr float INSERT_COIN_SCALE = 4.0f;
            constexpr float SUBTITLE_SCALE = 1.5f;
            constexpr float BLINK_PERIOD = 1.0f;

            // Blink "INSERT COIN" on a 1-second cycle
            if (std::fmod(m_insertCoinBlinkTimer, BLINK_PERIOD) < BLINK_PERIOD / 2.0f) {
                TextRenderer::renderText(renderer, "INSERT COIN",
                                        SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 20.0f,
                                        INSERT_COIN_SCALE, Colors::SILVER,
                                        TextRenderer::Alignment::CENTER);
            }
            TextRenderer::renderText(renderer, "PRESS 'C' TO INSERT COIN",
                                     SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 50.0f,
                                     SUBTITLE_SCALE, Colors::SILVER,
                                     TextRenderer::Alignment::CENTER);
        } else if (shipsRemaining == 0 && !m_shipExplosion) {
            // Game Over message
            constexpr float GAME_OVER_SCALE = 5.0f;
            TextRenderer::renderText(renderer, "GAME OVER",
                                    SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 25.0f,
                                    GAME_OVER_SCALE, Colors::SILVER,
                                    TextRenderer::Alignment::CENTER);
        } else if (m_levelClearedMessageVisible) {
            constexpr float LEVEL_CLEARED_SCALE = 4.0f;
            TextRenderer::renderText(renderer, "LEVEL CLEARED",
                                    SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 25.0f,
                                    LEVEL_CLEARED_SCALE, Colors::SILVER,
                                    TextRenderer::Alignment::CENTER);
        } else if (m_shipExplosion) {
            m_shipExplosion->render(renderer, SHIP_COLOR);
        } else if (m_gameStarted && !m_waitingToRespawn) {
            // Only render ship if game has started and not waiting to respawn
            // If ship is invulnerable, blink it according to SHIP_INVULNERABILITY_BLINK_PERIOD
            bool shouldRenderShip = true;
            if (m_shipInvulnerable) {
                const float period = SHIP_INVULNERABILITY_BLINK_PERIOD;
                // Toggle visibility on a period: visible for half period, invisible for half
                shouldRenderShip = std::fmod(m_shipInvulnerabilityBlinkTimer, period) < (period / 2.0f);
            }

            if (shouldRenderShip) {
                SDL_SetRenderDrawColor(renderer,
                    SHIP_COLOR.r,
                    SHIP_COLOR.g,
                    SHIP_COLOR.b,
                    SHIP_COLOR.a);
                m_ship.render(renderer, m_thrusting);
            }
        }

        for (auto& asteroid : m_asteroids) {
            asteroid.render(renderer, Colors::GRAY);
        }

        // Render bolts
        for (const auto& bolt : m_bolts) {
            bolt.render(renderer, Colors::SILVER);
        }

        // Render blinking "WAITING TO SAFELY RESPAWN" message if applicable
        if (m_waitingToRespawn) {
            constexpr float MESSAGE_Y = SCREEN_HEIGHT - 50.0f;

            // Toggle visibility every half period
            if (constexpr float BLINK_PERIOD = 0.8f; std::fmod(m_respawnWaitingMessageBlinkTimer, BLINK_PERIOD) < BLINK_PERIOD / 2.0f) {
                constexpr float MESSAGE_SCALE = 1.2f;
                TextRenderer::renderText(renderer, "WAITING TO SAFELY RESPAWN",
                                         SCREEN_WIDTH / 2.0f, MESSAGE_Y,
                                         MESSAGE_SCALE, Colors::SILVER,
                                         TextRenderer::Alignment::CENTER);
            }
        }

        // Render blinking "WAIT FOR SAFE DEPLOYMENT" message when game is waiting to start
        if (!m_gameStarted && !m_insertCoinScreen) {
            constexpr float BLINK_PERIOD = 0.8f;  // Blink cycle duration (0.4s visible, 0.4s hidden)
            constexpr float MESSAGE_Y = SCREEN_HEIGHT - 50.0f;
            constexpr float MESSAGE_SCALE = 1.2f;

            // Toggle visibility every half period
            if (std::fmod(m_gameStartWaitingMessageBlinkTimer, BLINK_PERIOD) < BLINK_PERIOD / 2.0f) {
                TextRenderer::renderText(renderer, "WAIT FOR SAFE DEPLOYMENT",
                                        SCREEN_WIDTH / 2.0f, MESSAGE_Y,
                                        MESSAGE_SCALE, Colors::SILVER,
                                        TextRenderer::Alignment::CENTER);
            }
        }

        SDL_RenderPresent(renderer);

        const Uint64 frameEndTicks = SDL_GetTicks();
        if (const auto frameElapsedMs = static_cast<float>(frameEndTicks - frameStartTicks);
            frameElapsedMs < TARGET_FRAME_TIME_MS) {
            SDL_Delay(static_cast<Uint32>(TARGET_FRAME_TIME_MS - frameElapsedMs));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

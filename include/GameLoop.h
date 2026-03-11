//
// Created by Alessio Saltarin on 11/03/26.
//

#pragma once

#include <SDL3/SDL_render.h>
#include <vector>
#include <memory>

#include "Game.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Bolt.h"
#include "ShipExplosion.h"

class GameLoop {
public:
    GameLoop();

    // Run the main game loop. This replaces the previous free function `mainLoop`.
    void mainLoop(SDL_Window* window, SDL_Renderer* renderer);

private:
    // Internal state previously local to mainLoop
    bool m_running = true;
    bool m_turningLeft = false;
    bool m_turningRight = false;
    bool m_thrusting = false;
    bool m_insertCoinScreen = true; // start on INSERT COIN

    Game m_game;
    SDL_Event m_event{};
    Ship m_ship;

    std::vector<Asteroid> m_asteroids;
    std::vector<Bolt> m_bolts;

    // Fire control
    const float m_fireCooldown = 0.25f;
    float m_timeSinceLastFire = m_fireCooldown;

    // Explosion / respawn state
    std::unique_ptr<ShipExplosion> m_shipExplosion = nullptr;
    bool m_shipDestroyed = false;
    bool m_waitingToRespawn = false;

    // Blink / waiting timers
    float m_respawnWaitingMessageBlinkTimer = 0.0f;
    float m_gameStartWaitingMessageBlinkTimer = 0.0f;
    float m_respawnWaitingTime = 0.0f;
    float m_gameStartWaitingTime = 0.0f;
    float m_insertCoinBlinkTimer = 0.0f;
    float m_gameOverDisplayTimer = 0.0f;

    bool m_levelClearedMessageVisible = false;
    float m_levelClearedMessageTimer = 0.0f;

    std::vector<Asteroid> m_respawnAsteroidProfile;
    bool m_gameStarted = false;

    // Respawn invulnerability
    bool m_shipInvulnerable = false;
    float m_shipInvulnerabilityTimer = 0.0f;
    float m_shipInvulnerabilityBlinkTimer = 0.0f;

    // Helper to render the header (score, lives, high score)
    int renderHeader(SDL_Renderer* renderer, const Colors::Color& shipColor) const;

    // Reset game state while preserving the high score. If showInsertCoin is true,
    // the INSERT COIN screen will be shown after reset.
    void resetGamePreserveHighScore(bool showInsertCoin);
};


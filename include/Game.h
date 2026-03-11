//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <vector>
#include "Asteroid.h"
#include "Ship.h"

class Bolt;  // Forward declaration

class Game {
public:
    Game();

    [[nodiscard]] int getScore() const;
    [[nodiscard]] int getShipsRemaining() const;
    [[nodiscard]] int getHighScore() const;
    [[nodiscard]] int getCurrentLevel() const;
    [[nodiscard]] int getAsteroidCountForCurrentLevel() const;

    void setScore(int score);
    void addScore(int points);
    void setShipsRemaining(int ships);
    void loseShip();
    void setHighScore(int highScore);
    void updateHighScore();
    void advanceLevel();
    void populateAsteroids(std::vector<Asteroid>& asteroids, int count) const;
    void repopulateAsteroidsPreservingProfile(std::vector<Asteroid>& asteroids, const std::vector<Asteroid>& profile) const;

    // Collision detection
    static bool checkCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2) ;
    static bool checkShipAsteroidCollision(const Ship& ship, const Asteroid& asteroid) ;
    static bool isPositionSafe(float x, float y, float safetyRadius, const std::vector<Asteroid>& asteroids) ;
    static std::vector<Asteroid> createFragments(const Asteroid& parent);

    // Collision handling
    bool handleShipAsteroidCollisions(Ship& ship, std::vector<Asteroid>& asteroids, float shipResetX, float shipResetY);
    static void handleAsteroidAsteroidCollisions(std::vector<Asteroid>& asteroids) ;
    void handleBoltAsteroidCollisions(std::vector<Bolt>& bolts, std::vector<Asteroid>& asteroids);

private:
    int m_score;
    int m_shipsRemaining;
    int m_highScore;
    int m_currentLevel;
};

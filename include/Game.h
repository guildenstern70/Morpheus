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

    int getScore() const;
    void setScore(int score);
    void addScore(int points);

    int getShipsRemaining() const;
    void setShipsRemaining(int ships);
    void loseShip();

    int getHighScore() const;
    void setHighScore(int highScore);
    void updateHighScore();

    int getCurrentLevel() const;
    int getAsteroidCountForCurrentLevel() const;
    void advanceLevel();

    void populateAsteroids(std::vector<Asteroid>& asteroids, int count);
    void repopulateAsteroidsPreservingProfile(std::vector<Asteroid>& asteroids, const std::vector<Asteroid>& profile);

    // Collision detection
    bool checkCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2) const;
    bool checkShipAsteroidCollision(const Ship& ship, const Asteroid& asteroid) const;
    bool isPositionSafe(float x, float y, float safetyRadius, const std::vector<Asteroid>& asteroids) const;
    std::vector<Asteroid> createFragments(const Asteroid& parent);

    // Collision handling
    bool handleShipAsteroidCollisions(Ship& ship, std::vector<Asteroid>& asteroids, float shipResetX, float shipResetY);
    void handleAsteroidAsteroidCollisions(std::vector<Asteroid>& asteroids) const;
    void handleBoltAsteroidCollisions(std::vector<Bolt>& bolts, std::vector<Asteroid>& asteroids);

private:
    int m_score;
    int m_shipsRemaining;
    int m_highScore;
    int m_currentLevel;
};

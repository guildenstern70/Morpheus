//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Game.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game()
    : m_score(0),
      m_shipsRemaining(3),
      m_highScore(0) {
    srand(static_cast<unsigned>(time(nullptr)));
}

int Game::getScore() const {
    return m_score;
}

void Game::setScore(int score) {
    m_score = score;
    updateHighScore();
}

void Game::addScore(int points) {
    m_score += points;
    updateHighScore();
}

int Game::getShipsRemaining() const {
    return m_shipsRemaining;
}

void Game::setShipsRemaining(int ships) {
    m_shipsRemaining = ships;
}

void Game::loseShip() {
    if (m_shipsRemaining > 0) {
        m_shipsRemaining--;
    }
}

int Game::getHighScore() const {
    return m_highScore;
}

void Game::updateHighScore() {
    m_highScore = std::max(m_highScore, m_score);
}

void Game::populateAsteroids(std::vector<Asteroid>& asteroids, int count) {
    constexpr float MIN_VELOCITY = 2.0f;
    constexpr float MAX_VELOCITY = 6.0f;
    constexpr float VELOCITY_RANGE = MAX_VELOCITY - MIN_VELOCITY;

    asteroids.clear();

    for (int i = 0; i < count; ++i) {
        // Random position across screen
        const float x = static_cast<float>(rand() % 800);
        const float y = static_cast<float>(rand() % 600);

        // Random velocity between 1.0 and 5.0
        const float velocityMagnitude = MIN_VELOCITY + (static_cast<float>(rand()) / RAND_MAX) * VELOCITY_RANGE;
        const float velocityAngle = (static_cast<float>(rand()) / RAND_MAX) * 360.0f;
        const float angleRadians = velocityAngle * (3.14159265358979323846f / 180.0f);
        const float velocityX = std::cos(angleRadians) * velocityMagnitude;
        const float velocityY = std::sin(angleRadians) * velocityMagnitude;

        // Random size
        const Asteroid::Size sizes[] = {Asteroid::Size::SMALL, Asteroid::Size::MEDIUM, Asteroid::Size::LARGE};
        const Asteroid::Size size = sizes[rand() % 3];

        // Random shape
        const Asteroid::Shape shapes[] = {Asteroid::Shape::SHAPE_A, Asteroid::Shape::SHAPE_B};
        const Asteroid::Shape shape = shapes[rand() % 2];

        asteroids.emplace_back(x, y, velocityX, velocityY, size, shape);
    }
}

bool Game::checkCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2) const {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float distanceSquared = dx * dx + dy * dy;
    const float radiusSum = r1 + r2;
    return distanceSquared < (radiusSum * radiusSum);
}

bool Game::checkShipAsteroidCollision(const Ship& ship, const Asteroid& asteroid) const {
    return checkCircleCollision(
        ship.getX(), ship.getY(), 10.0f,  // SHIP_COLLISION_RADIUS
        asteroid.getX(), asteroid.getY(), asteroid.getRadius()
    );
}

std::vector<Asteroid> Game::createFragments(const Asteroid& parent) const {
    std::vector<Asteroid> fragments;

    Asteroid::Size fragmentSize;
    bool shouldFragment = false;

    if (parent.getRadius() == 48.0f) {  // LARGE
        fragmentSize = Asteroid::Size::MEDIUM;
        shouldFragment = true;
    } else if (parent.getRadius() == 32.0f) {  // MEDIUM
        fragmentSize = Asteroid::Size::SMALL;
        shouldFragment = true;
    }

    if (!shouldFragment) {
        return fragments;  // Small asteroids don't fragment
    }

    const float baseAngle = (static_cast<float>(rand()) / RAND_MAX) * 360.0f;
    const float angleSpread = 45.0f;  // FRAGMENT_ANGLE_SPREAD_DEG
    const float impulseMag = 40.0f;   // FRAGMENT_IMPULSE_MAGNITUDE

    for (int i = 0; i < 3; ++i) {  // ASTEROID_FRAGMENTS_COUNT
        const float angle = baseAngle + (i - 1) * angleSpread;
        const float angleRad = angle * (3.14159265358979323846f / 180.0f);

        const float impulseX = std::cos(angleRad) * impulseMag;
        const float impulseY = std::sin(angleRad) * impulseMag;

        // Alternate shapes for variety
        const Asteroid::Shape shape = (i % 2 == 0) ? Asteroid::Shape::SHAPE_A : Asteroid::Shape::SHAPE_B;

        fragments.emplace_back(
            parent.getX(),
            parent.getY(),
            impulseX,
            impulseY,
            fragmentSize,
            shape
        );
    }

    return fragments;
}

//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Game.h"
#include "config.h"

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
    constexpr int MAX_PLACEMENT_ATTEMPTS = 100;
    constexpr float MIN_SEPARATION_MARGIN = 10.0f;  // Extra space between asteroids

    asteroids.clear();

    for (int i = 0; i < count; ++i) {
        bool positionFound = false;
        float x = 0.0f;
        float y = 0.0f;
        Asteroid::Size size = Asteroid::Size::SMALL;
        float radius = 0.0f;

        // Try to find a non-overlapping position
        for (int attempt = 0; attempt < MAX_PLACEMENT_ATTEMPTS; ++attempt) {
            // Random position across screen
            x = static_cast<float>(rand() % 800);
            y = static_cast<float>(rand() % 600);

            // Random size
            const Asteroid::Size sizes[] = {Asteroid::Size::SMALL, Asteroid::Size::MEDIUM, Asteroid::Size::LARGE};
            size = sizes[rand() % 3];

            // Get radius for this size
            if (size == Asteroid::Size::SMALL) {
                radius = ASTEROID_SMALL_RADIUS;
            } else if (size == Asteroid::Size::MEDIUM) {
                radius = ASTEROID_MEDIUM_RADIUS;
            } else {
                radius = ASTEROID_LARGE_RADIUS;
            }

            // Check if this position overlaps with any existing asteroid
            bool overlaps = false;
            for (const auto& existing : asteroids) {
                const float existingRadius = existing.getRadius();

                if (checkCircleCollision(x, y, radius + MIN_SEPARATION_MARGIN / 2.0f,
                                        existing.getX(), existing.getY(),
                                        existingRadius + MIN_SEPARATION_MARGIN / 2.0f)) {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps) {
                positionFound = true;
                break;
            }
        }

        // If we couldn't find a non-overlapping position after many attempts,
        // just place it anyway (this shouldn't happen with reasonable asteroid counts)
        if (!positionFound) {
            x = static_cast<float>(rand() % 800);
            y = static_cast<float>(rand() % 600);
        }

        // Random velocity between MIN_VELOCITY and MAX_VELOCITY
        const float velocityMagnitude = MIN_VELOCITY + (static_cast<float>(rand()) / RAND_MAX) * VELOCITY_RANGE;
        const float velocityAngle = (static_cast<float>(rand()) / RAND_MAX) * 360.0f;
        const float angleRadians = velocityAngle * (3.14159265358979323846f / 180.0f);
        const float velocityX = std::cos(angleRadians) * velocityMagnitude;
        const float velocityY = std::sin(angleRadians) * velocityMagnitude;


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
    return distanceSquared <= (radiusSum * radiusSum);
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

bool Game::handleShipAsteroidCollisions(Ship& ship, std::vector<Asteroid>& asteroids, float shipResetX, float shipResetY) {
    (void)shipResetX;  // Will be used after explosion animation
    (void)shipResetY;  // Will be used after explosion animation

    for (size_t i = 0; i < asteroids.size(); ++i) {
        if (checkShipAsteroidCollision(ship, asteroids[i])) {
            loseShip();
            // Create fragments from destroyed asteroid
            auto fragments = createFragments(asteroids[i]);
            asteroids.erase(asteroids.begin() + i);
            asteroids.insert(asteroids.end(), fragments.begin(), fragments.end());
            // Don't reset ship here - let the explosion animation play first
            return true;  // Collision occurred
        }
    }
    return false;  // No collision
}

void Game::handleAsteroidAsteroidCollisions(std::vector<Asteroid>& asteroids) {
    for (size_t i = 0; i < asteroids.size(); ++i) {
        for (size_t j = i + 1; j < asteroids.size(); ++j) {
            if (checkCircleCollision(
                asteroids[i].getX(), asteroids[i].getY(), asteroids[i].getRadius(),
                asteroids[j].getX(), asteroids[j].getY(), asteroids[j].getRadius())) {

                // Compute collision normal (unit vector from i to j)
                const float dx = asteroids[j].getX() - asteroids[i].getX();
                const float dy = asteroids[j].getY() - asteroids[i].getY();
                const float dist = std::sqrt(dx * dx + dy * dy);
                if (dist == 0.0f) continue;  // coincident centres — skip
                const float nx = dx / dist;
                const float ny = dy / dist;

                // Project velocities onto the collision normal
                const float v1n = asteroids[i].getVelocityX() * nx + asteroids[i].getVelocityY() * ny;
                const float v2n = asteroids[j].getVelocityX() * nx + asteroids[j].getVelocityY() * ny;

                // Only resolve if asteroids are moving toward each other
                if (v1n - v2n <= 0.0f) continue;

                // Equal-mass elastic collision: swap normal components
                const float dvx = (v2n - v1n) * nx;
                const float dvy = (v2n - v1n) * ny;

                asteroids[i].setVelocity(
                    asteroids[i].getVelocityX() + dvx,
                    asteroids[i].getVelocityY() + dvy);
                asteroids[j].setVelocity(
                    asteroids[j].getVelocityX() - dvx,
                    asteroids[j].getVelocityY() - dvy);
            }
        }
    }
}


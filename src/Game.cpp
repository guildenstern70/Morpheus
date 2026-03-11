//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Game.h"
#include "Bolt.h"
#include "config.h"
#include "Random.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game()
    : m_score(0),
      m_shipsRemaining(3),
      m_highScore(0),
      m_currentLevel(1) {
    // Seed the C++11 RNG once at game creation
    Random::seedFromRandomDevice();
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

void Game::setHighScore(int highScore) {
    m_highScore = highScore;
}

void Game::updateHighScore() {
    m_highScore = std::max(m_highScore, m_score);
}

int Game::getCurrentLevel() const {
    return m_currentLevel;
}

int Game::getAsteroidCountForCurrentLevel() const {
    return LEVEL_ONE_ASTEROID_COUNT + (m_currentLevel - 1) * LEVEL_ASTEROID_INCREMENT;
}

void Game::advanceLevel() {
    ++m_currentLevel;
}

void Game::populateAsteroids(std::vector<Asteroid>& asteroids, int count) const {
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
            x = static_cast<float>(Random::uniformInt(0, 800 - 1));
            y = static_cast<float>(Random::uniformInt(0, 600 - 1));

            // Random size
            constexpr Asteroid::Size sizes[] =
                { Asteroid::Size::SMALL,
                  Asteroid::Size::MEDIUM,
                  Asteroid::Size::LARGE };
            size = sizes[Random::uniformInt(0, 2)];

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
            x = static_cast<float>(Random::uniformInt(0, 800 - 1));
            y = static_cast<float>(Random::uniformInt(0, 600 - 1));
        }

        // Random velocity between MIN_VELOCITY and MAX_VELOCITY
        const float velocityMagnitude = MIN_VELOCITY + Random::uniformReal(0.0f, VELOCITY_RANGE);
        const float velocityAngle = Random::uniformReal(0.0f, 360.0f);
        const float angleRadians = velocityAngle * (PI / 180.0f);
        const float velocityX = std::cos(angleRadians) * velocityMagnitude;
        const float velocityY = std::sin(angleRadians) * velocityMagnitude;


        // Random shape among all available shapes
        constexpr Asteroid::Shape shapes[] = {Asteroid::Shape::SHAPE_A,
                                              Asteroid::Shape::SHAPE_B,
                                              Asteroid::Shape::SHAPE_C,
                                              Asteroid::Shape::SHAPE_D,
                                              Asteroid::Shape::SHAPE_E};
        const std::size_t shapesCount = sizeof(shapes) / sizeof(shapes[0]);
        const Asteroid::Shape shape = shapes[Random::choiceIndex(shapesCount)];

        asteroids.emplace_back(x, y, velocityX, velocityY, size, shape);
    }
}

void Game::repopulateAsteroidsPreservingProfile(std::vector<Asteroid>& asteroids, const std::vector<Asteroid>& profile) const {
    constexpr float MIN_VELOCITY = 2.0f;
    constexpr float MAX_VELOCITY = 6.0f;
    constexpr float VELOCITY_RANGE = MAX_VELOCITY - MIN_VELOCITY;
    constexpr int MAX_PLACEMENT_ATTEMPTS = 100;
    constexpr float MIN_SEPARATION_MARGIN = 10.0f;

    std::vector<Asteroid> regenerated;
    regenerated.reserve(profile.size());

    for (const auto& source : profile) {
        bool positionFound = false;
        float x = 0.0f;
        float y = 0.0f;
        const Asteroid::Size size = source.getSize();
        const Asteroid::Shape shape = source.getShape();

        float radius = ASTEROID_LARGE_RADIUS;
        if (size == Asteroid::Size::SMALL) {
            radius = ASTEROID_SMALL_RADIUS;
        } else if (size == Asteroid::Size::MEDIUM) {
            radius = ASTEROID_MEDIUM_RADIUS;
        }

        for (int attempt = 0; attempt < MAX_PLACEMENT_ATTEMPTS; ++attempt) {
            x = static_cast<float>(Random::uniformInt(0, SCREEN_WIDTH - 1));
            y = static_cast<float>(Random::uniformInt(0, SCREEN_HEIGHT - 1));

            bool overlaps = false;
            for (const auto& existing : regenerated) {
                if (checkCircleCollision(x, y, radius + MIN_SEPARATION_MARGIN / 2.0f,
                                         existing.getX(), existing.getY(),
                                         existing.getRadius() + MIN_SEPARATION_MARGIN / 2.0f)) {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps) {
                positionFound = true;
                break;
            }
        }

        if (!positionFound) {
            x = static_cast<float>(Random::uniformInt(0, SCREEN_WIDTH - 1));
            y = static_cast<float>(Random::uniformInt(0, SCREEN_HEIGHT - 1));
        }

        const float velocityMagnitude = MIN_VELOCITY + Random::uniformReal(0.0f, VELOCITY_RANGE);
        const float velocityAngle = Random::uniformReal(0.0f, 360.0f);
        const float angleRadians = velocityAngle * (PI / 180.0f);
        const float velocityX = std::cos(angleRadians) * velocityMagnitude;
        const float velocityY = std::sin(angleRadians) * velocityMagnitude;

        regenerated.emplace_back(x, y, velocityX, velocityY, size, shape);
    }

    asteroids = std::move(regenerated);
}

bool Game::checkCircleCollision(const float x1,
                                const float y1,
                                const float r1,
                                const float x2,
                                const float y2,
                                const float r2) {
    const float dx = x2 - x1;
    const float dy = y2 - y1;
    const float distanceSquared = dx * dx + dy * dy;
    // Use 0.85 factor to reduce effective collision radius
    // This better matches the irregular polygon shapes
    const float effectiveR1 = r1 * 0.85f;
    const float effectiveR2 = r2 * 0.85f;
    const float radiusSum = effectiveR1 + effectiveR2;
    return distanceSquared <= (radiusSum * radiusSum);
}

bool Game::checkShipAsteroidCollision(const Ship& ship, const Asteroid& asteroid) {
    return checkCircleCollision(
        ship.getX(), ship.getY(), 10.0f,  // SHIP_COLLISION_RADIUS
        asteroid.getX(), asteroid.getY(), asteroid.getRadius()
    );
}

bool Game::isPositionSafe(const float x,
                          const float y,
                          const float safetyRadius,
                          const std::vector<Asteroid>& asteroids) {
    for (const auto& asteroid : asteroids) {
        if (checkCircleCollision(x, y, safetyRadius,
                                asteroid.getX(), asteroid.getY(), asteroid.getRadius())) {
            return false;  // Position is not safe
        }
    }
    return true;  // Position is safe
}

std::vector<Asteroid> Game::createFragments(const Asteroid& parent) {
    std::vector<Asteroid> fragments;

    Asteroid::Size fragmentSize;
    bool shouldFragment = false;
    int fragmentCount = 0;

    if (parent.getRadius() == 48.0f) {  // LARGE
        fragmentSize = Asteroid::Size::MEDIUM;
        shouldFragment = true;
        fragmentCount = 3;  // LARGE breaks into 3 MEDIUM pieces
    } else if (parent.getRadius() == 32.0f) {  // MEDIUM
        fragmentSize = Asteroid::Size::SMALL;
        shouldFragment = true;
        fragmentCount = 5;  // MEDIUM breaks into 5 SMALL pieces
    }

    if (!shouldFragment) {
        return fragments;  // Small asteroids don't fragment
    }

    const float baseAngle = Random::uniformReal(0.0f, 360.0f);
    const float angleSpread = 360.0f / static_cast<float>(fragmentCount);  // Evenly spread fragments
    const float impulseMag = 40.0f;   // FRAGMENT_IMPULSE_MAGNITUDE

    for (int i = 0; i < fragmentCount; ++i) {
        const float angle = baseAngle + static_cast<float>(i) * angleSpread;
        const float angleRad = angle * (PI / 180.0f);

        const float impulseX = std::cos(angleRad) * impulseMag;
        const float impulseY = std::sin(angleRad) * impulseMag;

        // Alternate shapes for variety among fragments
        const Asteroid::Shape shape = static_cast<Asteroid::Shape>( (i % 5) );

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

void Game::handleBoltAsteroidCollisions(std::vector<Bolt>& bolts, std::vector<Asteroid>& asteroids) {
    // Iterate through bolts
    for (size_t boltIndex = 0; boltIndex < bolts.size(); ) {
        bool boltHit = false;

        // Check collision with asteroids
        for (size_t asteroidIndex = 0; asteroidIndex < asteroids.size(); ) {
            if (checkCircleCollision(
                bolts[boltIndex].getX(),
                bolts[boltIndex].getY(),
                bolts[boltIndex].getRadius(),
                asteroids[asteroidIndex].getX(),
                asteroids[asteroidIndex].getY(),
                asteroids[asteroidIndex].getRadius())) {

                // Bolt hit asteroid!
                boltHit = true;

                // Create fragments based on asteroid size
                auto fragments = createFragments(asteroids[asteroidIndex]);

                // Award points based on asteroid size
                if (asteroids[asteroidIndex].getRadius() == 48.0f) {  // LARGE
                    addScore(20);
                } else if (asteroids[asteroidIndex].getRadius() == 32.0f) {  // MEDIUM
                    addScore(50);
                } else {  // SMALL
                    addScore(100);
                }

                // Remove the hit asteroid
                asteroids.erase(asteroids.begin() + static_cast<std::ptrdiff_t>(asteroidIndex));

                // Add fragments
                asteroids.insert(asteroids.end(), fragments.begin(), fragments.end());

                // Don't increment asteroidIndex since we erased an element
                break;  // Bolt can only hit one asteroid, move to next bolt
            } else {
                ++asteroidIndex;
            }
        }

        // Remove bolt if it hit an asteroid
        if (boltHit) {
            bolts.erase(bolts.begin() + static_cast<std::ptrdiff_t>(boltIndex));
        } else {
            ++boltIndex;
        }
    }
}


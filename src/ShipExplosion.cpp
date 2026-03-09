//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "ShipExplosion.h"
#include <cmath>
#include <cstdlib>

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

ShipExplosion::ShipExplosion(float shipX, float shipY, float shipOrientation)
    : m_elapsedTime(0.0f) {
    createDebris(shipX, shipY, shipOrientation);
}

void ShipExplosion::update(float deltaSeconds) {
    m_elapsedTime += deltaSeconds;

    for (auto& particle : m_particles) {
        particle.x += particle.velocityX * deltaSeconds;
        particle.y += particle.velocityY * deltaSeconds;
        particle.rotation += particle.rotationSpeed * deltaSeconds;
    }
}

void ShipExplosion::render(SDL_Renderer* renderer, const Colors::Color& color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    for (const auto& particle : m_particles) {
        const float angleRadians = particle.rotation * (kPi / 180.0f);
        const float cosA = std::cos(angleRadians);
        const float sinA = std::sin(angleRadians);

        std::vector<SDL_FPoint> transformedPoints;
        transformedPoints.reserve(particle.shape.size());

        for (const auto& point : particle.shape) {
            SDL_FPoint transformed;
            transformed.x = particle.x + (point.x * cosA - point.y * sinA);
            transformed.y = particle.y + (point.x * sinA + point.y * cosA);
            transformedPoints.push_back(transformed);
        }

        for (size_t i = 0; i + 1 < transformedPoints.size(); ++i) {
            SDL_RenderLine(renderer,
                          transformedPoints[i].x,
                          transformedPoints[i].y,
                          transformedPoints[i + 1].x,
                          transformedPoints[i + 1].y);
        }
    }
}

bool ShipExplosion::isFinished() const {
    return m_elapsedTime >= EXPLOSION_DURATION;
}

float ShipExplosion::getElapsedTime() const {
    return m_elapsedTime;
}

void ShipExplosion::createDebris(float shipX, float shipY, float shipOrientation) {
    (void)shipOrientation;  // Reserved for future use (could rotate debris based on ship orientation)

    // Ship's original points in local coordinates
    const float noseX = 0.0f;
    const float noseY = -10.4f;
    const float leftWingX = -6.4f;
    const float leftWingY = 8.0f;
    const float rightWingX = 6.4f;
    const float rightWingY = 8.0f;

    // Create debris pieces with various shapes
    constexpr int NUM_PIECES = 8;
    constexpr float BASE_SPEED = 80.0f;
    constexpr float SPEED_VARIATION = 40.0f;

    for (int i = 0; i < NUM_PIECES; ++i) {
        Particle particle;

        // Random position around ship center (slightly spread)
        const float offsetAngle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * kPi;
        const float offsetDist = (static_cast<float>(rand()) / RAND_MAX) * 5.0f;
        particle.x = shipX + std::cos(offsetAngle) * offsetDist;
        particle.y = shipY + std::sin(offsetAngle) * offsetDist;

        // Random velocity radiating outward from ship
        const float angle = (static_cast<float>(i) / NUM_PIECES) * 2.0f * kPi +
                           (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
        const float speed = BASE_SPEED + (static_cast<float>(rand()) / RAND_MAX) * SPEED_VARIATION;
        particle.velocityX = std::cos(angle) * speed;
        particle.velocityY = std::sin(angle) * speed;

        // Random rotation
        particle.rotation = (static_cast<float>(rand()) / RAND_MAX) * 360.0f;
        particle.rotationSpeed = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 360.0f;

        // Create different shaped debris pieces
        switch (i % 5) {
            case 0: // Nose piece (triangle)
                particle.shape.push_back({noseX, noseY});
                particle.shape.push_back({-2.0f, -4.0f});
                particle.shape.push_back({2.0f, -4.0f});
                particle.shape.push_back({noseX, noseY});
                break;
            case 1: // Left wing piece
                particle.shape.push_back({leftWingX, leftWingY});
                particle.shape.push_back({-3.0f, 4.0f});
                particle.shape.push_back({-1.0f, 6.0f});
                particle.shape.push_back({leftWingX, leftWingY});
                break;
            case 2: // Right wing piece
                particle.shape.push_back({rightWingX, rightWingY});
                particle.shape.push_back({3.0f, 4.0f});
                particle.shape.push_back({1.0f, 6.0f});
                particle.shape.push_back({rightWingX, rightWingY});
                break;
            case 3: // Center piece
                particle.shape.push_back({-2.0f, 0.0f});
                particle.shape.push_back({2.0f, 0.0f});
                particle.shape.push_back({2.0f, 4.0f});
                particle.shape.push_back({-2.0f, 4.0f});
                particle.shape.push_back({-2.0f, 0.0f});
                break;
            case 4: // Small fragment
                particle.shape.push_back({-1.5f, -1.5f});
                particle.shape.push_back({1.5f, -1.5f});
                particle.shape.push_back({1.5f, 1.5f});
                particle.shape.push_back({-1.5f, -1.5f});
                break;
        }

        m_particles.push_back(particle);
    }
}




//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Ship.h"
#include "config.h"

#include <array>
#include <cmath>


Ship::Ship(float centerX, float centerY)
    : m_x(centerX),
      m_y(centerY),
      m_velocityX(0.0f),
      m_velocityY(0.0f),
      m_orientationDegrees(0.0f) {
}

void Ship::update(const bool turningLeft,
                  const bool turningRight,
                  const bool thrusting,
                  const float deltaSeconds,
                  const float turnSpeedDegPerSec,
                  const float thrustAcceleration,
                  const float screenWidth,
                  const float screenHeight) {
    if (turningRight) {
        m_orientationDegrees += turnSpeedDegPerSec * deltaSeconds;
    }
    if (turningLeft) {
        m_orientationDegrees -= turnSpeedDegPerSec * deltaSeconds;
    }

    normalizeOrientation();

    if (thrusting) {
        const float angleRadians = m_orientationDegrees * (PI / 180.0f);
        const float thrustX = std::sin(angleRadians) * thrustAcceleration * deltaSeconds;
        const float thrustY = -std::cos(angleRadians) * thrustAcceleration * deltaSeconds;
        m_velocityX += thrustX;
        m_velocityY += thrustY;
    }

    updatePosition(deltaSeconds);
    wrapPosition(screenWidth, screenHeight);
}

void Ship::render(SDL_Renderer* renderer, bool showThrust) const {
    const std::array<SDL_FPoint, 5> localPoints = {{{0.0f, -10.4f}, {-6.4f, 8.0f}, {0.0f, 3.2f}, {6.4f, 8.0f}, {0.0f, -10.4f}}};

    const float angleRadians = m_orientationDegrees * (PI / 180.0f);
    const float cosA = std::cos(angleRadians);
    const float sinA = std::sin(angleRadians);

    std::array<SDL_FPoint, 5> transformedPoints{};
    for (size_t i = 0; i < localPoints.size(); ++i) {
        const float x = localPoints[i].x;
        const float y = localPoints[i].y;
        transformedPoints[i].x = m_x + (x * cosA - y * sinA);
        transformedPoints[i].y = m_y + (x * sinA + y * cosA);
    }

    for (size_t i = 0; i + 1 < transformedPoints.size(); ++i) {
        SDL_RenderLine(renderer,
                       transformedPoints[i].x,
                       transformedPoints[i].y,
                       transformedPoints[i + 1].x,
                       transformedPoints[i + 1].y);
    }

    if (showThrust) {
        const std::array<SDL_FPoint, 4> thrustPoints = {{{-3.2f, 8.0f}, {0.0f, 12.8f}, {3.2f, 8.0f}, {-3.2f, 8.0f}}};

        std::array<SDL_FPoint, 4> transformedThrustPoints{};
        for (size_t i = 0; i < thrustPoints.size(); ++i) {
            const float x = thrustPoints[i].x;
            const float y = thrustPoints[i].y;
            transformedThrustPoints[i].x = m_x + (x * cosA - y * sinA);
            transformedThrustPoints[i].y = m_y + (x * sinA + y * cosA);
        }

        for (size_t i = 0; i + 1 < transformedThrustPoints.size(); ++i) {
            SDL_RenderLine(renderer,
                           transformedThrustPoints[i].x,
                           transformedThrustPoints[i].y,
                           transformedThrustPoints[i + 1].x,
                           transformedThrustPoints[i + 1].y);
        }
    }
}

void Ship::renderIcon(SDL_Renderer* renderer, float x, float y, float scale) {
    // Ship points in local coordinates (upward-pointing triangle)
    const std::array<SDL_FPoint, 5> localPoints = {
        {{0.0f, -10.4f}, {-6.4f, 8.0f}, {0.0f, 3.2f}, {6.4f, 8.0f}, {0.0f, -10.4f}}
    };

    std::array<SDL_FPoint, 5> transformedPoints{};
    for (size_t i = 0; i < localPoints.size(); ++i) {
        transformedPoints[i].x = x + localPoints[i].x * scale;
        transformedPoints[i].y = y + localPoints[i].y * scale;
    }

    for (size_t i = 0; i + 1 < transformedPoints.size(); ++i) {
        SDL_RenderLine(renderer,
                      transformedPoints[i].x,
                      transformedPoints[i].y,
                      transformedPoints[i + 1].x,
                      transformedPoints[i + 1].y);
    }
}

void Ship::normalizeOrientation() {
    m_orientationDegrees = std::fmod(m_orientationDegrees, 360.0f);
    if (m_orientationDegrees < 0.0f) {
        m_orientationDegrees += 360.0f;
    }
}

void Ship::updatePosition(float deltaSeconds) {
    m_x += m_velocityX * deltaSeconds;
    m_y += m_velocityY * deltaSeconds;
}

void Ship::wrapPosition(const float screenWidth, const float screenHeight) {
    // Wrap horizontally
    if (m_x < 0.0f) {
        m_x += screenWidth;
    } else if (m_x > screenWidth) {
        m_x -= screenWidth;
    }

    // Wrap vertically
    if (m_y < 0.0f) {
        m_y += screenHeight;
    } else if (m_y > screenHeight) {
        m_y -= screenHeight;
    }
}

float Ship::getX() const {
    return m_x;
}

float Ship::getY() const {
    return m_y;
}

float Ship::getOrientation() const {
    return m_orientationDegrees;
}

float Ship::getVelocityX() const {
    return m_velocityX;
}

float Ship::getVelocityY() const {
    return m_velocityY;
}


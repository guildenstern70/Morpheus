//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Ship.h"

#include <array>
#include <cmath>

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

Ship::Ship(float centerX, float centerY)
    : m_centerX(centerX),
      m_centerY(centerY),
      m_orientationDegrees(0.0f) {
}

void Ship::update(bool turningLeft,
                  bool turningRight,
                  float deltaSeconds,
                  float turnSpeedDegPerSec) {
    if (turningRight) {
        m_orientationDegrees += turnSpeedDegPerSec * deltaSeconds;
    }
    if (turningLeft) {
        m_orientationDegrees -= turnSpeedDegPerSec * deltaSeconds;
    }

    normalizeOrientation();
}

void Ship::render(SDL_Renderer* renderer) const {
    std::array<SDL_FPoint, 5> localPoints = {{{0.0f, -26.0f}, {-16.0f, 20.0f}, {0.0f, 8.0f}, {16.0f, 20.0f}, {0.0f, -26.0f}}};

    const float angleRadians = m_orientationDegrees * (kPi / 180.0f);
    const float cosA = std::cos(angleRadians);
    const float sinA = std::sin(angleRadians);

    std::array<SDL_FPoint, 5> transformedPoints{};
    for (size_t i = 0; i < localPoints.size(); ++i) {
        const float x = localPoints[i].x;
        const float y = localPoints[i].y;
        transformedPoints[i].x = m_centerX + (x * cosA - y * sinA);
        transformedPoints[i].y = m_centerY + (x * sinA + y * cosA);
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


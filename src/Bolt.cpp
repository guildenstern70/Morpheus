//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Bolt.h"
#include "config.h"
#include <cmath>

Bolt::Bolt(float x, float y, float angle, float shipVelocityX, float shipVelocityY)
    : m_x(x)
    , m_y(y)
{
    // Convert angle from degrees to radians
    const float angleRadians = angle * (PI / 180.0f);

    // Bolt moves in the direction the ship is facing plus ship's velocity
    // Ship's forward direction: sin(angle) for X, -cos(angle) for Y (matching ship thrust)
    m_velocityX = std::sin(angleRadians) * BOLT_SPEED + shipVelocityX;
    m_velocityY = (-std::cos(angleRadians)) * BOLT_SPEED + shipVelocityY;
}

void Bolt::update(const float deltaSeconds, [[maybe_unused]] float screenWidth, [[maybe_unused]] float screenHeight) {
    updatePosition(deltaSeconds);
    // Note: Bolts do NOT wrap around the screen - they go off-screen and get deleted
}

void Bolt::updatePosition(const float deltaSeconds) {
    m_x += m_velocityX * deltaSeconds;
    m_y += m_velocityY * deltaSeconds;
}

void Bolt::render(SDL_Renderer* renderer, const Colors::Color& color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw bolt as a small filled circle (sphere/point)
    constexpr float BOLT_RENDER_RADIUS = 2.0f;
    constexpr int CIRCLE_SEGMENTS = 8;  // Low segment count for small circle

    // Draw filled circle by drawing lines from center to edge points
    for (int i = 0; i < CIRCLE_SEGMENTS; ++i) {
        const float angle1 = (static_cast<float>(i) / CIRCLE_SEGMENTS) * 2.0f * PI;
        const float angle2 = (static_cast<float>(i + 1) / CIRCLE_SEGMENTS) * 2.0f * PI;

        const float x1 = m_x + std::cos(angle1) * BOLT_RENDER_RADIUS;
        const float y1 = m_y + std::sin(angle1) * BOLT_RENDER_RADIUS;
        const float x2 = m_x + std::cos(angle2) * BOLT_RENDER_RADIUS;
        const float y2 = m_y + std::sin(angle2) * BOLT_RENDER_RADIUS;

        // Draw triangle from center to edge segment
        SDL_RenderLine(renderer, m_x, m_y, x1, y1);
        SDL_RenderLine(renderer, x1, y1, x2, y2);
        SDL_RenderLine(renderer, x2, y2, m_x, m_y);
    }

    // Also draw the center point for brightness
    SDL_RenderPoint(renderer, m_x, m_y);
}

float Bolt::getX() const {
    return m_x;
}

float Bolt::getY() const {
    return m_y;
}

float Bolt::getRadius() {
    return BOLT_RADIUS;
}

bool Bolt::isOffScreen(float screenWidth, float screenHeight) const {
    constexpr float MARGIN = 50.0f;  // Small margin to ensure bolt is truly off-screen
    return (m_x < -MARGIN || m_x > screenWidth + MARGIN ||
            m_y < -MARGIN || m_y > screenHeight + MARGIN);
}






//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Asteroid.h"
#include "config.h"
#include "shapes.h"

#include <array>
#include <cmath>
#include <vector>


Asteroid::Asteroid(const float x,
    const float y,
    const float velocityX,
    const float velocityY,
    const Size size,
    const Shape shape)
    : m_x(x),
      m_y(y),
      m_velocityX(velocityX),
      m_velocityY(velocityY),
      m_rotationDegrees(0.0f),
      m_size(size),
      m_shape(shape) {
}

void Asteroid::update(const float deltaSeconds, const float screenWidth, const float screenHeight) {
    updatePosition(deltaSeconds);
    wrapPosition(screenWidth, screenHeight);
    m_rotationDegrees += ASTEROID_ROTATION_SPEED_DEG_PER_SEC * deltaSeconds;
    m_rotationDegrees = std::fmod(m_rotationDegrees, 360.0f);
}

namespace {
    struct ShapeData {
        const std::pair<float, float>* data;
        std::size_t count;
    };

    ShapeData getShapeData(Asteroid::Shape shape) {
        switch (shape) {
            case Asteroid::Shape::SHAPE_A: return { Shapes::ASTEROID_SHAPE_A.data(), Shapes::ASTEROID_SHAPE_A.size() };
            case Asteroid::Shape::SHAPE_B: return { Shapes::ASTEROID_SHAPE_B.data(), Shapes::ASTEROID_SHAPE_B.size() };
            case Asteroid::Shape::SHAPE_C: return { Shapes::ASTEROID_SHAPE_C.data(), Shapes::ASTEROID_SHAPE_C.size() };
            case Asteroid::Shape::SHAPE_D: return { Shapes::ASTEROID_SHAPE_D.data(), Shapes::ASTEROID_SHAPE_D.size() };
            case Asteroid::Shape::SHAPE_E: return { Shapes::ASTEROID_SHAPE_E.data(), Shapes::ASTEROID_SHAPE_E.size() };
            case Asteroid::Shape::SHAPE_F: return { Shapes::ASTEROID_SHAPE_F.data(), Shapes::ASTEROID_SHAPE_F.size() };
            case Asteroid::Shape::SHAPE_G: return { Shapes::ASTEROID_SHAPE_G.data(), Shapes::ASTEROID_SHAPE_G.size() };
            case Asteroid::Shape::SHAPE_H: return { Shapes::ASTEROID_SHAPE_H.data(), Shapes::ASTEROID_SHAPE_H.size() };
            case Asteroid::Shape::SHAPE_I: return { Shapes::ASTEROID_SHAPE_I.data(), Shapes::ASTEROID_SHAPE_I.size() };
        }
        return { Shapes::ASTEROID_SHAPE_A.data(), Shapes::ASTEROID_SHAPE_A.size() };
    }
}

void Asteroid::render(SDL_Renderer* renderer, const Colors::Color& color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const float radius = getRadiusForSize();
    const float angleRadians = m_rotationDegrees * (PI / 180.0f);
    const float cosA = std::cos(angleRadians);
    const float sinA = std::sin(angleRadians);

    const auto shapeData = getShapeData(m_shape);
    std::vector<SDL_FPoint> transformedPoints(shapeData.count);
    for (size_t i = 0; i < shapeData.count; ++i) {
        const float x = shapeData.data[i].first * radius;
        const float y = shapeData.data[i].second * radius;
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
}

void Asteroid::applyImpact(const float impulseX, const float impulseY) {
    m_velocityX += impulseX;
    m_velocityY += impulseY;
}

float Asteroid::getX() const {
    return m_x;
}

float Asteroid::getY() const {
    return m_y;
}

float Asteroid::getRadius() const {
    return getRadiusForSize();
}

float Asteroid::getVelocityX() const {
    return m_velocityX;
}

float Asteroid::getVelocityY() const {
    return m_velocityY;
}

Asteroid::Size Asteroid::getSize() const {
    return m_size;
}

Asteroid::Shape Asteroid::getShape() const {
    return m_shape;
}

void Asteroid::setVelocity(const float velocityX, const float velocityY) {
    m_velocityX = velocityX;
    m_velocityY = velocityY;
}

void Asteroid::updatePosition(const float deltaSeconds) {
    m_x += m_velocityX * deltaSeconds;
    m_y += m_velocityY * deltaSeconds;
}

void Asteroid::wrapPosition(const float screenWidth, const float screenHeight) {
    if (m_x < 0.0f) {
        m_x += screenWidth;
    } else if (m_x > screenWidth) {
        m_x -= screenWidth;
    }

    if (m_y < 0.0f) {
        m_y += screenHeight;
    } else if (m_y > screenHeight) {
        m_y -= screenHeight;
    }
}

float Asteroid::getRadiusForSize() const {
    switch (m_size) {
        case Size::SMALL:
            return ASTEROID_SMALL_RADIUS;
        case Size::MEDIUM:
            return ASTEROID_MEDIUM_RADIUS;
        case Size::LARGE:
            return ASTEROID_LARGE_RADIUS;
        default:
            return ASTEROID_LARGE_RADIUS;
    }
}


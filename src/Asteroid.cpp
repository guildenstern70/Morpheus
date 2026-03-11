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

void Asteroid::render(SDL_Renderer* renderer, const Colors::Color& color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const float radius = getRadiusForSize();
    const float angleRadians = m_rotationDegrees * (PI / 180.0f);
    const float cosA = std::cos(angleRadians);
    const float sinA = std::sin(angleRadians);

    // Select the shape array and size based on m_shape
    switch (m_shape) {
        case Shape::SHAPE_A: {
            const auto& shape = Shapes::ASTEROID_SHAPE_A;
            std::vector<SDL_FPoint> transformedPoints(shape.size());
            for (size_t i = 0; i < shape.size(); ++i) {
                const float x = shape[i].first * radius;
                const float y = shape[i].second * radius;
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
            break;
        }
        case Shape::SHAPE_B: {
            const auto& shape = Shapes::ASTEROID_SHAPE_B;
            std::vector<SDL_FPoint> transformedPoints(shape.size());
            for (size_t i = 0; i < shape.size(); ++i) {
                const float x = shape[i].first * radius;
                const float y = shape[i].second * radius;
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
            break;
        }
        case Shape::SHAPE_C: {
            const auto& shape = Shapes::ASTEROID_SHAPE_C;
            std::vector<SDL_FPoint> transformedPoints(shape.size());
            for (size_t i = 0; i < shape.size(); ++i) {
                const float x = shape[i].first * radius;
                const float y = shape[i].second * radius;
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
            break;
        }
        case Shape::SHAPE_D: {
            const auto& shape = Shapes::ASTEROID_SHAPE_D;
            std::vector<SDL_FPoint> transformedPoints(shape.size());
            for (size_t i = 0; i < shape.size(); ++i) {
                const float x = shape[i].first * radius;
                const float y = shape[i].second * radius;
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
            break;
        }
        case Shape::SHAPE_E: {
            const auto& shape = Shapes::ASTEROID_SHAPE_E;
            std::vector<SDL_FPoint> transformedPoints(shape.size());
            for (size_t i = 0; i < shape.size(); ++i) {
                const float x = shape[i].first * radius;
                const float y = shape[i].second * radius;
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
            break;
        }
        default:
            break;
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


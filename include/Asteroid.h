//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <SDL3/SDL.h>
#include "colors.h"

class Asteroid {
public:
    enum class Size {
        SMALL,
        MEDIUM,
        LARGE
    };

    enum class Shape {
        SHAPE_A,
        SHAPE_B
    };

    Asteroid(float x, float y, float velocityX, float velocityY, Size size, Shape shape);

    void update(float deltaSeconds, float screenWidth, float screenHeight);
    void render(SDL_Renderer* renderer, const Colors::Color& color) const;
    void applyImpact(float impulseX, float impulseY);

    float getX() const;
    float getY() const;
    float getRadius() const;
    float getVelocityX() const;
    float getVelocityY() const;
    void setVelocity(float velocityX, float velocityY);

private:
    float m_x;
    float m_y;
    float m_velocityX;
    float m_velocityY;
    float m_rotationDegrees;
    Size m_size;
    Shape m_shape;

    void updatePosition(float deltaSeconds);
    void wrapPosition(float screenWidth, float screenHeight);
    float getRadiusForSize() const;
    void renderShape(SDL_Renderer* renderer, const SDL_FPoint* points, size_t pointCount) const;
};



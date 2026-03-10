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

class Bolt {
public:
    Bolt(float x, float y, float angle, float shipVelocityX, float shipVelocityY);

    void update(float deltaSeconds, float screenWidth, float screenHeight);
    void render(SDL_Renderer* renderer, const Colors::Color& color) const;

    float getX() const;
    float getY() const;
    float getRadius() const;
    bool isOffScreen(float screenWidth, float screenHeight) const;

private:
    float m_x;
    float m_y;
    float m_velocityX;
    float m_velocityY;

    void updatePosition(float deltaSeconds);
};


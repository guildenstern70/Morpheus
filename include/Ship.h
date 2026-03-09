//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <SDL3/SDL.h>

class Ship {
public:
    Ship(float centerX, float centerY);

    void update(bool turningLeft,
                bool turningRight,
                bool thrusting,
                float deltaSeconds,
                float turnSpeedDegPerSec,
                float thrustAcceleration);

    void render(SDL_Renderer* renderer, bool showThrust) const;

private:
    float m_x;
    float m_y;
    float m_velocityX;
    float m_velocityY;
    float m_orientationDegrees;

    void normalizeOrientation();
    void updatePosition(float deltaSeconds);
};


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
                float deltaSeconds,
                float turnSpeedDegPerSec);

    void render(SDL_Renderer* renderer) const;

private:
    float m_centerX;
    float m_centerY;
    float m_orientationDegrees;

    void normalizeOrientation();
};


//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include "colors.h"

class ShipExplosion {
public:
    struct Particle {
        float x, y;
        float velocityX, velocityY;
        float rotation;
        float rotationSpeed;
        std::vector<SDL_FPoint> shape;
    };

    ShipExplosion(float shipX, float shipY, float shipOrientation);

    void update(float deltaSeconds);
    void render(SDL_Renderer* renderer, const Colors::Color& color) const;

    [[nodiscard]] bool isFinished() const;
    [[nodiscard]] float getElapsedTime() const;

private:
    std::vector<Particle> m_particles;
    float m_elapsedTime;
    static constexpr float EXPLOSION_DURATION = 2.0f;

    void createDebris(float shipX, float shipY, float shipOrientation);
};


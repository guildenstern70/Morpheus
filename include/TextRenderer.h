//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include "colors.h"

class TextRenderer {
public:
    enum class Alignment {
        LEFT,
        CENTER,
        RIGHT
    };

    static void renderText(SDL_Renderer* renderer,
                          const std::string& text,
                          float x,
                          float y,
                          float scale,
                          const Colors::Color& color,
                          Alignment alignment = Alignment::LEFT);

    static float getTextWidth(const std::string& text, float scale);

private:
    static void renderChar(SDL_Renderer* renderer, char c, float x, float y, float scale);
    static float getCharWidth(char c);
};


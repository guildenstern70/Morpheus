//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <array>

namespace Shapes {
    // SHAPE_A: Irregular polygon with 7 vertices
    constexpr std::array<std::pair<float, float>, 8> ASTEROID_SHAPE_A = {{
        {0.0f, -1.0f},              // Top peak
        {0.65f, -0.45f},            // Upper right
        {0.85f, 0.25f},             // Right protrusion
        {0.35f, 0.75f},             // Lower right
        {-0.15f, 0.9f},             // Bottom
        {-0.8f, 0.3f},              // Left protrusion
        {-0.7f, -0.5f},             // Upper left
        {0.0f, -1.0f}               // Close the shape
    }};

    // SHAPE_B: Irregular polygon with 8 vertices
    constexpr std::array<std::pair<float, float>, 9> ASTEROID_SHAPE_B = {{
        {0.3f, -1.0f},              // Top (offset right)
        {0.85f, -0.6f},             // Upper right
        {0.95f, 0.1f},              // Right peak
        {0.6f, 0.8f},               // Lower right
        {0.1f, 0.95f},              // Bottom right
        {-0.5f, 0.7f},              // Bottom left
        {-0.9f, -0.2f},             // Upper left
        {-0.5f, -0.8f},             // Upper left (inner)
        {0.3f, -1.0f}               // Close the shape
    }};
}


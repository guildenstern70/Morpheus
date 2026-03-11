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

    // SHAPE_C: Chunky, wide asteroid with deep indentations
    constexpr std::array<std::pair<float, float>, 10> ASTEROID_SHAPE_C = {{
        {0.0f, -1.0f},              // Top center
        {0.6f, -0.7f},              // Upper right
        {1.0f, -0.1f},              // Far right
        {0.7f, 0.4f},               // Lower right
        {0.2f, 0.9f},               // Bottom-right
        {-0.2f, 0.85f},             // Bottom-left
        {-0.8f, 0.4f},              // Lower left
        {-1.0f, -0.1f},             // Far left
        {-0.5f, -0.75f},            // Upper-left indentation
        {0.0f, -1.0f}               // Close the shape
    }};

    // SHAPE_D: Narrow, spiky asteroid with asymmetric peaks
    constexpr std::array<std::pair<float, float>, 9> ASTEROID_SHAPE_D = {{
        {0.15f, -1.0f},             // Top (slightly right)
        {0.7f, -0.55f},             // Upper right spike
        {0.55f, 0.05f},             // Right inset
        {0.9f, 0.6f},               // Lower right spike
        {0.2f, 0.95f},              // Bottom small
        {-0.35f, 0.7f},             // Bottom-left
        {-0.85f, 0.1f},             // Left protrusion
        {-0.6f, -0.7f},             // Upper-left
        {0.15f, -1.0f}              // Close the shape
    }};

    // SHAPE_E: Rounded, multi-lobed asteroid (softer profile)
    constexpr std::array<std::pair<float, float>, 11> ASTEROID_SHAPE_E = {{
        {0.0f, -1.0f},              // Top
        {0.45f, -0.85f},            // upper-right lobe
        {0.9f, -0.4f},              // right lobe
        {0.95f, 0.15f},             // right-bottom
        {0.6f, 0.6f},               // lower-right
        {0.1f, 0.95f},              // bottom
        {-0.25f, 0.8f},             // bottom-left lobe
        {-0.7f, 0.4f},              // left-lower
        {-0.9f, -0.15f},            // left lobe
        {-0.4f, -0.8f},             // upper-left
        {0.0f, -1.0f}               // Close the shape
    }};
}


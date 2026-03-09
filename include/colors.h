//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <cstdint>

namespace Colors {
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    constexpr Color BLACK = {30, 30, 30, 255};
    constexpr Color GREEN = {0, 255, 0, 255};
    constexpr Color RED = {255, 0, 255, 255};
    constexpr Color SILVER = {222, 222, 222, 255};
}


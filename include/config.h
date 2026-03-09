//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "Morpheus";

constexpr float SHIP_TURN_SPEED_DEG_PER_SEC = 180.0f;
constexpr float SHIP_THRUST_ACCELERATION = 200.0f;
constexpr float SHIP_CENTER_X = SCREEN_WIDTH / 2.0f;
constexpr float SHIP_CENTER_Y = SCREEN_HEIGHT / 2.0f;

constexpr int TARGET_FPS = 60;
constexpr float TARGET_FRAME_TIME_MS = 1000.0f / static_cast<float>(TARGET_FPS);


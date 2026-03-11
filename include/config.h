//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include "colors.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr auto WINDOW_TITLE = "Morpheus";

// Mathematical constants
constexpr float PI = 3.14159265358979323846f;

// Ship configuration
constexpr float SHIP_TURN_SPEED_DEG_PER_SEC = 180.0f;
constexpr float SHIP_THRUST_ACCELERATION = 200.0f;
constexpr float SHIP_CENTER_X = SCREEN_WIDTH / 2.0f;
constexpr float SHIP_CENTER_Y = SCREEN_HEIGHT / 2.0f;
constexpr float SHIP_COLLISION_RADIUS = 10.0f;

// Asteroid configuration
constexpr float ASTEROID_LARGE_RADIUS = 48.0f;
constexpr float ASTEROID_MEDIUM_RADIUS = 32.0f;
constexpr float ASTEROID_SMALL_RADIUS = 16.0f;
constexpr float ASTEROID_ROTATION_SPEED_DEG_PER_SEC = 20.0f;
constexpr float ASTEROID_BASE_SPEED = 40.0f;
constexpr int LEVEL_ONE_ASTEROID_COUNT = 5;
constexpr int LEVEL_ASTEROID_INCREMENT = 2;
constexpr float LEVEL_CLEARED_MESSAGE_DURATION = 1.5f;

// Collision configuration
constexpr int ASTEROID_FRAGMENTS_COUNT = 3;
constexpr float FRAGMENT_IMPULSE_MAGNITUDE = 40.0f;
constexpr float FRAGMENT_ANGLE_SPREAD_DEG = 45.0f;

constexpr float WAITING_REGENERATE_THRESHOLD = 3.0f;
constexpr float GAME_OVER_DISPLAY_DURATION = 3.0f;  // Seconds to show GAME OVER before INSERT COIN screen
constexpr int TARGET_FPS = 60;
constexpr float TARGET_FRAME_TIME_MS = 1000.0f / static_cast<float>(TARGET_FPS);

// Ship invulnerability on respawn
constexpr float SHIP_INVULNERABILITY_DURATION = 2.0f; // seconds
constexpr float SHIP_INVULNERABILITY_BLINK_PERIOD = 0.50f; // seconds

// Bolt
constexpr float BOLT_SPEED = 400.0f;  // Speed of the bolt
constexpr float BOLT_RADIUS = 1.0f;   // Collision radius of the bolt

// Colors
constexpr Colors::Color BACKGROUND_COLOR = Colors::BLACK;
constexpr Colors::Color SHIP_COLOR = Colors::SILVER;

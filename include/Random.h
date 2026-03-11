//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <random>
#include <chrono>
#include <cstdint>

class Random {
 public:
  // Deterministic seed for reproducible runs
  static void seed(uint32_t s) { getGenerator().seed(s); }

  // Non-deterministic seed using random_device with a time fallback
  static void seedFromRandomDevice() {
    std::random_device rd;
    uint32_t s = rd();
    if (s == 0) {
      s = static_cast<uint32_t>(
          std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }
    getGenerator().seed(s);
  }

  // Inclusive integer range [min, max]
  static int uniformInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getGenerator());
  }

  // Floating point range [min, max)
  static float uniformReal(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(getGenerator());
  }

  // Convenience: choose an index in [0, n-1]
  static std::size_t choiceIndex(std::size_t n) {
    if (n == 0) return 0;
    return static_cast<std::size_t>(uniformInt(0, static_cast<int>(n) - 1));
  }

 private:
  // Single generator per thread (project is single-threaded but this is future-proof)
  static std::mt19937& getGenerator() {
    static thread_local std::mt19937 gen(
        static_cast<uint32_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    return gen;
  }
};


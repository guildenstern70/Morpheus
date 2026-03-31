//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

#include <SDL3/SDL_audio.h>
#include <vector>

class BackgroundAudio {
public:
    BackgroundAudio() = default;
    ~BackgroundAudio();

    [[nodiscard]] bool initialize();
    [[nodiscard]] bool start();
    void stop();
    void shutdown();

private:
    enum class Phase {
        BEEP,
        SILENCE_AFTER_BEEP,
        BOP,
        SILENCE_AFTER_BOP
    };

    static void SDLCALL audioCallback(void* userData,
                                      SDL_AudioStream* stream,
                                      int additionalAmount,
                                      int totalAmount);

    void fillStream(SDL_AudioStream* stream, int additionalAmount);
    void advancePhase();
    [[nodiscard]] float currentPhaseDuration() const;
    [[nodiscard]] bool isTonePhase() const;
    [[nodiscard]] const std::vector<float>& currentToneSamples() const;
    [[nodiscard]] bool loadToneSamples(const char* relativePath, std::vector<float>& outSamples);
    void resetSequence();

    SDL_AudioStream* m_audioStream = nullptr;
    bool m_isPlaying = false;
    Phase m_phase = Phase::BEEP;
    float m_phaseElapsedSeconds = 0.0f;
    std::vector<float> m_beepSamples;
    std::vector<float> m_bopSamples;
    int m_toneSampleIndex = 0;
};




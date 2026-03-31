//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "BackgroundAudio.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>

#include <cstring>
#include <string>
#include <vector>

#include "config.h"

namespace {
std::vector<std::string> buildAssetPathCandidates(const char* relativePath) {
    std::vector<std::string> candidates;
    candidates.emplace_back(relativePath);

    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        std::string fullPath(basePath);
        if (!fullPath.empty() && fullPath.back() != '/') {
            fullPath.push_back('/');
        }
        fullPath += relativePath;
        candidates.push_back(fullPath);
    }

    return candidates;
}
}  // namespace

BackgroundAudio::~BackgroundAudio() {
    shutdown();
}

bool BackgroundAudio::initialize() {
    if (m_audioStream) {
        return true;
    }

    SDL_AudioSpec desiredSpec{};
    desiredSpec.format = SDL_AUDIO_F32;
    desiredSpec.channels = 1;
    desiredSpec.freq = BACKGROUND_AUDIO_SAMPLE_RATE;

    m_audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                              &desiredSpec,
                                              &BackgroundAudio::audioCallback,
                                              this);
    if (!m_audioStream) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Background audio disabled: %s",
                    SDL_GetError());
        return false;
    }

    if (!loadToneSamples(BACKGROUND_BEEP_SOUND_PATH, m_beepSamples) ||
        !loadToneSamples(BACKGROUND_BOP_SOUND_PATH, m_bopSamples)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Background audio disabled: unable to load beat sounds");
        shutdown();
        return false;
    }

    resetSequence();
    m_isPlaying = false;

    return true;
}

bool BackgroundAudio::start() {
    if (!m_audioStream && !initialize()) {
        return false;
    }

    if (m_isPlaying) {
        return true;
    }

    resetSequence();
    if (!SDL_ResumeAudioStreamDevice(m_audioStream)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Background audio disabled: %s",
                    SDL_GetError());
        return false;
    }

    m_isPlaying = true;
    return true;
}

void BackgroundAudio::stop() {
    if (!m_audioStream || !m_isPlaying) {
        return;
    }

    if (!SDL_PauseAudioStreamDevice(m_audioStream)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Failed to pause background audio: %s",
                    SDL_GetError());
    }
    m_isPlaying = false;
    resetSequence();
}

void BackgroundAudio::shutdown() {
    if (!m_audioStream) {
        return;
    }

    stop();
    SDL_DestroyAudioStream(m_audioStream);
    m_audioStream = nullptr;
    m_beepSamples.clear();
    m_bopSamples.clear();
}

void SDLCALL BackgroundAudio::audioCallback(void* userData,
                                            SDL_AudioStream* stream,
                                            int additionalAmount,
                                            int totalAmount) {
    (void)totalAmount;
    auto* backgroundAudio = static_cast<BackgroundAudio*>(userData);
    if (!backgroundAudio || additionalAmount <= 0) {
        return;
    }

    backgroundAudio->fillStream(stream, additionalAmount);
}

void BackgroundAudio::fillStream(SDL_AudioStream* stream, int additionalAmount) {
    const int sampleCount = additionalAmount / static_cast<int>(sizeof(float));
    if (sampleCount <= 0) {
        return;
    }

    std::vector<float> samples(static_cast<std::size_t>(sampleCount), 0.0f);
    const float sampleIntervalSeconds = 1.0f / static_cast<float>(BACKGROUND_AUDIO_SAMPLE_RATE);

    for (int i = 0; i < sampleCount; ++i) {
        if (isTonePhase()) {
            const std::vector<float>& toneSamples = currentToneSamples();
            if (m_toneSampleIndex >= 0 && m_toneSampleIndex < static_cast<int>(toneSamples.size())) {
                samples[static_cast<std::size_t>(i)] = toneSamples[static_cast<std::size_t>(m_toneSampleIndex)] * BACKGROUND_AUDIO_GAIN;
                ++m_toneSampleIndex;
            }
        }

        m_phaseElapsedSeconds += sampleIntervalSeconds;
        const float phaseDuration = currentPhaseDuration();
        if (m_phaseElapsedSeconds >= phaseDuration) {
            m_phaseElapsedSeconds -= phaseDuration;
            advancePhase();
        }
    }

    if (!SDL_PutAudioStreamData(stream, samples.data(), additionalAmount)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Failed to queue background audio: %s",
                    SDL_GetError());
    }
}

void BackgroundAudio::advancePhase() {
    switch (m_phase) {
        case Phase::BEEP:
            m_phase = Phase::SILENCE_AFTER_BEEP;
            break;
        case Phase::SILENCE_AFTER_BEEP:
            m_phase = Phase::BOP;
            m_toneSampleIndex = 0;
            break;
        case Phase::BOP:
            m_phase = Phase::SILENCE_AFTER_BOP;
            break;
        case Phase::SILENCE_AFTER_BOP:
            m_phase = Phase::BEEP;
            m_toneSampleIndex = 0;
            break;
    }
}

float BackgroundAudio::currentPhaseDuration() const {
    switch (m_phase) {
        case Phase::BEEP:
        case Phase::BOP:
            return BACKGROUND_AUDIO_TONE_DURATION_SECONDS;
        case Phase::SILENCE_AFTER_BEEP:
        case Phase::SILENCE_AFTER_BOP:
            return BACKGROUND_AUDIO_SILENCE_DURATION_SECONDS;
    }

    return BACKGROUND_AUDIO_TONE_DURATION_SECONDS;
}

bool BackgroundAudio::isTonePhase() const {
    return m_phase == Phase::BEEP || m_phase == Phase::BOP;
}

const std::vector<float>& BackgroundAudio::currentToneSamples() const {
    return (m_phase == Phase::BEEP) ? m_beepSamples : m_bopSamples;
}

bool BackgroundAudio::loadToneSamples(const char* relativePath, std::vector<float>& outSamples) {
    SDL_AudioSpec sourceSpec{};
    Uint8* sourceBuffer = nullptr;
    Uint32 sourceLength = 0;

    bool loaded = false;
    for (const std::string& candidatePath : buildAssetPathCandidates(relativePath)) {
        if (SDL_LoadWAV(candidatePath.c_str(), &sourceSpec, &sourceBuffer, &sourceLength)) {
            loaded = true;
            break;
        }
    }

    if (!loaded) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Unable to load sound asset '%s': %s",
                    relativePath,
                    SDL_GetError());
        return false;
    }

    SDL_AudioSpec targetSpec{};
    targetSpec.format = SDL_AUDIO_F32;
    targetSpec.channels = 1;
    targetSpec.freq = BACKGROUND_AUDIO_SAMPLE_RATE;

    Uint8* convertedBuffer = nullptr;
    int convertedLength = 0;
    const bool converted = SDL_ConvertAudioSamples(&sourceSpec,
                                                   sourceBuffer,
                                                   static_cast<int>(sourceLength),
                                                   &targetSpec,
                                                   &convertedBuffer,
                                                   &convertedLength);
    SDL_free(sourceBuffer);

    if (!converted || !convertedBuffer || convertedLength <= 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO,
                    "Unable to convert sound asset '%s': %s",
                    relativePath,
                    SDL_GetError());
        if (convertedBuffer) {
            SDL_free(convertedBuffer);
        }
        return false;
    }

    const int sampleCount = convertedLength / static_cast<int>(sizeof(float));
    if (sampleCount <= 0) {
        SDL_free(convertedBuffer);
        return false;
    }

    outSamples.resize(static_cast<std::size_t>(sampleCount));
    std::memcpy(outSamples.data(), convertedBuffer, static_cast<std::size_t>(convertedLength));
    SDL_free(convertedBuffer);

    return true;
}

void BackgroundAudio::resetSequence() {
    m_phase = Phase::BEEP;
    m_phaseElapsedSeconds = 0.0f;
    m_toneSampleIndex = 0;
}




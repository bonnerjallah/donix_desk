#pragma once

#include <Arduino.h>
#include <driver/i2s.h>

// =========================
// Microphone Pins
// =========================

const int MIC_WS_PIN = 23;
const int MIC_SD_PIN = 22;
const int MIC_SCK_PIN = 21;

// =========================
// I2S Configuration
// =========================

const i2s_port_t MIC_I2S_PORT = I2S_NUM_0;

const int MIC_SAMPLE_RATE = 16000;

const int MIC_BITS_PER_SAMPLE =
    I2S_BITS_PER_SAMPLE_16BIT;

const int MIC_CHANNEL_FORMAT =
    I2S_CHANNEL_FMT_ONLY_LEFT;

const int MIC_DMA_BUFFER_COUNT = 8;
const int MIC_DMA_BUFFER_LENGTH = 1024;

// =========================
// Recording Configuration
// =========================

extern unsigned long micStartTime;

extern const unsigned long MIC_TIMEOUT;

extern bool micActive;

extern int micBufferCount;

extern const int MIC_WARMUP_BUFFER_COUNT;

extern const int32_t SOUND_THRESHOLD;

extern const int RECORD_SAMPLES;

// =========================
// Audio Buffers
// =========================

extern int32_t micBuffer[1024];

extern int16_t pcmBuffer[1024];

extern int32_t* audioBuffer;

extern size_t audioIndex;

// =========================
// Recording State
// =========================

extern bool recording;

// =========================
// Functions
// =========================

void mic_init();

void mic_read(int32_t *buffer, size_t *samples);

void mic_deinit();

void startRecording();

void stopRecording();
#include "amp.h"

void amp_init() {
    Serial.println("Initializing amplifier...");

    pinMode(AMP_SD_PIN, OUTPUT);
    digitalWrite(AMP_SD_PIN, HIGH);

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(
            I2S_MODE_MASTER |
            I2S_MODE_TX
        ),
        .sample_rate = AMP_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = AMP_DMA_BUFFER_COUNT,
        .dma_buf_len = AMP_DMA_BUFFER_LENGTH,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_driver_install(
        AMP_I2S_PORT,
        &i2s_config,
        0,
        nullptr
    );

    i2s_pin_config_t pin_config = {
        .bck_io_num = AMP_BCLK_PIN,
        .ws_io_num = AMP_LRC_PIN,
        .data_out_num = AMP_DIN_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_set_pin(
        AMP_I2S_PORT,
        &pin_config
    );

    i2s_zero_dma_buffer(AMP_I2S_PORT);

    Serial.println("Amplifier initialized successfully.");
}

int amp_play(int16_t* audioData, size_t sampleCount) {
    Serial.println("Playing audio through amplifier...");

    if (audioData == nullptr || sampleCount == 0) {
        return 0;
    }

    size_t bytesWritten = 0;

    esp_err_t result = i2s_write(
        AMP_I2S_PORT,
        audioData,
        sampleCount * sizeof(int16_t),
        &bytesWritten,
        portMAX_DELAY
    );

    if (result != ESP_OK) {
        return -1;
    }

    Serial.println("Audio playback completed.");
    return bytesWritten / sizeof(int16_t);
}

int amp_play_chunk(uint8_t* data, size_t length) {

    if (data == nullptr || length == 0) {
        return 0;
    }

    // Make sure we have complete 16-bit samples
    size_t sampleCount = length / sizeof(int16_t);

    int16_t* monoSamples =
        reinterpret_cast<int16_t*>(data);

    // Maximum expected chunk:
    // 2048 bytes / 2 = 1024 mono samples
    // 1024 * 2 = 2048 stereo samples
    static int16_t stereoBuffer[2048];

    for (size_t i = 0; i < sampleCount; i++) {

        stereoBuffer[i * 2] =
            monoSamples[i];

        stereoBuffer[i * 2 + 1] =
            monoSamples[i];
    }

    size_t bytesWritten = 0;

    esp_err_t result = i2s_write(
        AMP_I2S_PORT,
        stereoBuffer,
        sampleCount * 2 * sizeof(int16_t),
        &bytesWritten,
        portMAX_DELAY
    );

    if (result != ESP_OK) {

        Serial.printf(
            "I2S write failed: %d\n",
            result
        );

        return -1;
    }

    return bytesWritten;
}

void amp_stop() {
    i2s_zero_dma_buffer(AMP_I2S_PORT);
    digitalWrite(AMP_SD_PIN, LOW);
}

void amp_deinit() {
    digitalWrite(AMP_SD_PIN, LOW);
    i2s_driver_uninstall(AMP_I2S_PORT);
}
#include <Arduino.h>
#include "mic.h"
#include "app_web_server.h"

unsigned long micStartTime = 0;
const unsigned long MIC_TIMEOUT = 60000; // 60 seconds

bool micActive = false;
int micBufferCount = 0;

const int MIC_WARMUP_BUFFER_COUNT = 10;

const int32_t SOUND_THRESHOLD = 30000000;

const int RECORD_SAMPLES = 16000 * 5; // 5 seconds at 16 kHz

int32_t micBuffer[1024];
int16_t pcmBuffer[1024];
int32_t* audioBuffer = nullptr;
size_t audioIndex = 0;

bool recording = false;

void mic_init() {

    Serial.println("Initializing microphone...");

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(
            I2S_MODE_MASTER |
            I2S_MODE_RX
        ),

        .sample_rate = 16000,

        // INMP441 outputs 24-bit audio
        // inside a 32-bit I2S frame.
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,

        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,

        .communication_format = I2S_COMM_FORMAT_I2S,

        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,

        .dma_buf_count = 8,

        .dma_buf_len = 1024,

        .use_apll = false,

        .tx_desc_auto_clear = false,

        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = MIC_SCK_PIN,
        .ws_io_num = MIC_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = MIC_SD_PIN
    };

    esp_err_t err = i2s_driver_install(
        MIC_I2S_PORT,
        &i2s_config,
        0,
        NULL
    );

    if (err != ESP_OK) {
        Serial.printf(
            "Failed to install I2S driver: %d\n",
            err
        );
        return;
    }

    err = i2s_set_pin(
        MIC_I2S_PORT,
        &pin_config
    );

    if (err != ESP_OK) {
        Serial.printf(
            "Failed to set I2S pins: %d\n",
            err
        );

        i2s_driver_uninstall(MIC_I2S_PORT);
        return;
    }

    // Clear any garbage already sitting in DMA.
    i2s_zero_dma_buffer(MIC_I2S_PORT);

    Serial.println(
        "Microphone initialized successfully."
    );
};

void mic_read(int32_t *buffer, size_t *samples) {
    
    if (buffer == nullptr || samples == nullptr) {
        return;
    }

    size_t bytesRead = 0;

    esp_err_t err = i2s_read(
        MIC_I2S_PORT,
        buffer,
        (*samples) * sizeof(int32_t),
        &bytesRead,
        portMAX_DELAY
    );

    if (err != ESP_OK) {
        *samples = 0;
        return;
    }

    *samples = bytesRead / sizeof(int32_t);
}

void mic_deinit() {

    i2s_driver_uninstall(MIC_I2S_PORT);

    Serial.println("Microphone deinitialized.");
}

void startRecording() {
    recording = true;
};

void stopRecording() {
    recording = false;

    sendWebSocketText("RECORDING_COMPLETE");

    Serial.println("Recording complete");
}
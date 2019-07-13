#include "Arduino.h"
#include "SD.h"
#include <MD5Builder.h>
#include <mbedtls/sha1.h>
#include "T4_V13.h"

// Non-default SPI pinout
SPIClass sdSPI(VSPI);

// Parallel task. Should only be used with exactly 2 CPU cores. (2 tasks)
void md5calc(void *params) {
    int cpuNumber = *((int *)params);
    String cpuString = "CPU" + String(cpuNumber) + " ";

    MD5Builder md5;
    File screen = SD.open("/screen.tft", FILE_READ);
    int halfSize = cpuNumber == 0 ? screen.size() / 2 : screen.size() / 2;
    int remaining = cpuNumber == 0 ? halfSize : screen.size() - halfSize;

    Serial.println(cpuString + "Start MD5 hashing...");

    unsigned long startTime = millis();

    screen.seek(cpuNumber == 0 ? 0 : halfSize);

    md5.begin();
    md5.addStream(screen, remaining);
    md5.calculate();
    screen.close();

    Serial.println("\n" + cpuString + md5.toString());
    unsigned long finishTime = millis() - startTime;
    Serial.println(cpuString + String(finishTime) + " ms (" + String(remaining) + " bytes)");
    Serial.println(cpuString + String(remaining / (finishTime * 1.0)) + " bytes/ms");
    vTaskDelete(NULL);
}

void peripheral_init() {
    Serial.begin(115200);
    
    sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);

    if (!SD.begin(SD_CS, sdSPI)) {
        Serial.println("SD init failed");
    }
}

void parallel_md5_demo() {
    File f = SD.open("/screen.tft", FILE_READ);
    Serial.println(String(f.size()) + " bytes total");
    f.close();

    int CPU0 = 0;
    int CPU1 = 1;
    xTaskCreatePinnedToCore(md5calc, "md5calc0", 4096, &CPU0, 4, NULL, CPU0);
    xTaskCreatePinnedToCore(md5calc, "md5calc1", 4096, &CPU1, 4, NULL, CPU1);
}

void single_sha1_demo(void *params) {
    File f = SD.open("/screen.tft", FILE_READ);
    int fileSize = f.size();
    Serial.println(String(fileSize) + " bytes total");    

    unsigned long startTime = millis();

    uint8_t output[20];
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init(&ctx);
    mbedtls_sha1_starts_ret(&ctx);

    // Create a buffer with 1024 bytes size
    int bufferSize = 1024;
    uint8_t sha1_buffer[bufferSize];
    int iteration = fileSize / bufferSize;
    int remaining = fileSize % bufferSize;

    for (int i = 0; i < iteration; i++) {
        f.readBytes((char *)sha1_buffer, bufferSize);
        mbedtls_sha1_update_ret(&ctx, sha1_buffer, bufferSize);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    // Update the remaining bytes
    f.readBytes((char *)sha1_buffer, remaining);
    mbedtls_sha1_update_ret(&ctx, sha1_buffer, remaining);

    mbedtls_sha1_finish_ret(&ctx, output);

    mbedtls_sha1_free(&ctx);

    for (int i = 0; i < 20; i++) {
        if (output[i] < 0x10) Serial.print('0');
        Serial.print(output[i], HEX);
    }
    Serial.println();
    unsigned long finishTime = millis() - startTime;
    Serial.println(String(finishTime) + " ms");
    Serial.println(String(fileSize / finishTime * 1.0) + " bytes/ms");
    f.close();
    vTaskDelete(NULL);
}

void setup() {
    peripheral_init();

    Serial.println("SHA1 starting...");
    xTaskCreate(single_sha1_demo, "sha1_demo", 4096, NULL, 3, NULL);
    Serial.println("SHA1 task created");
}

void loop() {

}
#include <ArduinoJson.h>
#include <Wire.h>
#ifndef I2C_SDA
#define I2C_SDA SDA
#endif
#ifndef I2C_SCL
#define I2C_SCL SCL
#endif

#include "PLT.h"
PLT input;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.printf("\nPLT JSON Test\n");

    Wire.setPins(I2C_SDA, I2C_SCL);
    Wire.begin();

    if (input.begin()) {
        Serial.println("PLT initialized successfully.");
    } else {
        Serial.println("Failed to initialize PLT!");
        exit(0);
    }
}

void loop() {
    JsonDocument doc;

    if (input.getJSON(doc)) {
        serializeJsonPretty(doc, Serial);
        Serial.println();
    } else {
        Serial.println("Failed to get PLT data.");
    }

    delay(2000);
}

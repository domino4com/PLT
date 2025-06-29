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
float var1, var2;
char s[] = "Voltage %.2f V, State of Charge: %.2f %%\n";

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.printf("\nPLT Example Test\n");

    Wire.setPins(I2C_SDA, I2C_SCL);
    Wire.begin();

    if (input.begin()) {
        Serial.println("PLT initialized successfully.");
    } else {
        Serial.println("Failed to initialize PLT");
        exit(0);
    }
}

void loop() {
    if (input.getData(var1, var2)) {
        Serial.printf(s, var1, var2);
    } else {
        Serial.println("Failed to get PLT data.");
    }

    delay(1000);
}

#ifndef PLT_H
#define PLT_H

#include <ArduinoJson.h>

#include "Arduino.h"
#include "Wire.h"

// MAX1704X register addresses
#define MAX1704X_ADDR 0x36

#define MAX1704X_VCELL_ADDR 0x02
#define MAX1704X_SOC_ADDR 0x04
#define MAX1704X_MODE_ADDR 0x06
#define MAX1704X_VERSION_ADDR 0x08
#define MAX1704X_CONFIG_ADDR 0x0C
#define MAX1704X_RCOMP_ADDR 0x0C
#define MAX1704X_ATHRD_ADDR 0x0D
#define MAX1704X_COMMAND_ADDR 0xFE

class PLT {
   public:
    PLT();
    bool begin();
    bool getData(float &voltage, float &SOC);
    bool getJSON(JsonDocument &doc);
    double getVoltage();
    double getSOC();
    int getVersion();
    byte getCompensation();
    uint8_t getAlertThreshold();
    uint8_t setCompensation(uint8_t comp);
    uint8_t setAlertThreshold(uint8_t thrd);
    uint8_t clearAlertInterrupt();
    uint8_t sleep();
    uint8_t wake();
    boolean sleeping();
    uint8_t quickStart();
    uint8_t reset();
    uint8_t getStatus();
};

#endif  // PLT_H

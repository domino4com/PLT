#include "PLT.h"

PLT::PLT() {
}

// It forces the MAX1704X to completely reset
uint8_t PLT::reset() {
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_COMMAND_ADDR);
    Wire.write(0x54);
    Wire.write(0x00);
    return Wire.endTransmission();
}

// It forces the MAX1704X to
// restart fuel-gauge calculations
uint8_t PLT::quickStart() {
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_MODE_ADDR);
    Wire.write(0x40);
    Wire.write(0x00);
    return Wire.endTransmission();
}

// Returns the production version of the IC
int PLT::getVersion() {
    byte MSB = 0;
    byte LSB = 0;
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_VERSION_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(MAX1704X_ADDR, 2);
    MSB = Wire.read();
    LSB = Wire.read();
    return (MSB << 8) + LSB;
}

bool PLT::begin() {
    PLT::reset();
    delay(1000);
    Serial.println("Resetting MAX1704X... Version: " + String(PLT::getVersion(), HEX));
    return true;
}

// Returns a measurement of the voltage of the connected 1S / 2S LiIon battery
// 0-5V range w/ 1.25mV resolution for the MAX17048
// 0-10V range w/ 2.5mV resolution for the MAX17049
double PLT::getVoltage() {
    PLT::reset();
    PLT::quickStart();
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_VCELL_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(MAX1704X_ADDR, 2);
    return ((Wire.read() << 4) + (Wire.read() >> 4)) * 0.00125;
}

// Returns the relative state of charge of the connected LiIon Polymer battery
// as a percentage of the full capacity w/ resolution 1/256%
double PLT::getSOC() {
    PLT::reset();
    PLT::quickStart();
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_SOC_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(MAX1704X_ADDR, 2);
    return Wire.read() + (double)Wire.read() / 256;
}



// Return the value used to optimize IC performance to different operating conditions
byte PLT::getCompensation() {
    byte MSB = 0;
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_RCOMP_ADDR);
    Wire.endTransmission();
    Wire.requestFrom(MAX1704X_ADDR, 2);
    MSB = Wire.read();
    return MSB;
}

// Return the alert threshold as a percentage, below an alert interrupt is generated
uint8_t PLT::getAlertThreshold() {
    return (~getStatus() & 0x1F) + 1;
}

// Return the LSByte of the CONFIG register
uint8_t PLT::getStatus() {
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_ATHRD_ADDR);
    Wire.endTransmission(true);
    Wire.requestFrom(MAX1704X_ADDR, 1);
    return Wire.read();
}

// Sets a value to the MSB of the CONFIG register used
// to optimizethe  IC performance to different operating conditions
uint8_t PLT::setCompensation(uint8_t comp) {
    uint8_t status = getStatus();

    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_CONFIG_ADDR);
    Wire.write(comp);
    Wire.write(status);
    return Wire.endTransmission();
}

// Sets the alert threshold below which an alert interrupt is generated
// The acceptable range is 1-32%. Default threshold is 4%
uint8_t PLT::setAlertThreshold(uint8_t thrd) {
    if (thrd > 32)
        thrd = 32;
    else if (thrd < 1)
        thrd = 1;
    thrd = (~thrd + 1) & 0x1F;

    uint8_t comp, sleepBit;
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_CONFIG_ADDR);
    Wire.endTransmission(false);
    Wire.requestFrom(MAX1704X_ADDR, 2);
    comp = Wire.read();
    sleepBit = Wire.read() & 0x80;

    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_CONFIG_ADDR);
    Wire.write(comp);
    Wire.write(sleepBit | thrd);
    return Wire.endTransmission();
}

// After an alert interrupt has been generated,
// it clears the alert bit on the CONFIG register
uint8_t PLT::clearAlertInterrupt() {
    uint8_t compensation = getCompensation();
    uint8_t status = getStatus();
    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_CONFIG_ADDR);
    Wire.write(compensation);
    Wire.write(0xDF & status);
    return Wire.endTransmission();
}

// It puts the MAX1704X to sleep
// All IC operations are halted
uint8_t PLT::sleep() {
    uint8_t compensation = getCompensation();
    uint8_t threshold = getAlertThreshold();

    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_CONFIG_ADDR);
    Wire.write(compensation);
    Wire.write(0x80 | threshold);
    return Wire.endTransmission();
}

// It wakes the MAX1704X from sleep mode
uint8_t PLT::wake() {
    uint8_t compensation = getCompensation();
    uint8_t threshold = getAlertThreshold();

    Wire.beginTransmission(MAX1704X_ADDR);
    Wire.write(MAX1704X_CONFIG_ADDR);
    Wire.write(compensation);
    Wire.write(0x7F & threshold);
    return Wire.endTransmission();
}

// whether the MAX1704X is in sleep mode
boolean PLT::sleeping() {
    return (getStatus() & 0x80) == 0x80;
}

bool PLT::getData(float &voltage, float &SOC) {
    voltage = PLT::getVoltage();
    SOC = PLT::getSOC();
    return true;  // Return true for successful read (add error handling if needed)
}

bool PLT::getJSON(JsonDocument &doc) {
    JsonArray dataArray = doc["PLT"].to<JsonArray>();

    JsonObject dataSet = dataArray.add<JsonObject>();  // First data set
    dataSet["name"] = "Voltage";
    dataSet["value"] = PLT::getVoltage();
    dataSet["unit"] = "V";

    dataSet = dataArray.add<JsonObject>();  // Subsequent data sets
    dataSet["name"] = "State of Charge";
    dataSet["value"] = PLT::getSOC();
    dataSet["unit"] = "%";

    return true;
}

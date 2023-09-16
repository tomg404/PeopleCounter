#include <Wire.h>
#include <Arduino.h>
#include <memory>

#include "PeopleSensor.h"

/* uncomment to switch entry sensor with exit sensor */
//#define FLIP_ORIENTATION

/* uncomment to enable some debug messages */
#define DEBUG

/* uncomment to see if the sensors are working */
//#define TEST

/* uncomment to disable output code like led's */
//#define DONT_USE_OUTPUT

/* sensor settings */
constexpr uint32_t sensorTimeout{100}; // abort measurement after x ms
constexpr uint32_t measurementTimingBudget{20000}; // 20ms (default is 33ms)
constexpr uint8_t entrySensorAddress{0x30};
constexpr uint8_t exitSensorAddress{0x31};

/* pin definitions */
constexpr uint8_t entrySensorXshut{14}; // D5
constexpr uint8_t exitSensorXshut{12}; // D6
constexpr uint8_t entrySensorGpio0{0}; // currently not defined
constexpr uint8_t exitSensorGpio0{0}; //

constexpr uint8_t enterLedPin{2}; // D4
constexpr uint8_t exitLedPin{13}; // D7
constexpr uint8_t errorLedPin{15}; // D8

std::shared_ptr<Output> out;
std::unique_ptr<PeopleSensor> ps;

void setup() {
    Serial.begin(115200); // initialize serial connection
    Wire.begin(); // initialize two-wire

    Serial.printf("\nStarting people counter...\n");

#ifndef FLIP_ORIENTATION
    ps = std::make_unique<PeopleSensor>(entrySensorXshut, exitSensorXshut, entrySensorGpio0, exitSensorGpio0,
                                        entrySensorAddress, exitSensorAddress, sensorTimeout,measurementTimingBudget);
#else
    ps = std::make_unique<PeopleSensor>(exitSensorXshut, entrySensorXshut, exitSensorGpio0, entrySensorGpio0,
                                        exitSensorAddress, entrySensorAddress, sensorTimeout,measurementTimingBudget);
#endif

#ifndef DONT_USE_OUTPUT
    out = std::make_shared<Output>(enterLedPin, exitLedPin, errorLedPin);
    ps->setOutput(out);
#endif

}

#ifndef TEST
void loop() {
    ps->update();
    #ifndef DONT_USE_OUTPUT
    out->update();
    #endif
}
#else
void loop() {
    Serial.printf("s1: %d, s2: %d\n",
                  ps->getEntrySensor().readRangeContinuousMillimeters(),
                  ps->getExitSensor().readRangeContinuousMillimeters());
}
#endif


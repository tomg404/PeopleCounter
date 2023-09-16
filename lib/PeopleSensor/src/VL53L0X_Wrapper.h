#pragma once

#include <cstdint>

#include "VL53L0X.h"

class VL53L0X_Wrapper {
public:
    VL53L0X_Wrapper(uint8_t xshut, uint8_t gpio0, uint8_t i2cAddress, uint32_t sensorTimeout,
                    uint32_t measurementTimingBudget);

    void turnOn();
    void turnOff();

    void initSensor();

    uint16_t getMeasurement();
    uint16_t getMedian(uint8_t n);

    bool wasTriggered();

    [[nodiscard]] uint16_t getStaticDistance() const;
    void resetStaticDistance(uint16_t d = 0);

    VL53L0X& getSensor();

private:
    VL53L0X sensor;
    uint16_t staticDistance;

    /* pins */
    const uint8_t xshut;
    const uint8_t gpio0;

    /* sensor settings */
    const uint8_t i2cAddress;
    const uint32_t sensorTimeout;
    const uint32_t measurementTimingBudget;
};
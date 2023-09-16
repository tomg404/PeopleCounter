#include "VL53L0X_Wrapper.h"
#include <Arduino.h>

VL53L0X_Wrapper::VL53L0X_Wrapper(uint8_t xshut, uint8_t gpio0, uint8_t i2cAddress, uint32_t sensorTimeout,
                                 uint32_t measurementTimingBudget) :
        sensor{}, staticDistance{0}, xshut{xshut}, gpio0{gpio0},
        i2cAddress{i2cAddress}, sensorTimeout{sensorTimeout}, measurementTimingBudget{measurementTimingBudget} {
    pinMode(xshut, OUTPUT);
    pinMode(gpio0, OUTPUT);

    digitalWrite(xshut, LOW); // start with sensor off
}

/* Turns on the sensor via the xshut pin. init() is called as the sensor has to be initialized
 * every time when waking up */
void VL53L0X_Wrapper::turnOn() {
    Serial.printf("Turning on sensor (0x%x)\n", i2cAddress);
    digitalWrite(xshut, HIGH);
//    init_sensor();
}

/* Turns the sensor off via the xshut pin */
void VL53L0X_Wrapper::turnOff() {
    Serial.printf("Turning off sensor (0x%x)\n", i2cAddress);
//    sensor.stopContinuous();
    digitalWrite(xshut, LOW);
}

/* Returns a single measurement in mm from continuous mode */
uint16_t VL53L0X_Wrapper::getMeasurement() {
    return sensor.readRangeContinuousMillimeters();
}

/* Returns the median of n measurements */
uint16_t VL53L0X_Wrapper::getMedian(uint8_t n) {
    uint16_t sum{0};
    for (uint8_t i = 0; i < n; ++i) {
        sum += getMeasurement();
    }
    return sum / n;
}

/* Takes a measurement and checks if something is in the sensors fov.
 * The datasheet of VL53L0X states that a deviation of +/- 5% is to be expected using the 'high speed' profile (20ms
 * per measurement) on a distance of 1.2m. 5% were added to be on the safe side. So a value which is 10% lower than
 * the static distance is considered to be triggered */
bool VL53L0X_Wrapper::wasTriggered() {
    uint16_t value = getMeasurement();
//    Serial.printf("0x%x: %dmm\n", i2cAddress, value);
    return value < staticDistance - staticDistance * 0.10;
}

[[nodiscard]] uint16_t VL53L0X_Wrapper::getStaticDistance() const {
    return staticDistance;
}

/* Recalculates the staticDistance when called with no argument. Otherwise, sets the staticDistance to d */
void VL53L0X_Wrapper::resetStaticDistance(uint16_t d) {
    if (d == 0)
        staticDistance = getMedian(50);
    else
        staticDistance = d;
}

/* Fully initializes a VL53L0X sensor with timeout, timing budget and i2c address */
void VL53L0X_Wrapper::initSensor() {
    Serial.printf(
            "Starting initialization of sensor (0x%x) with settings:\nxshut=%d, gpio0=%d\n" \
            "sensorTimeout=%d\nmeasurementTimingBudget=%d\n",
            i2cAddress, xshut, gpio0, sensorTimeout, measurementTimingBudget
    );

    /* call the sensors own init() function */
    if (!sensor.init()) {
        Serial.printf("Failed to detect and initialize sensor with i2c address 0x%x\n", i2cAddress);
        while (1);
    }

    /* set new i2c address (note: this must happen after sensor.init();
     * turning the sensor off via xshut pin resets the i2c address of sensor) */
    sensor.setAddress(i2cAddress);

    sensor.setMeasurementTimingBudget(measurementTimingBudget);
    sensor.setTimeout(sensorTimeout);

    /* set measuring mode to continuous back-to-back mode for fastest measurements */
    sensor.startContinuous();

    Serial.printf("Successfully initialized sensor with i2c address 0x%x!\n\n", i2cAddress);
}

VL53L0X &VL53L0X_Wrapper::getSensor() {
    return sensor;
}

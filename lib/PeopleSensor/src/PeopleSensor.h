#pragma once

#include <cstdint>
#include <memory>

#include "FSM.h"
#include "VL53L0X_Wrapper.h"
#include "../../output/src/Output.h"

class PeopleSensor {
public:
    PeopleSensor(uint8_t entrySensorXshut, uint8_t exitSensorXshut,
                 uint8_t entrySensorGpio0, uint8_t exitSensorGpio0,
                 uint8_t entrySensorAddress = 0x29, uint8_t exitSensorAddress = 0x2A,
                 uint32_t sensorTimeout = 100, uint32_t measurementTimingBudget = 20000);

    void update();

    uint8_t personEntered();
    uint8_t personLeft();

    void setOutput(std::shared_ptr<Output> output);

#ifdef TEST
    VL53L0X& getEntrySensor();
    VL53L0X& getExitSensor();
#endif

private:
    VL53L0X_Wrapper entrySensor;
    VL53L0X_Wrapper exitSensor;
    fsm::FiniteStateMachine fsm;
    std::shared_ptr<Output> output;
    uint8_t peopleInRoom;  // current number of people in the room

    static event::PossibleEvent translateBoolsToEvent(bool e1, bool e2);

    void handleError();
};
/* PeopleSensor.cpp
 * This is the main piece of code in this project. The PeopleSensor class contains
 * two VL53L0X sensors. These sensors are set up to work in continuous mode so
 * distance readings can be taken as fast as possible. These distance readings are
 * then fed into a finite state machine which handles the logic for walking through
 * doors. */

#include "PeopleSensor.h"

PeopleSensor::PeopleSensor(uint8_t entrySensorXshut, uint8_t exitSensorXshut,
                           uint8_t entrySensorGpio0, uint8_t exitSensorGpio0,
                           uint8_t entrySensorAddress, uint8_t exitSensorAddress,
                           uint32_t sensorTimeout, uint32_t measurementTimingBudget) :
        entrySensor{entrySensorXshut,
                    entrySensorGpio0,
                    entrySensorAddress,
                    sensorTimeout,
                    measurementTimingBudget
        },
        exitSensor{exitSensorXshut,
                   exitSensorGpio0,
                   exitSensorAddress,
                   sensorTimeout,
                   measurementTimingBudget} {
    if (entrySensorAddress == exitSensorAddress) {
        Serial.println("Error! i2c addresses mustn't be the same\n");
        while (true);
    }
    if (entrySensorAddress == 0x29 || exitSensorAddress == 0x29) {
        Serial.println("Error! i2c addresses mustn't be 0x29\n");
        while (true);
    }

    entrySensor.turnOn();
    entrySensor.initSensor();

    exitSensor.turnOn();
    exitSensor.initSensor();

    entrySensor.resetStaticDistance();
    exitSensor.resetStaticDistance();

    peopleInRoom = 0;

    fsm.start();
}

/* This function should be repeatedly called in a loop */
void PeopleSensor::update() {
    Serial.printf("State: %s\n", fsm.getCurrentStateName().data());
    fsm.processEvent(translateBoolsToEvent(entrySensor.wasTriggered(), exitSensor.wasTriggered()));

    /* TODO: refactor this ugly polling shit. make entering/leaving state automatically call peopleSensor.someoneEntered() / .someoneLeft() */
    if (std::holds_alternative<state::SomeoneEntered>(fsm.currentState))
        personEntered();
    if (std::holds_alternative<state::SomeoneLeft>(fsm.currentState))
        personLeft();
    if (std::holds_alternative<state::Error>(fsm.currentState))
        handleError();
}

/* Handles the case when a person entered through the door */
uint8_t PeopleSensor::personEntered() {
    peopleInRoom += 1;
    if (output)
        output->enterProcedure();
    return peopleInRoom;
}

/* Handles the case when a person left through the door */
uint8_t PeopleSensor::personLeft() {
    peopleInRoom -= 1;
    if (output)
        output->exitProcedure();
    return peopleInRoom;
}

/* Resets the fsm and resets number of people in room */
void PeopleSensor::handleError() {
    output->errorProcedure();
    fsm.start();
    peopleInRoom = 0;
}

/* Convert two booleans into the corresponding event */
event::PossibleEvent PeopleSensor::translateBoolsToEvent(bool e1, bool e2) {
    if (e1 && e2)
        return event::BothTriggered{};
    else if (e1)
        return event::FirstTriggered{};
    else if (e2)
        return event::SecondTriggered{};
    else
        return event::NoneTriggered{};
}

void PeopleSensor::setOutput(std::shared_ptr<Output> o) {
    if (o)
        output = std::move(o);
}

#ifdef TEST
VL53L0X &PeopleSensor::getEntrySensor() {
    return entrySensor.getSensor();
}
VL53L0X &PeopleSensor::getExitSensor() {
    return exitSensor.getSensor();
}
#endif
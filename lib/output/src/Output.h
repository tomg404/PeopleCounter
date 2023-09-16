#pragma once

#include <cstdint>
#include <Arduino.h>

class Output {
public:
    Output(uint8_t enterLedPin, uint8_t exitLedPin, uint8_t errorLedPin);

    void update();

    void enterProcedure();
    void exitProcedure();
    void errorProcedure();

private:
    const uint8_t enterLedPin;
    const uint8_t exitLedPin;
    const uint8_t errorLedPin;

    uint64_t enterTimer;
    uint64_t exitTimer;
    uint64_t errorTimer;

    bool enterLedOn;
    bool exitLedOn;
    bool errorLedOn;

    const uint32_t enterDuration{2000};
    const uint32_t exitDuration{2000};
    const uint32_t errorDuration{1000};
};

/* This class manages three leds. The code is still pretty redundant. Pls fix */

#include "Output.h"

Output::Output(uint8_t enterLedPin, uint8_t exitLedPin, uint8_t errorLedPin)
        : enterLedPin{enterLedPin},
          exitLedPin{exitLedPin},
          errorLedPin{errorLedPin} {
    pinMode(enterLedPin, OUTPUT);
    pinMode(exitLedPin, OUTPUT);
    pinMode(errorLedPin, OUTPUT);

    enterLedOn = false;
    exitLedOn = false;
    errorLedOn = false;
}

/* This function is responsible for turning off the leds at the correct time. It
 * should be called repeatedly in a loop for better accuracy */
void Output::update() {
    uint64_t currentTime = millis();

    if (enterLedOn && currentTime > enterTimer) {
        enterLedOn = false;
        digitalWrite(enterLedPin, LOW);
    }
    if (exitLedOn && currentTime > exitTimer) {
        exitLedOn = false;
        digitalWrite(exitLedPin, LOW);
    }
    if (errorLedOn && currentTime > errorTimer) {
        errorLedOn = false;
        digitalWrite(errorLedPin, LOW);
    }
}

void Output::enterProcedure() {
    enterLedOn = true;
    enterTimer = millis() + enterDuration;
    digitalWrite(enterLedPin, HIGH);
}

void Output::exitProcedure() {
    exitLedOn = true;
    exitTimer = millis() + exitDuration;
    digitalWrite(exitLedPin, HIGH);
}

void Output::errorProcedure() {
    errorLedOn = true;
    errorTimer = millis() + errorDuration;
    digitalWrite(errorLedPin, HIGH);
}

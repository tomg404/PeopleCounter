/* this is work in progress */

#pragma once

#include <cstdint>
#include <chrono>

namespace led {
    using namespace std::chrono_literals;
    using duration_t = std::chrono::milliseconds;

    class LED {
    public:
        LED(uint8_t pin);
        void init();
        void on();
        void off();
        void setBrightness();
        void blink();
        void flash();
        void fadeIn();
        void fadeOut();
    private:
        const uint8_t pin;
        duration_t duration;
        std::chrono::time_point<std::chrono::steady_clock> start, end;
    };

} // led


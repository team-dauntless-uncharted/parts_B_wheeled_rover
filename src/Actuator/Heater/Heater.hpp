#pragma once

class Heater {
public:
    Heater(int pin);
    void heat(int pwm, int duration_ms);
private:
    int _pin;
};
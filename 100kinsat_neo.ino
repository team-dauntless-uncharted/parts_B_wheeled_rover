#include <Arduino.h>
#include "Controller/CansatController.hpp"

CansatController cansat;

void setup() {
    cansat.begin();
    delay(2000);
}

void loop() {
    unsigned long start_ms = millis();
    cansat.update();

    while (millis() - start_ms < 50);
}
#include <Arduino.h>
#include "Controller/CansatController.hpp"

CansatController cansat;

void setup() {
    cansat.begin();
    delay(2000);
}

void loop() {
    cansat.update();
    delay(50);
}
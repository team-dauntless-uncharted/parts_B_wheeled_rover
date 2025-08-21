#include <Arduino.h>
#include "Controller/CansatController.hpp"

CansatController cansat;
UserConfig config;

void setup() {
    // ユーザ設定
    config.goalLat = 35.7487860;
    config.goalLng = 139.8070711;
    config.standbyStateAltThreshold = 20;
    config.standbyStateTimeThreshold = 30000;
    config.launchStateCdsThreshold = 400;
    config.dropStateTimeThreshold = 30000;

    cansat.begin(config);
    delay(2000);
}

void loop() {
    cansat.update();
    delay(50);
}
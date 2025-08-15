#include <Arduino.h>
#include "Controller/CansatController.hpp"

CansatController cansat;

void setup() {
    // ユーザ設定
    cansat.userConfig.goalLat = 35.7487860;
    cansat.userConfig.goalLng = 139.8070711;
    cansat.userConfig.altThreshold = 20;
    cansat.userConfig.cdsThreshold = 400;
    cansat.userConfig.accThreshold = 0;
    cansat.userConfig.distanceThreshold = 5;
    cansat.userConfig.timeThreshold = 30000; // 30秒
    
    cansat.begin();
    delay(2000);
}

void loop() {
    cansat.update();
    delay(50);
}
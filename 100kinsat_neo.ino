#include <Arduino.h>
#include "Controller/CansatController.hpp"

CansatController cansat;
UserConfig config;

void setup() {
    // ユーザ設定
    config.goalLat = 35.7487860;
    config.goalLng = 139.8070711;
    config.standbyStateAltThreshold = 20;
    config.standbyStateTimeoutThreshold = 5 * 60 * 1000;
    config.launchStateCdsThreshold = 400;
    config.launchStateTimeoutThreshold = 20 * 60 * 1000;
    config.dropStateTimeoutThreshold = 5 * 60 * 1000;
    config.escapeStateDistanceThreshold = 0.5;
    config.escapeStateTimeoutThreshold = 60 * 1000;
    config.detectionMaxFailedCount = 50;
    config.recordingTimeoutThreshold = 5 * 60 * 1000;

    cansat.begin(config);
    delay(2000);
}

void loop() {
    cansat.update();
    delay(50);
}
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
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    
    // 5秒ごとにデバッグ出力
    if (currentTime - lastDebugTime > 5000) {
        Serial.print("Debug: Loop running, time: ");
        Serial.println(currentTime);
        lastDebugTime = currentTime;
    }
    
    cansat.update();
    cansat.runState();
    delay(50);
}
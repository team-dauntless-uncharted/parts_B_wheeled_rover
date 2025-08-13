#include <Arduino.h>
#include "Controller/CansatController.hpp"

CansatController cansat;

void setup() {
    Serial.begin(115200);
    while(!Serial) {}
    
    Serial.println("Starting 100kinSAT...");
    Serial.println("Step 1: Serial initialized");
    
    // ユーザ設定
    cansat.userConfig.goalLat = 35.7487860;
    cansat.userConfig.goalLng = 139.8070711;
    cansat.userConfig.altThreshold = 20;
    cansat.userConfig.cdsThreshold = 400;
    cansat.userConfig.accThreshold = 0;
    cansat.userConfig.distanceThreshold = 5;
    cansat.userConfig.timeThreshold = 30000; // 30秒
    
    Serial.println("Step 2: User config set");
    
    // 初期化
    Serial.println("Step 3: Starting initialization...");
    cansat.begin();
    
    Serial.println("Step 4: Initialization completed");
    Serial.println("Hello 100kinSAT!!!");
    
    delay(2000);
    Serial.println("Step 5: Setup completed, entering main loop");
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
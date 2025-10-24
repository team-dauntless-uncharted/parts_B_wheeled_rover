/**
 * @file parts_B_wheeled_rover.ino
 * @brief ARLISS 2025 TDU Team-Dauntless-Uncharted Bパーツメインプログラム
 *
 * @author TDU Team-Dauntless-Uncharted
 * @date 2025
 */

#include <Arduino.h>
#include "Controller/CansatController.hpp"

/**
 * @brief CanSat中央コントローラーのグローバルインスタンス
 */
CansatController cansat;

void setup() {
    cansat.begin();
    delay(2000);  // システム安定化待ち
}

void loop() {
    cansat.update();
    delay(200);  // 200ms周期（5Hz）
}
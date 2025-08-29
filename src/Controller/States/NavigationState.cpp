#include "Controller/States/NavigationState.hpp"
#include "Controller/CansatController.hpp"
#include "Utils/GeoUtils/GeoUtils.hpp"
#include <Arduino.h>

// ナビゲーション制御パラメータ
const double ARRIVAL_THRESHOLD = 5.0;        // 到着判定距離 [m]
const double HEADING_TOLERANCE = 15.0;       // 方位許容誤差 [度]
const int MOTOR_BASE_SPEED = 150;            // 基本モーター速度
const int MOTOR_TURN_SPEED = 100;            // 旋回時モーター速度
const unsigned long NAVIGATION_INTERVAL = 500;  // ナビゲーション更新間隔 [ms]
const unsigned long STATUS_LOG_INTERVAL = 5000; // ステータスログ出力間隔 [ms]

NavigationState::NavigationState(CansatController& ctx) 
    : _lastNavigationTime(0), _lastStatusLogTime(0), _isNavigating(false) {
}

void NavigationState::onEnter() {
    _ctx.writeSystemLog("Entering NavigationState");
    
    // 目標座標を設定
    UserConfig config = _ctx.getUserConfig();
    _goalLatitude = config.navigationGoalLatitude;
    _goalLongitude = config.navigationGoalLongitude;
    
    _ctx.writeSystemLog("Navigation goal set to: Lat=%.6f, Lon=%.6f", 
                       _goalLatitude, _goalLongitude);
    
    // LEDパターンを設定（ナビゲーション状態を示す）
    _ctx.setLed(0b1010); // LED0とLED2を点灯
    
    _lastNavigationTime = 0;
    _lastStatusLogTime = 0;
    _isNavigating = true;
}

void NavigationState::onUpdate() {
    _ctx.getSerialWriter().log("Updating NavigationState");
    
    unsigned long currentTime = millis();
    
    // ナビゲーション処理の実行間隔制御
    if (currentTime - _lastNavigationTime >= NAVIGATION_INTERVAL) {
        performNavigation();
        _lastNavigationTime = currentTime;
    }
    
    // ステータスログの出力
    if (currentTime - _lastStatusLogTime >= STATUS_LOG_INTERVAL) {
        logNavigationStatus();
        _lastStatusLogTime = currentTime;
    }
}

void NavigationState::onExit() {
    _ctx.writeSystemLog("Exiting NavigationState");
    
    // モーターを停止
    _ctx.getMotor().stop();
    
    // LED消灯
    _ctx.setLed(0b0000);
    
    _isNavigating = false;
}

State NavigationState::getState() const {
    return State::NAVIGATION;
}

void NavigationState::performNavigation() {
    // 現在位置を取得
    double currentLat = _ctx.getGnss().getLatitude();
    double currentLon = _ctx.getGnss().getLongitude();
    
    // GNSS測位が有効でない場合は処理をスキップ
    if (!_ctx.getGnss().isPosFix()) {
        _ctx.getSerialWriter().log("GNSS position fix not available, skipping navigation");
        _ctx.getMotor().stop();
        return;
    }
    
    // 目標地点までの距離と方位を計算
    double distance = GeoUtils::haversineDistance(currentLat, currentLon, 
                                                 _goalLatitude, _goalLongitude);
    double bearing = GeoUtils::haversineBearing(currentLat, currentLon, 
                                               _goalLatitude, _goalLongitude);
    
    // 現在の方位を取得（BNO055から）
    double currentHeading = _ctx.getBno055().getEulerAngles().heading;
    
    // 到着判定
    if (distance <= ARRIVAL_THRESHOLD) {
        _ctx.writeSystemLog("Arrived at destination! Distance: %.2fm", distance);
        _ctx.getMotor().stop();
        
        // 到着を示すLEDパターン（全点灯）
        _ctx.setLed(0b1111);
        
        // TODO: 次の状態への遷移処理
        // 例: _ctx.changeState(std::make_unique<NextState>(_ctx));
        return;
    }
    
    // 目標方位と現在方位の差を計算
    double headingError = calculateHeadingError(currentHeading, bearing);
    
    // モーター制御の決定
    controlMotors(headingError, distance);
    
    // ナビゲーション情報をログ出力
    _ctx.getSerialWriter().logf("Nav: Dist=%.2fm, Bearing=%.1f°, Heading=%.1f°, Error=%.1f°", 
                              distance, bearing, currentHeading, headingError);
}

double NavigationState::calculateHeadingError(double currentHeading, double targetBearing) {
    double error = targetBearing - currentHeading;
    
    // -180度から+180度の範囲に正規化
    while (error > 180.0) error -= 360.0;
    while (error < -180.0) error += 360.0;
    
    return error;
}

void NavigationState::controlMotors(double headingError, double distance) {
    // 方位誤差の絶対値
    double absError = fabs(headingError);
    
    if (absError <= HEADING_TOLERANCE) {
        // 方位が正しい場合は直進
        int speed = calculateForwardSpeed(distance);
        _ctx.getMotor().forward(speed);
        _ctx.getSerialWriter().logf("Motor: Forward, Speed=%d", speed);
        
    } else if (headingError > 0) {
        // 右に旋回が必要
        int turnSpeed = calculateTurnSpeed(absError);
        _ctx.getMotor().turnRight(turnSpeed);
        _ctx.getSerialWriter().logf("Motor: Turn Right, Speed=%d", turnSpeed);
        
    } else {
        // 左に旋回が必要
        int turnSpeed = calculateTurnSpeed(absError);
        _ctx.getMotor().turnLeft(turnSpeed);
        _ctx.getSerialWriter().logf("Motor: Turn Left, Speed=%d", turnSpeed);
    }
}

int NavigationState::calculateForwardSpeed(double distance) {
    // 距離に応じて速度を調整
    if (distance > 50.0) {
        return MOTOR_BASE_SPEED;  // 遠距離は最大速度
    } else if (distance > 20.0) {
        return MOTOR_BASE_SPEED * 0.8;  // 中距離は80%
    } else if (distance > 10.0) {
        return MOTOR_BASE_SPEED * 0.6;  // 近距離は60%
    } else {
        return MOTOR_BASE_SPEED * 0.4;  // 至近距離は40%
    }
}

int NavigationState::calculateTurnSpeed(double headingError) {
    // 方位誤差に応じて旋回速度を調整
    double normalizedError = headingError / 180.0;  // 0-1に正規化
    
    int turnSpeed = MOTOR_TURN_SPEED * (0.5 + 0.5 * normalizedError);
    
    // 速度の下限と上限を制限
    if (turnSpeed < MOTOR_TURN_SPEED * 0.5) {
        turnSpeed = MOTOR_TURN_SPEED * 0.5;
    }
    if (turnSpeed > MOTOR_BASE_SPEED) {
        turnSpeed = MOTOR_BASE_SPEED;
    }
    
    return turnSpeed;
}

void NavigationState::logNavigationStatus() {
    // 現在位置を取得
    double currentLat = _ctx.getGnss().getLatitude();
    double currentLon = _ctx.getGnss().getLongitude();
    double currentAlt = _ctx.getGnss().getAltitude();
    
    if (_ctx.getGnss().isPosFix()) {
        // 目標地点までの距離を計算
        double distance = GeoUtils::haversineDistance(currentLat, currentLon, 
                                                     _goalLatitude, _goalLongitude);
        double bearing = GeoUtils::haversineBearing(currentLat, currentLon, 
                                                   _goalLatitude, _goalLongitude);
        
        _ctx.writeSystemLog("Navigation Status:");
        _ctx.writeSystemLog("  Current: Lat=%.6f, Lon=%.6f, Alt=%.2fm", 
                           currentLat, currentLon, currentAlt);
        _ctx.writeSystemLog("  Goal: Lat=%.6f, Lon=%.6f", 
                           _goalLatitude, _goalLongitude);
        _ctx.writeSystemLog("  Distance: %.2fm, Bearing: %.1f°", distance, bearing);
    } else {
        _ctx.writeSystemLog("Navigation Status: GNSS position fix not available");
    }
}
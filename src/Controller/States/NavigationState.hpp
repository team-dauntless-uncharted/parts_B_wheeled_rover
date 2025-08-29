#pragma once

#include "Controller/ICansatState.hpp"

class CansatController;

class NavigationState : public ICansatState {
public:
    explicit NavigationState(CansatController& ctx);
    
    void onEnter() override;
    void onUpdate() override;
    void onExit() override;
    State getState() const override;

private:
    // CansatControllerへの参照
    CansatController& _ctx;
    
    // ナビゲーション制御メソッド
    void performNavigation();
    double calculateHeadingError(double currentHeading, double targetBearing);
    void controlMotors(double headingError, double distance);
    int calculateForwardSpeed(double distance);
    int calculateTurnSpeed(double headingError);
    void logNavigationStatus();
    
    // ナビゲーション状態
    double _goalLatitude;
    double _goalLongitude;
    unsigned long _lastNavigationTime;
    unsigned long _lastStatusLogTime;
    bool _isNavigating;
};
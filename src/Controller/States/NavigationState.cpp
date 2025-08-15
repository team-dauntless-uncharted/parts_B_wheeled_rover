#include "Controller/States/NavigationState.hpp"
#include "Controller/States/GoalState.hpp"
#include "Controller/CansatController.hpp"

void NavigationState::onEnter() {
	_ctx.getSerialWriter().log("Entering NavigationState");
}

void NavigationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating NavigationState");

    // 直進10秒
    _ctx.setmOutputTime(10000);
    _ctx.getMotor().forward(200);
    _ctx.setMrPwm(200);
    _ctx.setMlPwm(200);
    delay(_ctx.getmOutputTime());

    // ウエイト2秒
    _ctx.getMotor().stop();
    delay(2000);

    // 右旋回
    _ctx.setmOutputTime(3000);
    _ctx.getMotor().turnRight(150);
    _ctx.setMrPwm(150);
    _ctx.setMlPwm(150);
    delay(_ctx.getmOutputTime());
    _ctx.getMotor().stop();

    // ウエイト2秒
    _ctx.getMotor().stop();
    delay(2000);

    // 直進10秒
    _ctx.setmOutputTime(10000);
    _ctx.getMotor().forward(200);
    _ctx.setMrPwm(200);
    _ctx.setMlPwm(200);
    delay(_ctx.getmOutputTime());
    _ctx.getMotor().stop();

    // ウエイト2秒
    delay(2000);

    // 左旋回
    _ctx.setmOutputTime(3000);
    _ctx.getMotor().turnLeft(150);
    _ctx.setMrPwm(150);
    _ctx.setMlPwm(150);
    delay(_ctx.getmOutputTime());
    _ctx.getMotor().stop();

    delay(2000);
    // 画像を撮影する
    void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }
    delay(2000);

    // 目標地点に到達したら GOAL モードに遷移する
	// 移動前のゴールとの距離を取得
    // double beforeDistance = _ctx.getDistanceToGoal();
    // ゴールとの距離がしきい値以下の場合、ゴール状態へ遷移
    // if (beforeDistance < _ctx.userConfig.distanceThreshold) {
	// 	_ctx.changeState(std::make_unique<GoalState>(_ctx));
    //     return;
    // }

    // 移動前のゴールとの方位を取得
    // double beforeDirection = _ctx.getDirectionToGoal();
    // CanSatの向きと比較
    // double courseDiff = beforeDirection - _ctx.getHeading();
    // ゴールへ方向転換
	// _ctx.setmOutputTime((int)(14 * abs(courseDiff))); // モータへの出力時間を求める
    // if (courseDiff > 0) {
	// 	_ctx.getMotor().turnRight(150);
	// 	_ctx.setMrPwm(150);
	// 	_ctx.setMlPwm(150);
    //     delay(_ctx.getmOutputTime());
    //     _ctx.getMotor().stop();
	// 	_ctx.setMrPwm(0);
	// 	_ctx.setMlPwm(0);
    // } else {
	// 	_ctx.getMotor().turnLeft(150);
	// 	_ctx.setMrPwm(150);
	// 	_ctx.setMlPwm(150);
    //     delay(_ctx.getmOutputTime());
    //     _ctx.getMotor().stop();
	// 	_ctx.setMrPwm(0);
	// 	_ctx.setMlPwm(0);
    // }

    // 直進する
	// _ctx.setmOutputTime(5000);
	// _ctx.getMotor().forward(200);
	// _ctx.setMrPwm(200);
	// _ctx.setMlPwm(200);
    // delay(_ctx.getmOutputTime());
    // _ctx.getMotor().stop();
	// _ctx.setMrPwm(0);
	// _ctx.setMlPwm(0);
	
	// _ctx.setmOutputTime(0);
}

void NavigationState::onExit() {
	_ctx.getSerialWriter().log("Exiting NavigationState");
}

State NavigationState::getState() const {
	return State::NAVIGATION;
}
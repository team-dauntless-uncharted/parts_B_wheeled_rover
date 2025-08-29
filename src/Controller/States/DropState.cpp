#include "Controller/States/DropState.hpp"
#include "Controller/States/EscapeState.hpp"
#include "Controller/CansatController.hpp"
#include "BNO055Library.h"

#define LAND_IMPACT_THRESHOLD 24.5f   // m/s^2 (≈2.5g)
#define LAND_GYRO_THRESHOLD   5.0f    // deg/s
#define LAND_STABLE_TIME      3000    // ms (静止確認時間: 長め)
#define LAND_CHECK_INTERVAL   50      // msごとに判定更新

void DropState::onEnter() {
  _ctx.writeSystemLog("%lu: Entering DropState", millis());
  _ctx.setLed((int)State::DROP);

  if (!_ctx.isConnectTwelite()) {
    _ctx.getTwelite().on();
    _ctx.setIsConnectTwelite(true);
  }

  _impactDetected = false;
  _landed = false;
  _impactTime = 0;
  _stableStart = 0;

  _startTime = millis();
}

void DropState::onUpdate() {
  unsigned long now = millis();
  unsigned long elapsedTime = now - _startTime;

  // --- 1. タイムアウト ---
  if (elapsedTime > _ctx.getUserConfig().dropStateTimeoutThreshold) {
    _ctx.writeSystemLog("%lu: Timeout. Change to EscapeState", millis());
    _ctx.changeState(std::make_unique<EscapeState>(_ctx));
    return;
  }

  // --- 2. Twelite受信 ---
  twelite::Packet pkt;
  if (_ctx.getTwelite().receivePacket(pkt)) {
    if (twelite::TwelitePacket::match(pkt, twelite::C_PARTS, twelite::BROADCAST, twelite::DeployComplete)) {
      _ctx.writeSystemLog("%lu: DeployComplete received. Change to EscapeState", millis());
      _ctx.changeState(std::make_unique<EscapeState>(_ctx));
      return;
    }
  }

  // --- 3. IMU着地判定 ---
  Vector<float> accel = _ctx.getBno055().getAcceleration();
  Vector<float> gyro  = _ctx.getBno055().getGyroscope();
  float accelNorm = sqrt(accel.x*accel.x + accel.y*accel.y + accel.z*accel.z);
  float gyroNorm  = sqrt(gyro.x*gyro.x + gyro.y*gyro.y + gyro.z*gyro.z);

  // (a) 衝撃検知
  if (!_impactDetected && accelNorm > LAND_IMPACT_THRESHOLD) {
    _impactDetected = true;
    _impactTime = now;
    _ctx.getSpeaker().playBeep();
    _ctx.writeSystemLog("%lu: Impact detected (accelNorm=%.2f)", now, accelNorm);
  }

  // (b) 衝撃後の安定確認
  if (_impactDetected && !_landed) {
    if (gyroNorm < LAND_GYRO_THRESHOLD) {
      if (_stableStart == 0) {
        _stableStart = now;  // 安定開始時刻を記録
      } else if ((now - _stableStart) > LAND_STABLE_TIME) {
        // 連続して静止していたら着地判定
        _landed = true;
        _ctx.getSpeaker().playError();
        _ctx.writeSystemLog("%lu: Landing confirmed (stable for %d ms). Change to EscapeState",
                            now, LAND_STABLE_TIME);
        _ctx.changeState(std::make_unique<EscapeState>(_ctx));
        return;
      }
    } else {
      // 動きが検出されたらリセット
      _stableStart = 0;
    }
  }
}

void DropState::onExit() {
  _ctx.writeSystemLog("%lu: Exiting DropState", millis());

  if (!_ctx.getSDLogger().writeState((int)State::ESCAPE)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
  if (!_ctx.getFlashIO().writeState((int)State::ESCAPE)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
  }
#endif // USE_FLASH

	if (!_ctx.isInitCamera()) {
		if (!_ctx.getCamera().begin(PHOTO_MODE)) {
			return;
		}

		if (!_ctx.getCamera().startStreaming(true)) {
			return;
		}
		_ctx.setInitCamera(true);
	}

	void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }

	_ctx.getCamera().end();
	_ctx.setInitCamera(false);
}

State DropState::getState() const {
  return State::DROP;
}
#include <Arduino.h>
#include "Controller/States/HelpingState.hpp"
#include "Controller/CansatController.hpp"

void HelpingState::onEnter() {
	_ctx.writeSystemLog("Entering HelpingState");

	// _ctx.getSpeaker().playState((int)State::HELPING);
	_ctx.setLed((int)State::HELPING);

	if (!_ctx.isConnectTwelite()) {
    	_ctx.getTwelite().on();
    	_ctx.setIsConnectTwelite(true);
  	}
	sendTurnSignal();

	_ctx.writeSystemLog("TurnSignal sent");
}

void HelpingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating HelpingState");

	// 救助されたらどうする?
}

void HelpingState::onExit() {
	_ctx.writeSystemLog("Exiting HelpingState");

	// 救助されたらどうする?
}

State HelpingState::getState() const {
	return State::HELPING;
}

void HelpingState::sendTurnSignal() {
	double latitude = _ctx.getGnss().getLatitude();
	double longitude = _ctx.getGnss().getLongitude();

	uint8_t payloadLength = sizeof(double) * 2;
	uint8_t payload[payloadLength];

	memcpy(payload, &latitude, sizeof(double));
	memcpy(payload + sizeof(double), &longitude, sizeof(double));

	twelite::Packet pkt = twelite::TwelitePacket::makePacket(
		twelite::B_PARTS,
		twelite::A_PARTS,
		twelite::TurnSignal,
		payloadLength,
		payload
	);
	_ctx.getTwelite().sendPacket(pkt);
}
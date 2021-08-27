#pragma once
#include "AnimationEvent.h"

class AnimationStopEvent : public AnimationEvent {
public:
	AnimationStopEvent(CSpriteAnimation* sprite, CAnimation* animation) : AnimationEvent(sprite, animation) {
		name = BASE_NAME();
	}
	static const std::string BASE_NAME() {
		return "AnimationStopEvent";
	}
};
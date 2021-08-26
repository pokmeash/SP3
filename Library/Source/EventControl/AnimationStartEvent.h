#pragma once
#include "AnimationEvent.h"

class AnimationStartEvent : public AnimationEvent {
public:
	AnimationStartEvent(CSpriteAnimation* sprite, CAnimation* animation) : AnimationEvent(sprite, animation) {
		name = BASE_NAME();
	}
	static const std::string BASE_NAME() {
		return "AnimationStartEvent";
	}
};
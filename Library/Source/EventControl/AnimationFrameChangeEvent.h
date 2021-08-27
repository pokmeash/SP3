#pragma once
#include "AnimationEvent.h"

class AnimationFrameChangeEvent : public AnimationEvent {
protected:
	int previous, next;
public:
	AnimationFrameChangeEvent(CSpriteAnimation* sprite, CAnimation* animation, int previousFrame, int nextFrame) : AnimationEvent(sprite, animation), previous(previousFrame), next(nextFrame) {
		name = BASE_NAME();
	}
	static const std::string BASE_NAME() {
		return "AnimationFrameChangeEvent";
	}
	int getPreviousFrame() {
		return previous;
	}
	int getNextFrame() {
		return next;
	}
};
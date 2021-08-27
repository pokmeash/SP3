#pragma once
#include "AbstractEvent.h"
class CSpriteAnimation;
class CAnimation;

class AnimationEvent : public Event {
protected:
	CSpriteAnimation* sprite;
	CAnimation* animation;
public:
	AnimationEvent(CSpriteAnimation* sprite, CAnimation* animation) : Event(BASE_NAME()), sprite(sprite), animation(animation) {}

	static const std::string BASE_NAME() {
		return "AnimationEvent";
	}
	CSpriteAnimation* getSprite() {
		return sprite;
	}
	CAnimation* getAnimation() {
		return animation;
	}
};
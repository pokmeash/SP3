#pragma once
#include "AbstractEvent.h"

class NextRoomEvent : public Event {
public:
	NextRoomEvent() : Event(BASE_NAME()) {}
	static const std::string BASE_NAME() {
		return "NextRoomEvent";
	}
};
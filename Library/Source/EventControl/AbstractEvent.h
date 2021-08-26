#pragma once
#include <string>

class Event {
protected:
	std::string name;
public:
	Event(const std::string& name) : name(name) {}

	virtual ~Event() {}

	std::string getName() {
		return name;
	}
};

class Cancellable {
protected:
	bool cancelled;
public:
	Cancellable() : cancelled(false) {}
	bool isCancelled() {
		return cancelled;
	}

	void setCancelled(bool cancel) {
		cancelled = cancel;
	}
};

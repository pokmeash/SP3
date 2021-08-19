#pragma once
#include <string>
/**
 CGameStateBase
 @brief This is an Abstract Class. Derive from this class to implement it.
 By: Toh Da Jun
 Date: July 2021
 */

struct ButtonData
{
	std::string buttonName = "Button";
	std::string fileName = "";
	std::string keybindType = "";
	unsigned textureID = 0;
	int keybindID = -1;
};

class CGameStateBase
{
protected:
	bool initalized;
public:
	// Constructor
	CGameStateBase(void) {
		initalized = false;
	}
	// Destructor
	virtual ~CGameStateBase(void) {}

	// These are pure virtual functions
	virtual bool Init() = 0;
	virtual bool Update(const double dElapsedTime) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;
};

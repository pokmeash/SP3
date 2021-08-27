#pragma once

/**
 CCharacterSelectionState
 @brief This class is derived from CGameStateBase. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

#include "../SoundController/SoundController.h"

#include "System\filesystem.h"

#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif
#include <string>

class CCharacterSelectionState : public CGameStateBase
{
public:
	// Constructor
	CCharacterSelectionState(void);
	// Destructor
	~CCharacterSelectionState(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

protected:
	CBackgroundEntity* background;
	ImTextureID Character1, Character2;
};

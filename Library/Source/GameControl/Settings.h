/**
 CSettings
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "../DesignPatterns/SingletonTemplate.h"

#include <Windows.h>

// Include GLEW
#ifndef GLEW_STATIC
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif
// Include GLFW
#include <GLFW/glfw3.h>

class CSettings : public CSingletonTemplate<CSettings>
{
	friend CSingletonTemplate<CSettings>;
public:
	enum AXIS
	{
		x = 0,
		y = 1,
		z = 2,
		NUM_AXIS
	};

	enum KEYBINDS
	{
		MOVE_UP = 0,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		TRIGGER_SHOOT,
		TRIGGER_POWERUP,
		TOTAL_KEYBINDS
	};

	// File information
	char* logl_root;	// Root directory for loading digital assets

	// GLFW Information
	GLFWwindow* pWindow;
	bool bUse4XAntiliasing = true;

	// Windows Information
	// Should make these not hard-coded :P
	unsigned int iWindowWidth = 800;
	unsigned int iWindowHeight = 600;
	unsigned int iWindowPosX = 50;
	unsigned int iWindowPosY = 50;
	bool bDisableMousePointer = false;
	bool bShowMousePointer = true;
	unsigned int iKeybinds[TOTAL_KEYBINDS];

	// Frame Rate Information
	const unsigned char FPS = 30; // FPS of this game
	const unsigned int frameTime = 1000 / FPS; // time for each frame

	// Input control
	//const bool bActivateMouseInput

	// 2D Settings
	// The variables which stores the specifications of the map
	unsigned int NUM_TILES_XAXIS;
	unsigned int NUM_TILES_YAXIS;

	float TILE_WIDTH;
	float TILE_HEIGHT;

	float NUM_STEPS_PER_TILE_XAXIS;
	float NUM_STEPS_PER_TILE_YAXIS;

	float MICRO_STEP_XAXIS;
	float MICRO_STEP_YAXIS;

	// Update the specifications of the map
	void UpdateSpecifications(void);

	// Convert an index number of a tile to a coordinate in UV Space
	float ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset = 0.0f);
	float ConvertFloatToUVSpace(const AXIS sAxis, const float iIndex, const bool bInvert);
	float ConvertIndexToWSSpace(const AXIS sAxis, const int iIndex, const float fOffset = 0.f);
	void ConvertFloatToIndexSpace(const AXIS sAxis, const float fCoord, int* iIndex,  int* iMicro);
	void ConvertMouseToWSSpace(int mouseX, int mouseY, float* posX, float* posY);
	void SaveKeybinds();
protected:
	// Constructor
	CSettings(void);

	// Destructor
	virtual ~CSettings(void);
};


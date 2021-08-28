#include "Settings.h"

#include <iostream>
#include "../Library/Source/System/LoadIni.h"
using namespace std;

CSettings::CSettings(void)
	: pWindow(NULL)
	, logl_root(NULL)
	, NUM_TILES_XAXIS(32)
	, NUM_TILES_YAXIS(24)
	, TILE_WIDTH(0.0625f)
	, TILE_HEIGHT(0.08333f)
	, NUM_STEPS_PER_TILE_XAXIS(16.0f)
	, NUM_STEPS_PER_TILE_YAXIS(16.0f)
	, MICRO_STEP_XAXIS(0.00390625f)
	, MICRO_STEP_YAXIS(0.005208125f)
{
	iKeybinds[MOVE_UP] = GLFW_KEY_W;
	iKeybinds[MOVE_DOWN] = GLFW_KEY_S;
	iKeybinds[MOVE_LEFT] = GLFW_KEY_A;
	iKeybinds[MOVE_RIGHT] = GLFW_KEY_D;
	iKeybinds[TRIGGER_SHOOT] = 0;
	iKeybinds[TRIGGER_THROW] = GLFW_KEY_G;
	iKeybinds[TRIGGER_POWERUP] = GLFW_KEY_E;
	iKeybinds[TRIGGER_PORTAL] = 1;
	iKeybinds[TRIGGER_BEAM] = GLFW_KEY_B;
	std::map<unsigned, unsigned> keybinds;
	if (!LoadIni("settings.ini", keybinds)) {
		for (unsigned i = 0; i < TOTAL_KEYBINDS; ++i) {
			std::pair<unsigned, unsigned> pair(i, iKeybinds[i]);
			keybinds.insert(pair);
		}
		SaveIni("settings.ini", keybinds);
	} else {
		for (auto& pair : keybinds) {
			iKeybinds[pair.first] = pair.second;
		}
	}
}


CSettings::~CSettings(void)
{
}

/**
@brief Convert an index number of a tile to a coordinate in UV Space
*/
float CSettings::ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset)
{
	float fResult = 0.0f;
	if (sAxis == x)
	{
		fResult = -1.0f + (float)iIndex*TILE_WIDTH + TILE_WIDTH / 2.0f + fOffset;
	}
	else if (sAxis == y)
	{
		if (bInvert)
			fResult = 1.0f - (float)(iIndex + 1)*TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
		else
			fResult = -1.0f + (float)iIndex*TILE_HEIGHT + TILE_HEIGHT / 2.0f + fOffset;
	}
	return fResult;
}

float CSettings::ConvertFloatToUVSpace(const AXIS sAxis, const float iIndex, const bool bInvert)
{
	float fResult = 0.f;
	if (sAxis == x)
	{
		fResult = -1.f + iIndex * TILE_WIDTH;
	}
	else if (sAxis == y)
	{
		if (bInvert)
			fResult = 1.f - (iIndex + 1.f) * TILE_HEIGHT + TILE_HEIGHT / 2.f;
		else
			fResult = -1.f + iIndex * TILE_HEIGHT + TILE_HEIGHT / 2.f;
	}
	return fResult;
}

float CSettings::ConvertIndexToWSSpace(const AXIS sAxis, const int iIndex, const float fOffset)
{
	float fResult = 0.f;
	if (sAxis == x)
	{
		fResult = iIndex + 0.5f + (fOffset < 1.f ? 0.f : (fOffset / NUM_STEPS_PER_TILE_XAXIS));
	} 
	else if (sAxis == y)
	{
		fResult = iIndex + (fOffset < 1.f ? 0.f : (fOffset / NUM_STEPS_PER_TILE_YAXIS));
	}
	return fResult;
}

void CSettings::ConvertFloatToIndexSpace(const AXIS sAxis, const float fCoord, int* iIndex, int* iMicro)
{
	float fOffset = 0.f;
	if (sAxis == x) {
		fOffset = fCoord - 0.5f - (int)(fCoord - 0.5f);
		*iIndex = (int)(fCoord - 0.5f);
		*iMicro = (int)(NUM_STEPS_PER_TILE_XAXIS * fOffset);
		if (*iMicro >= NUM_STEPS_PER_TILE_XAXIS) {
			*iMicro = (int)(NUM_STEPS_PER_TILE_YAXIS * fOffset) - NUM_STEPS_PER_TILE_XAXIS;
			(*iIndex)++;
		}
	} else if (sAxis == y) {
		fOffset = fCoord - (int)fCoord;
		*iIndex = (int)(fCoord);
		*iMicro = (int)(NUM_STEPS_PER_TILE_YAXIS * fOffset);
		if (*iMicro >= NUM_STEPS_PER_TILE_YAXIS) {
			*iMicro = (int)(NUM_STEPS_PER_TILE_YAXIS * fOffset) - NUM_STEPS_PER_TILE_YAXIS;
			(*iIndex)++;
		}
	}
}

void CSettings::ConvertMouseToWSSpace(int mouseX, int mouseY, float* posX, float* posY)
{
	int w = iWindowWidth;
	int h = iWindowHeight;
	*posX = (float)mouseX / w * NUM_TILES_XAXIS;
	*posY = (float)(h - mouseY) / h * NUM_TILES_YAXIS - 0.5;
}

void CSettings::ConvertMouseToIndexSpace(int mouseX, int mouseY, int* posX, int* posY)
{
	int w = iWindowWidth;
	int h = iWindowHeight;
	float wsX = (float)mouseX / w * NUM_TILES_XAXIS;
	float wsY = (float)(h - mouseY) / h * NUM_TILES_YAXIS - 0.5;
	int microX, microY;
	ConvertFloatToIndexSpace(x, wsX, posX, &microX);
	ConvertFloatToIndexSpace(y, wsY, posY, &microY);
	if (microX > NUM_STEPS_PER_TILE_XAXIS * 0.5f) {
		(*posX)++;
	}
	if (microY > NUM_STEPS_PER_TILE_YAXIS * 0.5f) {
		(*posY)++;
	}
}

void CSettings::ConvertWSToMouseSpace(float x, float y, int* posX, int* posY)
{
	int w = iWindowWidth;
	int h = iWindowHeight;
	*posX = x / NUM_TILES_XAXIS * w;
	*posY = abs(y / NUM_TILES_YAXIS * h - h);
}

void CSettings::SaveKeybinds()
{
	std::map<unsigned, unsigned> keybinds;
	for (unsigned i = 0; i < TOTAL_KEYBINDS; ++i) {
		std::pair<unsigned, unsigned> pair(i, iKeybinds[i]);
		keybinds.insert(pair);
	}
	SaveIni("settings.ini", keybinds);
}

float CSettings::Random(float min, float max)
{
	float range = max - min;
	float random = ((float)rand() / (float)RAND_MAX * range) + min;
	while (random >= -0.5 && random <= 0.5)
	{
		random = ((float)rand() / (float)RAND_MAX * range) + min;
	}
	return random;
}

// Update the specifications of the map
void CSettings::UpdateSpecifications(void)
{
	TILE_WIDTH = 2.0f / NUM_TILES_XAXIS;	// 0.0625f;
	TILE_HEIGHT = 2.0f / NUM_TILES_YAXIS;	// 0.08333f;

	MICRO_STEP_XAXIS = TILE_WIDTH / NUM_STEPS_PER_TILE_XAXIS;
	MICRO_STEP_YAXIS = TILE_HEIGHT / NUM_STEPS_PER_TILE_YAXIS;
}

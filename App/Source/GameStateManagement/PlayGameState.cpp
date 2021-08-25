// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

#include "PlayGameState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
#include "System\filesystem.h"
using namespace std;

/**
 @brief Constructor
 */
CPlayGameState::CPlayGameState(void)
	: cScene2D(NULL)
{

}

/**
 @brief Destructor
 */
CPlayGameState::~CPlayGameState(void)
{
}

/**
 @brief Init this class instance
 */
bool CPlayGameState::Init(void)
{
	// Initialise the cScene2D instance
	cScene2D = CScene2D::GetInstance();
	if (cScene2D->Init() == false)
	{
		cout << "Failed to load Scene2D" << endl;
		return false;
	}
	cSoundController = CSoundController::GetInstance();

	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Lance.ogg"), 3, true, true);
	for (int i = 0; i < 8; i++)
	{
		cSoundController->VolumeDecrease(3);
	}
	bgm = true;

	return true;
}

/**
 @brief Update this class instance
 */
bool CPlayGameState::Update(const double dElapsedTime)
{
	/*if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		 Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		 Load the menu state
		cSoundController->StopAllSound();
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		cSoundController->PlaySoundByID(2);
		return true;
	}*/

	// Call the cScene2D's Update method
	cScene2D->Update(dElapsedTime);

	//BGM
	if (bgm)
	{
		cSoundController->PlaySoundByID(3);
		bgm = false;
	}
	// Check if the game has been won by the player
	if (CPlayer2D::GetInstance()->getHP() <= 0)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("GameOverState");
		cSoundController->StopAllSound();
		cSoundController->PlaySoundByID(1);
	}

	if (CGameManager::GetInstance()->bPlayerWon == true)
	{
		// End the game and switch to Win screen
		// Load the menu state
		CGameStateManager::GetInstance()->SetActiveGameState("WinState");
		cSoundController->StopAllSound();
		cSoundController->PlaySoundByID(1);
	}
	return true;
}

/**
 @brief Render this class instance
 */
void CPlayGameState::Render(void)
{

	// Call the cScene2D's Pre-Render method
	cScene2D->PreRender();

	// Call the cScene2D's Render method
	cScene2D->Render();

	// Call the cScene2D's PostRender method
	cScene2D->PostRender();
}

/**
 @brief Destroy this class instance
 */
void CPlayGameState::Destroy(void)
{

	// Destroy the cScene2D instance
	if (cScene2D)
	{
		cScene2D->Destroy();
		cScene2D = NULL;
	}
}
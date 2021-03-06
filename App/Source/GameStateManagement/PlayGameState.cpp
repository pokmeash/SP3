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
	bgm = true;

	return true;
}

/**
 @brief Update this class instance
 */
bool CPlayGameState::Update(const double dElapsedTime)
{
	// Call the cScene2D's Update method
	cScene2D->Update(dElapsedTime);

	//BGM
	if (bgm)
	{
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::LANCE);
		bgm = false;
	}
	// Check if the game has been won by the player
	if (CPlayer2D::GetInstance()->getHP() <= 0)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("GameOverState");
		cSoundController->StopAllSound();
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::ORAS);
	}

	if (CGameManager::GetInstance()->bPlayerWon == true)
	{
		// End the game and switch to Win screen
		// Load the menu state
		CGameStateManager::GetInstance()->SetActiveGameState("WinState");
		cSoundController->StopAllSound();
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::ORAS);
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
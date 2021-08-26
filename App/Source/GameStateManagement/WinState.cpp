// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "WinState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
#include "System\filesystem.h"
using namespace std;

/**
 @brief Constructor
 */
CWinState::CWinState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CWinState::~CWinState(void)
{
	
}

/**
 @brief Init this class instance
 */
bool CWinState::Init(void)
{

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	cSoundController = CSoundController::GetInstance();

	//Create Background Entity
	background = new CBackgroundEntity("Image/Winner.jpg");
	background->SetShader("2DShader");
	background->Init();


	bgm = true;

	return true;
}

/**
 @brief Update this class instance
 */
bool CWinState::Update(const double dElapsedTime)
{
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		CGameStateManager::GetInstance()->SetActiveGameState("IntroState");
		cSoundController->StopAllSound();
		return true;
	}

	//BGM
	if (bgm)
	{
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::ORAS);
		bgm = false;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CWinState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
 	background->Render();
}

/**
 @brief Destroy this class instance
 */
void CWinState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}
}
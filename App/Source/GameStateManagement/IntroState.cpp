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

#include "IntroState.h"

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
CIntroState::CIntroState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CIntroState::~CIntroState(void)
{
	
}

/**
 @brief Init this class instance
 */
bool CIntroState::Init(void)
{
	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	cSoundController = CSoundController::GetInstance();

	//Create Background Entity
	background = new CBackgroundEntity("Image/IntroBackground.png");
	background->SetShader("2DShader");
	background->Init();


	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Oras.ogg"), 1, true, true);
	for (int i = 0; i < 6; i++)
	{
		cSoundController->VolumeDecrease(1);
	}
	bgm = true;

	return true;
}

/**
 @brief Update this class instance
 */
bool CIntroState::Update(const double dElapsedTime)
{
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("Music");
		cSoundController->StopAllSound();
		return true;
	}

	//BGM
	if (bgm)
	{
		cSoundController->PlaySoundByID(1);
		bgm = false;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CIntroState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
 	background->Render();
}

/**
 @brief Destroy this class instance
 */
void CIntroState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}
}
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

#include "GameOverState.h"
#include "../Scene2D/Scene2D.h"
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

#include "../App/Source/Scene2D/Player2D.h"

using namespace std;

/**
 @brief Constructor
 */
CGameOverState::CGameOverState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CGameOverState::~CGameOverState(void)
{
}

/**
 @brief Init this class instance
 */
bool CGameOverState::Init(void)
{
	cout << "CGameOverState::Init()\n" << endl;

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/GameOverScreen.jpg");
	background->SetShader("2DShader");
	background->Init();
	
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	CImageLoader* ImageLoad = CImageLoader::GetInstance();
	Retry = (ImTextureID)ImageLoad->LoadTextureGetID("Image\\GUI\\RetryButton.png", false);
	return true;
}

/**
 @brief Update this class instance
 */
bool CGameOverState::Update(const double dElapsedTime)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags =0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;


	float buttonWidth = CSettings::GetInstance()->iWindowWidth * 0.2f; // 256
	float buttonHeight = CSettings::GetInstance()->iWindowHeight * 0.18f; // 128
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	// Game Over Screen
	{
		ImGui::Begin("Game Over", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth,CSettings::GetInstance()->iWindowHeight));
		ImGuiWindowFlags DmgWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Damage Dealt", NULL, DmgWindowFlags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.27875, CSettings::GetInstance()->iWindowHeight * 0.33));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		ImGui::SameLine();
		ImGui::SetWindowFontScale(CSettings::GetInstance()->iWindowWidth * (1.f / 640.f));
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "Damage Dealt: %i", CGameManager::GetInstance()->getFinalDmg());
		ImGui::End();

		ImGui::Begin("Damage Taken", NULL, DmgWindowFlags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.27875, CSettings::GetInstance()->iWindowHeight * 0.4));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		ImGui::SameLine();
		ImGui::SetWindowFontScale(CSettings::GetInstance()->iWindowWidth * (1.f / 640.f));
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "Damage Taken: %i", CGameManager::GetInstance()->getDamageTaken());
		ImGui::End();

		ImGui::Begin("PowerUps Taken", NULL, DmgWindowFlags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.27875, CSettings::GetInstance()->iWindowHeight * 0.47));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		ImGui::SameLine();
		ImGui::SetWindowFontScale(CSettings::GetInstance()->iWindowWidth * (1.f / 640.f));
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "PowerUps Taken: %i", CGameManager::GetInstance()->getTotalPower());
		ImGui::End();

		ImGui::Begin("Floors Cleared", NULL, DmgWindowFlags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.27875, CSettings::GetInstance()->iWindowHeight * 0.54));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		ImGui::SameLine();
		ImGui::SetWindowFontScale(CSettings::GetInstance()->iWindowWidth * (1.f/640.f)); // 2.f
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "Floors Cleared: %i", CScene2D::GetInstance()->floor);
		ImGui::End();

		ImGui::Begin("Retry Button", NULL, DmgWindowFlags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0.41, CSettings::GetInstance()->iWindowHeight * 0.61));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		if (ImGui::ImageButton(Retry, ImVec2(buttonWidth, buttonHeight), ImVec2(0, 0), ImVec2(1, 1), 1)) {
			//Load into the game
			CKeyboardController::GetInstance()->Reset();
			CGameManager::GetInstance()->Reset();
			CGameStateManager::GetInstance()->SetActiveGameState("CharacterSelectionState");
			CSoundController::GetInstance()->StopAllSound();
		}
		ImGui::End();
		ImGui::End();
	}
	
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ENTER))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		CSoundController::GetInstance()->StopAllSound();
		return true;
	}
	return true;
}

/**
 @brief Render this class instance
 */
void CGameOverState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
	background->Render();
	
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief Destroy this class instance
 */
void CGameOverState::Destroy(void)
{
	// Delete the background object
	if (background)
	{
		delete background;
		background = NULL;
	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	cout << "CGameOverState::Destroy()\n" << endl;


}
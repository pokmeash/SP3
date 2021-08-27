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

#include "CharacterSelectionState.h"

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
CCharacterSelectionState::CCharacterSelectionState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CCharacterSelectionState::~CCharacterSelectionState(void)
{
}

/**
 @brief Init this class instance
 */
bool CCharacterSelectionState::Init(void)
{
	cout << "CCharacterSelectionState::Init()\n" << endl;

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/CharacterSelection.jpg");
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
	Character1 = (ImTextureID)ImageLoad->LoadTextureGetID("Image\\GUI\\Character1.png", false);
	Character2 = (ImTextureID)ImageLoad->LoadTextureGetID("Image\\GUI\\Character2.png", false);
	return true;
}

/**
 @brief Update this class instance
 */
bool CCharacterSelectionState::Update(const double dElapsedTime)
{
	
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	float CharacterWidth = CSettings::GetInstance()->iWindowWidth * 0.225f;
	float CharacterHeight = CSettings::GetInstance()->iWindowWidth * 0.1625f;
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		//static float f = 0.0f;
		//static int counter = 0;
		ImGui::Begin("Character Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0f - CharacterWidth,
			CSettings::GetInstance()->iWindowHeight / 3.0f));                // Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight));
		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Add codes for Start button here
		if (ImGui::ImageButton(Character1, ImVec2(CharacterWidth, CharacterHeight), ImVec2(0, 0), ImVec2(1, 1), 1)) {
			//Load into the game
			CPlayer2D::GetInstance()->SelectCharacter(CPlayer2D::Male);
			CKeyboardController::GetInstance()->Reset();
			CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(Character2, ImVec2(CharacterWidth, CharacterHeight), ImVec2(0, 0), ImVec2(1, 1), 1)) {
			//Load into the game
			CPlayer2D::GetInstance()->SelectCharacter(CPlayer2D::Female);
			CKeyboardController::GetInstance()->Reset();
			CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
		}
		ImGui::End();
	}
	return true;
}

/**
 @brief Render this class instance
 */
void CCharacterSelectionState::Render()
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
void CCharacterSelectionState::Destroy(void)
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
	cout << "CCharacterSelectionState::Destroy()\n" << endl;
}
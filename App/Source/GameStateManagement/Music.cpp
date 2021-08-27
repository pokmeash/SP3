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

#include "Music.h"

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
CMusicState::CMusicState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CMusicState::~CMusicState(void)
{
}

/**
 @brief Init this class instance
 */
bool CMusicState::Init(void)
{

	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	cSoundController = CSoundController::GetInstance();

	//Create Background Entity
	background = new CBackgroundEntity("Image/MenuBackground.png");
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

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	startButtonData.fileName = "Image\\GUI\\VolUp.png";
	startButtonData.textureID = il->LoadTextureGetID(startButtonData.fileName.c_str(), false);
	exitButtonData.fileName = "Image\\GUI\\VolDown.png";
	exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);

	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Wii.ogg"), 2, true, true);
	for (int i = 0; i < 6; i++)
	{
		cSoundController->VolumeDecrease(CSoundController::SOUNDS::WII);
	}
	bgm = true;

	return true;
}

/**
 @brief Update this class instance
 */
bool CMusicState::Update(const double dElapsedTime)
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

	float buttonWidth = 256;
	float buttonHeight = 128;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Main Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - buttonWidth / 2.0,
			CSettings::GetInstance()->iWindowHeight / 3.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Add codes for Start button here
		if (ImGui::ImageButton((ImTextureID)startButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			cSoundController->MasterVolumeIncrease();
		}
		// Add codes for Exit button here
		if (ImGui::ImageButton((ImTextureID)exitButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			cSoundController->MasterVolumeDecrease();
		}
		ImGui::End();
	}

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::BUTTON);
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
		cSoundController->StopAllSound();
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::LANCE);
		
	}

	//BGM
	if (bgm)
	{
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::WII);
		bgm = false;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CMusicState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Render Background
	background->Render();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

/**
 @brief Destroy this class instance
 */
void CMusicState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

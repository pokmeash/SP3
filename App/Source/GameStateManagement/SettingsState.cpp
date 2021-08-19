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

#include "SettingsState.h"

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
#include "Inputs/MouseController.h"

#include <iostream>
#include "System\filesystem.h"
using namespace std;

/**
 @brief Constructor
 */
CSettingsState::CSettingsState(void)
	: background(NULL), activeKeybindSetButton(NULL)
{
}

/**
 @brief Destructor
 */
CSettingsState::~CSettingsState(void)
{
}

/**
 @brief Init this class instance
 */
bool CSettingsState::Init(void)
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
	CSettings* cs = CSettings::GetInstance();

	buttonData[EXIT].fileName = "Image/GUI/ExitButton.png";
	buttonData[VOL_UP].fileName = "Image/GUI/VolUp.png";
	buttonData[VOL_DOWN].fileName = "Image/GUI/VolDown.png";
	buttonData[CHANGE_UP].keybindID = CSettings::MOVE_UP;
	buttonData[CHANGE_UP].keybindType = "Up";
	buttonData[CHANGE_DOWN].keybindID = CSettings::MOVE_DOWN;
	buttonData[CHANGE_DOWN].keybindType = "Down";
	buttonData[CHANGE_LEFT].keybindID = CSettings::MOVE_LEFT;
	buttonData[CHANGE_LEFT].keybindType = "Left";
	buttonData[CHANGE_RIGHT].keybindID = CSettings::MOVE_RIGHT;
	buttonData[CHANGE_RIGHT].keybindType = "Right";
	buttonData[CHANGE_SHOOT].keybindID = CSettings::TRIGGER_SHOOT;
	buttonData[CHANGE_SHOOT].keybindType = "Shoot";
	buttonData[CHANGE_ACTIVATE].keybindID = CSettings::TRIGGER_POWERUP;
	buttonData[CHANGE_ACTIVATE].keybindType = "Activate";
	
	for (unsigned i = 0; i < TOTAL_BUTTONS; ++i) {
		if (buttonData[i].keybindID == -1)
		buttonData[i].textureID = il->LoadTextureGetID(buttonData[i].fileName.c_str(), false);
	}

	cSoundController->PlaySoundByID(2);
	for (int i = 0; i < 6; i++)
	{
		cSoundController->VolumeDecrease(2);
	}
	bgm = true;
	std::cout << "CSettingsState::Init\n";
	return true;
}

/**
 @brief Update this class instance
 */
bool CSettingsState::Update(const double dElapsedTime)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	float buttonWidth = 256;
	float buttonHeight = 128;
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Main Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - buttonWidth / 2.0,
			CSettings::GetInstance()->iWindowHeight / 12.0));					// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		if (ImGui::ImageButton((ImTextureID)buttonData[EXIT].textureID, 
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			CKeyboardController::GetInstance()->Reset();
			CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		}

		for (unsigned i = 0; i < TOTAL_BUTTONS; ++i) {
			if (buttonData[i].keybindID == -1) continue;
			if (glfwGetKeyName(CSettings::GetInstance()->iKeybinds[buttonData[i].keybindID], 0) != NULL)
				buttonData[i].buttonName = buttonData[i].keybindType + ": " + glfwGetKeyName(CSettings::GetInstance()->iKeybinds[buttonData[i].keybindID], 0);
			else {
				std::stringstream ss;
				ss << buttonData[i].keybindType << ": " << "Button_" << CSettings::GetInstance()->iKeybinds[buttonData[i].keybindID] << std::endl;
				buttonData[i].buttonName = ss.str();
			}
			if (ImGui::Button(buttonData[i].buttonName.c_str(), ImVec2(buttonWidth * 0.5, buttonHeight * 0.5))) {
				if (activeKeybindSetButton) continue;
				activeKeybindSetButton = &buttonData[i];
			}
		}

		if (activeKeybindSetButton) {
			ImGuiWindowFlags inventoryWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::Begin("waiting", NULL, inventoryWindowFlags);
			ImGui::SetWindowPos(ImVec2(25.0f, 550.0f));
			ImGui::SetWindowSize(ImVec2(200.0f, 25.0f));
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Waiting for input");
			ImGui::End();
			ImGui::PopStyleColor();
			for (int key = 0; key < CKeyboardController::GetInstance()->MAX_KEYS; key++) {
				if (CKeyboardController::GetInstance()->IsKeyPressed(key) || CMouseController::GetInstance()->IsButtonPressed(key)) {
					CSettings::GetInstance()->iKeybinds[activeKeybindSetButton->keybindID] = key;
					CSettings::GetInstance()->SaveKeybinds();
					if (glfwGetKeyName(key, 0) != NULL)
						activeKeybindSetButton->buttonName = activeKeybindSetButton->keybindType + ": " + glfwGetKeyName(key, 0);
					else {
						std::stringstream ss;
						ss << activeKeybindSetButton->keybindType << ": " << "Button_" << key << std::endl;
						activeKeybindSetButton->buttonName = ss.str();
					}
					activeKeybindSetButton = nullptr;
					break;
				}
			}		}

		ImGui::End();
	}
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		CKeyboardController::GetInstance()->Reset();
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		return true;
	}

	//BGM
	if (bgm)
	{
		cSoundController->PlaySoundByID(2);
		bgm = false;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CSettingsState::Render(void)
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
void CSettingsState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	std::cout << "CSettingsState::Destroy\n";

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

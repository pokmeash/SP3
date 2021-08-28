/**
 CGUI_Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"
#include "Scene2D.h"

#include <iostream>
using namespace std;
bool wings = false;
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: cSettings(NULL)
	, m_fProgressBar(0.0f)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Show the mouse pointer
	if (cSettings->bDisableMousePointer == true)
		glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		// Hide the cursor
		if (cSettings->bShowMousePointer == false)
			glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
  @brief Initialise this instance
  */
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

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

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Initialise the cInventoryManager
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Tree as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Tree", "Image/Scene2D_StarTile.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	CImageLoader* ImageLoad = CImageLoader::GetInstance();
	Resume = (ImTextureID)ImageLoad->LoadTextureGetID("Image\\GUI\\ResumeButton.png", false);
	Exit = (ImTextureID)ImageLoad->LoadTextureGetID("Image\\GUI\\ExitButton2.png", false);
	return true;
}

/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create an invisible window which covers the entire OpenGL window
	ImGui::Begin("Invisible window", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
	ImGui::SetWindowFontScale(1.5f);

	// Display the FPS
	//ImGui::TextColored(ImVec4(0, 0, 1, 1), "FPS: %d", cFPSCounter->GetFrameRate());

	// Render the Health
	/*ImGuiWindowFlags healthWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Health", NULL, healthWindowFlags);
	ImGui::SetWindowPos(ImVec2(25.0f, 25.0f));
	ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
	cInventoryItem = cInventoryManager->GetItem("Health");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
	ImGui::ProgressBar(cInventoryItem->GetCount() /
		(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f, 20.0f));
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();*/

	// Render the Lives
	ImGuiWindowFlags livesWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Lives", NULL, livesWindowFlags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0, CSettings::GetInstance()->iWindowHeight * 0.f)); // 0.035f
	ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
	cInventoryItem = cInventoryManager->GetItem("Lives");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::SetWindowFontScale(2.f);
	ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "%i",
		CPlayer2D::GetInstance()->getHP());
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "/%i",
		CPlayer2D::GetInstance()->getMaxHP());
	ImGui::End();

	ImGui::Begin("Damage", NULL, livesWindowFlags);
	ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth * 0, CSettings::GetInstance()->iWindowHeight * 0.045f)); // 0.035f
	ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
	cInventoryItem = cInventoryManager->GetItem("Damage");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::SetWindowFontScale(2.f);
	ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1), "%i",
		CPlayer2D::GetInstance()->getDmg());
	ImGui::End();
	if (wings == true)
	{
		// Render the Lives
		ImGuiWindowFlags WingsWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Wing", NULL, WingsWindowFlags);
		ImGui::SetWindowPos(ImVec2(725.0f, 550.0f));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		cInventoryItem = cInventoryManager->GetItem("Wing");
		ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
			ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}

	for (unsigned i = 0; i < CScene2D::GetInstance()->enemyVector.size(); ++i) {
		CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[i];
		if (!enemy->bIsActive) continue;
		glm::ivec2 screenPos(50, 50);
		CSettings::GetInstance()->ConvertWSToMouseSpace(enemy->vec2WSCoordinate.x - enemy->scale.x * 0.5f, enemy->vec2WSCoordinate.y + enemy->scale.x, &screenPos.x, &screenPos.y);
		ImGuiWindowFlags enemyHealthWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("EnemyHealth"+i, NULL, enemyHealthWindowFlags);
		ImGui::SetWindowPos(ImVec2(screenPos.x, screenPos.y));
		ImGui::SetWindowSize(ImVec2(cSettings->iWindowWidth / cSettings->NUM_TILES_XAXIS, cSettings->iWindowHeight / cSettings->NUM_TILES_YAXIS * 0.25f));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, .0f, 0.0f, 1.0f));
		ImGui::ProgressBar(enemy->getHP() / (float)enemy->getMaxHP(), ImVec2(cSettings->iWindowWidth / cSettings->NUM_TILES_XAXIS * enemy->scale.x, cSettings->iWindowHeight / cSettings->NUM_TILES_YAXIS * 0.25f * enemy->scale.x));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	ImGui::End();
}

void CGUI_Scene2D::updatePause(float dElapsedTime)
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

	float buttonWidth = 180;
	float buttonHeight = 80;
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Pause Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0f - buttonWidth / 2.0f,
			CSettings::GetInstance()->iWindowHeight / 3.0f));                // Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2((float)CSettings::GetInstance()->iWindowWidth, (float)CSettings::GetInstance()->iWindowHeight));
		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;

		// Add codes for Start button here
		if (ImGui::ImageButton(Resume, ImVec2(buttonWidth, buttonHeight), ImVec2(0, 0), ImVec2(1, 1), 1)) {
			//Load into the game

			CScene2D::GetInstance()->setPause(false);
		}

		// Add codes for Exit button here
		if (ImGui::ImageButton(Exit, ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), 1))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();


			CSoundController::GetInstance()->StopAllSound();
			
			// Load the menu state
			cout << "Quitting to main menu" << endl;
			
			CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		}

		ImGui::End();
	}
}



/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}

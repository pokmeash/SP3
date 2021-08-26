#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"
#include "Enemies/SpaceFly.h"
#include "Enemies/SpaceTurret.h"
#include "Enemies/SpaceSkeleton.h"
#include "Enemies/SpaceCannon.h"
#include "Enemies/SpaceSpawner.h"
#include "Bosses/BossTimeControl.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	: cMap2D(NULL)
	, cPlayer2D(NULL)
	, cKeyboardController(NULL)
	, cGUI_Scene2D(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
	, cEntityManager(NULL)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	std::cout << "destroy\n";
	cSoundController = NULL;
	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}

	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// Destroy the enemies
	for (int i = 0; i < enemyVector.size(); i++)
	{
		
		delete enemyVector[i];
		enemyVector[i] = NULL;
	}
	enemyVector.clear();

	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}

	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}

	if (cEntityManager)
	{
		cEntityManager->Destroy();
		cEntityManager = NULL;
	}


	// Clear out all the shaders
	//CShaderManager::GetInstance()->Destroy();
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(void)
{
	// Include Shader Manager
	//CShaderManager::GetInstance()->Add("2DShader", "Shader//Scene2D.vs", "Shader//Scene2D.fs");
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	isPaused = false;
	// Create and initialise the Map 2D
	cMap2D = CFloorManager::GetInstance();
	// Set a shader to this class
	cMap2D->SetShader("2DShader");
	


	cMap2D->GenerateNewLevel(11,24,32);

	// Activate diagonal movement
	cMap2D->SetDiagonalMovement(false);

	// Load Scene2DColor into ShaderManager
	CShaderManager::GetInstance()->Add("2DColorShader", "Shader//Scene2DColor.vs", "Shader//Scene2DColor.fs");
	CShaderManager::GetInstance()->Use("2DColorShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	// Create and initialise the CPlayer2D
	cPlayer2D = CPlayer2D::GetInstance();
	// Pass shader to cPlayer2D
	cPlayer2D->SetShader("2DColorShader");
	// Initialise the instance
	if (cPlayer2D->Init() == false)
	{
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}

	// ENEMEY INIT
	cMap2D->LoadEnemies();

	// Setup the shaders
	CShaderManager::GetInstance()->Add("textShader", "Shader//text.vs", "Shader//text.fs");
	CShaderManager::GetInstance()->Use("textShader");

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Store the cGUI_Scene2D singleton instance here
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	cGUI_Scene2D->Init();

	// Game Manager
	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	//EntityManager
	cEntityManager = EntityManager::GetInstance();
	cEntityManager->Init();

	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Jump.ogg"), 4, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Pickup.ogg"), 5, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Door.ogg"), 6, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Button.ogg"), 7, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Swap.ogg"), 8, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Hurt.ogg"), 9, true);
	CBossTimeControl::GetInstance()->Init();

	for (int i = 1; i < 11; ++i)
	{
		randomvect.push_back(i);
	}
	temp = 0;
	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		isPaused = !isPaused;
	}

	if (isPaused)
	{
		cGUI_Scene2D->updatePause(dElapsedTime);
		return true;
	}
	
	if (!CBossTimeControl::GetInstance()->UpdateReverse()) {
		cPlayer2D->Update(dElapsedTime);
		cEntityManager->Update(dElapsedTime);
		for (unsigned int i = 0; i < enemyVector.size(); i++) enemyVector[i]->Update(dElapsedTime);
		if (CBossTimeControl::GetInstance()->isListening()) {
			CBossTimeControl::GetInstance()->Update();
			if (CBossTimeControl::GetInstance()->getCurrentFrame() >= 60) {
				CBossTimeControl::GetInstance()->setListening(false);
			}
		}
	} else {
		std::cout << "Reversing\n";
		if (CBossTimeControl::GetInstance()->getCurrentFrame() <= 0) {
			CBossTimeControl::GetInstance()->Reset();
			CBossTimeControl::GetInstance()->setListening(false);
		}
	}

	// Call the Map2D's update method
	cMap2D->Update(dElapsedTime);

	if (cKeyboardController->IsKeyDown(GLFW_KEY_H)) {
		if (!CBossTimeControl::GetInstance()->isListening() && CBossTimeControl::GetInstance()->getPackets().size() == 0) {
			CBossTimeControl::GetInstance()->setListening(true);
		}
	}

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_F6))
	{
		// Save the current game to a save file
		// Make sure the file is open
		try {
			if (cMap2D->SaveMap("Maps/DM2213_Map_Level_01_SAVEGAMEtest.csv") == false)
			{
				throw runtime_error("Unable to save the current game to a file");
			}
		}
		catch (runtime_error e)
		{
			cout << "Runtime error: " << e.what();
			return false;
		}
	}

	// Call the cGUI_Scene2D's update method
	cGUI_Scene2D->Update(dElapsedTime);
	if (cGameManager->bPlayerLost == true)
	{
		//cSoundController->PlaySoundByID(2);
		return false;
	}

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{
	for (int i = 0; i < enemyVector.size(); i++)
	{
		// Call the CEnemy2D's PreRender()
		enemyVector[i]->PreRender();
		// Call the CEnemy2D's Render()
		enemyVector[i]->Render();
		// Call the CEnemy2D's PostRender()
		enemyVector[i]->PostRender();
	}

	// Call the CPlayer2D's PreRender()
	cPlayer2D->PreRender();
	// Call the CPlayer2D's Render()
	cPlayer2D->Render();
	// Call the CPlayer2D's PostRender()
	cPlayer2D->PostRender();
	
	cEntityManager->Render();

	// Call the Map2D's PreRender()
	cMap2D->PreRender();
	// Call the Map2D's Render()
	cMap2D->Render();
	// Call the Map2D's PostRender()
	cMap2D->PostRender();

	// Call the cGUI_Scene2D's PreRender()
	cGUI_Scene2D->PreRender();
	// Call the cGUI_Scene2D's Render()
	cGUI_Scene2D->Render();
	// Call the cGUI_Scene2D's PostRender()
	cGUI_Scene2D->PostRender();

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}


void CScene2D::setPause(bool n)
{
	isPaused = n;
}

void CScene2D::LevelCompleted(int DoorDir)
{
	srand(time(NULL));
	int random = 0;

	switch (DoorDir)
	{
		case 0://Top
		{
			if (cMap2D->GetDoorInfo(22,16) != -1)
			{
				cMap2D->SetCurrentLevel(cMap2D->GetDoorInfo(22, 16));
			}
			else
			{
				if (randomvect.size() - 1 > 0)
				{
					std::random_shuffle(randomvect.begin(), randomvect.end()); // 1 to 8 with random order.
					random = (randomvect[randomvect.size() - 1]); // Use the outcome in whatever way.
					randomvect.pop_back();
				}
				cMap2D->SetDoorInfo(22, 16, random);
				temp = cMap2D->GetCurrentLevel();
				cMap2D->SetCurrentLevel(random);
				cMap2D->SetDoorInfo(1, 16, temp);

			}
			cPlayer2D->vec2WSCoordinate.x = 16.5;
			cPlayer2D->vec2WSCoordinate.y = 3;
			break;
		}
		case 1://Right
		{
			if (cMap2D->GetDoorInfo(11, 30) != -1)
			{
				cMap2D->SetCurrentLevel(cMap2D->GetDoorInfo(11, 30));

			}
			else
			{
				if (randomvect.size() - 1 > 0)
				{
					std::random_shuffle(randomvect.begin(), randomvect.end()); // 1 to 8 with random order.
					random = (randomvect[randomvect.size() - 1]); // Use the outcome in whatever way.
					randomvect.pop_back();
				}
				cMap2D->SetDoorInfo(11, 30, random);
				temp = cMap2D->GetCurrentLevel();
				cMap2D->SetCurrentLevel(random);
				cMap2D->SetDoorInfo(11, 1, temp);
			}
			cPlayer2D->vec2WSCoordinate.x = 3;
			cPlayer2D->vec2WSCoordinate.y = 11;
			break;
		}
		case 2://Bot
		{
			if (cMap2D->GetDoorInfo(1, 16) != -1)
			{
				cMap2D->SetCurrentLevel(cMap2D->GetDoorInfo(1, 16));
			}
			else
			{
				if (randomvect.size() - 1 > 0)
				{
					std::random_shuffle(randomvect.begin(), randomvect.end()); // 1 to 8 with random order.
					random = (randomvect[randomvect.size() - 1]); // Use the outcome in whatever way.
					randomvect.pop_back();
				}
				cMap2D->SetDoorInfo(1, 16, random);
				temp = cMap2D->GetCurrentLevel();
				cMap2D->SetCurrentLevel(random);
				cMap2D->SetDoorInfo(22, 16, temp);
			}
			cPlayer2D->vec2WSCoordinate.x = 16.5;
			cPlayer2D->vec2WSCoordinate.y = 21;
			break;
		}
		case 3://Left
		{
			if (cMap2D->GetDoorInfo(11, 1) != -1)
			{
				cMap2D->SetCurrentLevel(cMap2D->GetDoorInfo(11, 1));

			}
			else
			{
				if (randomvect.size() - 1 > 0)
				{
					std::random_shuffle(randomvect.begin(), randomvect.end()); // 1 to 8 with random order.
					random = (randomvect[randomvect.size() - 1]); // Use the outcome in whatever way.
					randomvect.pop_back();
				}
				cMap2D->SetDoorInfo(11, 1, random);
				temp = cMap2D->GetCurrentLevel();
				cMap2D->SetCurrentLevel(random);
				cMap2D->SetDoorInfo(11, 30, temp);
			}
			cPlayer2D->vec2WSCoordinate.x = 28;
			cPlayer2D->vec2WSCoordinate.y = 11;
			break;
		}
		case 4://next floor
		{
			randomvect.clear();
			for (int i = 1; i < 11; ++i)
			{
				randomvect.push_back(i);
			}
			cPlayer2D->vec2WSCoordinate.x = 16;
			cPlayer2D->vec2WSCoordinate.y = 12;

			cMap2D->GenerateNewLevel(11, 24, 32);
		}
		default:
		{
			cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel());
			break;
		}
	}
		
	

	cPlayer2D->Reset();
	cMap2D->once = false;

	//ENEMY
	cMap2D->LoadEnemies();

}

#include "MapManager.h"

CMapManager::CMapManager() : currentFloor(NULL), currentRoom(0) {
	currentFloor = new CMap2D();
}

CMapManager::~CMapManager()
{
	while (floorList.size() > 0)
	{
		CMap2D* floor = floorList.back();
		delete floor;
		floorList.pop_back();
	}
}

void CMapManager::SetShader(const std::string& _name)
{
	CEntity2D::SetShader(_name);
	currentFloor->SetShader(_name);
}

bool CMapManager::Init(const unsigned int uiNumLevels, const unsigned int uiNumRows, const unsigned int uiNumCols)
{
	if (!currentFloor->Init(uiNumLevels, uiNumRows, uiNumCols)) {
		return false;
	}
	floorList.push_back(currentFloor);
	return true;
}

void CMapManager::Update(const double dt)
{
	if (currentFloor) {
		currentFloor->Update(dt);
	}
}

void CMapManager::PreRender()
{
	if (currentFloor) {
		currentFloor->PreRender();
	}
}

void CMapManager::Render()
{
	if (currentFloor) {
		currentFloor->Render();
	}
}

void CMapManager::PostRender()
{
	if (currentFloor) {
		currentFloor->PostRender();
	}
}

void CMapManager::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	currentFloor->SetNumTiles(sAxis, uiValue);
}

void CMapManager::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	currentFloor->SetNumSteps(sAxis, uiValue);
}

void CMapManager::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	currentFloor->SetMapInfo(uiRow, uiCol, iValue, bInvert);
}

int CMapManager::GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	return currentFloor->GetMapInfo(uiRow, uiCol, bInvert);
}

bool CMapManager::LoadMap(string filename, const unsigned int uiLevel)
{
	return currentFloor->LoadMap(filename, uiLevel);
}

bool CMapManager::SaveMap(string filename, const unsigned int uiLevel)
{
	return currentFloor->SaveMap(filename, uiLevel);
}

bool CMapManager::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert)
{
	return currentFloor->FindValue(iValue, uirRow, uirCol, bInvert);
}

void CMapManager::SetCurrentLevel(unsigned int uiCurLevel)
{
	currentFloor->SetCurrentLevel(uiCurLevel);
}

unsigned int CMapManager::GetCurrentLevel(void) const
{
	return currentFloor->GetCurrentLevel();
}

std::vector<glm::i32vec2> CMapManager::PathFind(const glm::i32vec2& startPos, const glm::i32vec2& targetPos, HeuristicFunction heuristicFunc, const int weight)
{
	return currentFloor->PathFind(startPos, targetPos, heuristicFunc, weight);
}

void CMapManager::SetDiagonalMovement(const bool bEnable)
{
	if (currentFloor) {
		currentFloor->SetDiagonalMovement(bEnable);
	}
}


#include "FloorManager.h"

CFloorManager::CFloorManager() : currentFloor(NULL), currentRoom(0) {
	currentFloor = new CFloor2D();
}

CFloorManager::~CFloorManager()
{
	while (floorList.size() > 0)
	{
		CFloor2D* floor = floorList.back();
		delete floor;
		floorList.pop_back();
	}
}

void CFloorManager::SetShader(const std::string& _name)
{
	CEntity2D::SetShader(_name);
	currentFloor->SetShader(_name);
}

bool CFloorManager::Init(const unsigned int uiNumLevels, const unsigned int uiNumRows, const unsigned int uiNumCols)
{
	if (!currentFloor->Init(uiNumLevels, uiNumRows, uiNumCols)) {
		return false;
	}
	floorList.push_back(currentFloor);
	return true;
}

void CFloorManager::Update(const double dt)
{
	if (currentFloor) {
		currentFloor->Update(dt);
	}
}

void CFloorManager::PreRender()
{
	if (currentFloor) {
		currentFloor->PreRender();
	}
}

void CFloorManager::Render()
{
	if (currentFloor) {
		currentFloor->Render();
	}
}

void CFloorManager::PostRender()
{
	if (currentFloor) {
		currentFloor->PostRender();
	}
}

void CFloorManager::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	currentFloor->SetNumTiles(sAxis, uiValue);
}

void CFloorManager::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	currentFloor->SetNumSteps(sAxis, uiValue);
}

void CFloorManager::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	currentFloor->SetMapInfo(uiRow, uiCol, iValue, bInvert);
}

void CFloorManager::SetDoorInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	currentFloor->SetDoorInfo(uiRow, uiCol, iValue, bInvert);
}

int CFloorManager::GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	return currentFloor->GetMapInfo(uiRow, uiCol, bInvert);
}

int CFloorManager::GetDoorInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	return currentFloor->GetDoorInfo(uiRow, uiCol, bInvert);
}

bool CFloorManager::LoadMap(string filename, const unsigned int uiLevel)
{
	return currentFloor->LoadMap(filename, uiLevel);
}

bool CFloorManager::SaveMap(string filename, const unsigned int uiLevel)
{
	return currentFloor->SaveMap(filename, uiLevel);
}

bool CFloorManager::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert)
{
	return currentFloor->FindValue(iValue, uirRow, uirCol, bInvert);
}

void CFloorManager::SetCurrentLevel(unsigned int uiCurLevel)
{
	currentFloor->SetCurrentLevel(uiCurLevel);
}

unsigned int CFloorManager::GetCurrentLevel(void) const
{
	return currentFloor->GetCurrentLevel();
}

std::vector<glm::i32vec2> CFloorManager::PathFind(const glm::i32vec2& startPos, const glm::i32vec2& targetPos, HeuristicFunction heuristicFunc, const int weight)
{
	return currentFloor->PathFind(startPos, targetPos, heuristicFunc, weight);
}

void CFloorManager::SetDiagonalMovement(const bool bEnable)
{
	if (currentFloor) {
		currentFloor->SetDiagonalMovement(bEnable);
	}
}


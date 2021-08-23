#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
#include <vector>
#include "Primitives/Entity2D.h"
#include "Floor2D.h"

class CFloorManager : public CSingletonTemplate<CFloorManager>, public CFloor2D {
	friend CSingletonTemplate<CFloorManager>;
protected:
	std::vector<CFloor2D*> floorList;
	int currentRoom;
	CFloor2D* currentFloor;
	CFloorManager();
	virtual ~CFloorManager();
public:
	void SetShader(const std::string& _name);
	bool Init(const unsigned int uiNumLevels = 1, const unsigned int uiNumRows = 24, const unsigned int uiNumCols = 32);
	void Update(const double dt);
	void PreRender();
	void Render();
	void PostRender();
	void SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue);
	void SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue);
	void SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert = true);
	void SetDoorInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert = true);
	int GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert = true) const;
	int GetDoorInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert = true) const;
	bool LoadMap(string filename, const unsigned int uiLevel = 0);
	bool SaveMap(string filename, const unsigned int uiLevel = 0);
	bool FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert = true);
	void SetCurrentLevel(unsigned int uiCurLevel);
	unsigned int GetCurrentLevel(void) const;
	std::vector<glm::i32vec2> PathFind(const glm::i32vec2& startPos, const glm::i32vec2& targetPos, HeuristicFunction heuristicFunc, const int weight = 1);
	void SetDiagonalMovement(const bool bEnable);
	CFloor2D* GetCurrentFloor() const;
	CFloor2D* ProduceMap(unsigned int level) {
		CFloor2D* map = new CFloor2D();
		map->SetShader(sShaderName);
		if (!map->Init()) return NULL;
		floorList.push_back(map);
		return map;
	}
};
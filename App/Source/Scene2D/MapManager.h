#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"
// Include vector
#include <vector>
#include "Primitives/Entity2D.h"
#include "Map2D.h"

class CMapManager : public CSingletonTemplate<CMapManager>, public CMap2D {
	friend CSingletonTemplate<CMapManager>;
protected:
	std::vector<CMap2D*> floorList;
	int currentRoom;
	CMap2D* currentFloor;
	CMapManager();
	virtual ~CMapManager();
public:
	void SetShader(const std::string& _name);
	bool Init(const unsigned int uiNumLevels = 1, const unsigned int uiNumRows = 24, const unsigned int uiNumCols = 32);

	void Update(const double dt);
	void PreRender();
	void Render();
	void PostRender();

	// Set the specifications of the map
	void SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue);

	void SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue);

	// Set the value at certain indices in the arrMapInfo
	void SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert = true);

	// Get the value at certain indices in the arrMapInfo
	int GetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert = true) const;

	// Load a map
	bool LoadMap(string filename, const unsigned int uiLevel = 0);

	// Save a tilemap
	bool SaveMap(string filename, const unsigned int uiLevel = 0);

	// Find the indices of a certain value in arrMapInfo
	bool FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert = true);

	// Set current level
	void SetCurrentLevel(unsigned int uiCurLevel);
	// Get current level

	unsigned int GetCurrentLevel(void) const;

	// For AStar PathFinding
	std::vector<glm::i32vec2> PathFind(const glm::i32vec2& startPos,
		const glm::i32vec2& targetPos,
		HeuristicFunction heuristicFunc,
		const int weight = 1);

	void SetDiagonalMovement(const bool bEnable);
};
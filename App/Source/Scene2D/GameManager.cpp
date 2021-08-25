#include "GameManager.h"

void CGameManager::setFinalDmg(int dmg)
{
	FinalDmg = dmg;
}

int CGameManager::getFinalDmg()
{
	return FinalDmg;
}

void CGameManager::addFinalDmg(int Finaldmg)
{
	FinalDmg += Finaldmg;
}

/**
@brief Constructor
*/
CGameManager::CGameManager(void) 
	: bPlayerWon(false)
	, bPlayerLost(false)
	, bLevelCompleted(false)
	, bLevelToReplay(false)
	, bGameToRestart(false) {
}

/**
@brief Destructor
*/
CGameManager::~CGameManager(void)
{
}

// Initialise this class instance
void CGameManager::Init(void)
{
	bPlayerWon = false;
	bPlayerLost = false;
	bLevelCompleted = false;
	bLevelToReplay = false;
	bGameToRestart = false;
}


#include "GameManager.h"
int CGameManager::getFinalDmg()
{
	return FinalDmg;
}

void CGameManager::setFloor(int floor)
{
	this->floor = floor;
}


void CGameManager::addFinalDmg(int Finaldmg)
{
	FinalDmg += Finaldmg;
}

int CGameManager::getDamageTaken()
{
	return DamageTaken;
}

void CGameManager::addDamageTaken(int dmgTaken)
{
	DamageTaken += dmgTaken;
}

void CGameManager::addPowerUp(int power)
{
	totalPowerUp += power;
}

int CGameManager::getTotalPower()
{
	return totalPowerUp;
}

int CGameManager::getFloor()
{
	return floor;
}

void CGameManager::Reset()
{
	FinalDmg = 0;
	DamageTaken = 0;
	totalPowerUp = 0;
	floor = 0;
}

/**
@brief Constructor
*/
CGameManager::CGameManager(void) 
	: bPlayerWon(false)
	, bPlayerLost(false)
	, bLevelCompleted(false)
	, bLevelToReplay(false)
	, bGameToRestart(false)
	, FinalDmg(0)
	, DamageTaken(0)	
	, totalPowerUp(0)
	, floor(0){
}

/**
@brief Destructor
*/
CGameManager::~CGameManager(void)
{
	cout << "GameManager Destroyed\n";
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


#pragma once

#include "EntityFactory.h"
#include "FloorManager.h"
#include "DesignPatterns/SingletonTemplate.h"
#include "GameControl/Settings.h"

#include <vector>

class EntityManager : public CSingletonTemplate<EntityManager>
{
	friend CSingletonTemplate<EntityManager>;
public:

    // Handler to the CMap2D instance
    CFloor2D* cMap2D;
    CPhysics2D cPhysics;

    // Constructor
    EntityManager(void);

    // Destructor
    ~EntityManager(void);

    // Init
    bool Init(void);

    // Update
    void Update(const double dElapsedTime);

    // Render
    void Render(void);
    
    std::vector<CEntity2D*> entitylist;

    bool getExplode();
    void setExplode();

    glm::vec2 getPos();
private:
    //int hitcount = 3;
    bool explode = false;
    glm::vec2 tempPos;
};


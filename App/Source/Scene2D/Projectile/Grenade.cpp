#include "Grenade.h"
#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DMoveEvent.h"
#include "EventControl/Entity2DDespawnEvent.h"
#include "EventControl/GrenadeExplodeEvent.h"
#include"RenderControl/ShaderManager.h"
#include"System/ImageLoader.h"
#include"Primitives/MeshBuilder.h"
#include "../EntityFactory.h"
#include "../FloorManager.h"
#include "../Particles/ParticleManager.h"
#include "../../SoundController/SoundController.h"

Grenade::Grenade()
{
    rotation = 0.f;
    scale = glm::vec3(1, 1, 1);
    explode = false;
}
Grenade::~Grenade()
{}

bool Grenade::Init() {
    cSettings = CSettings::GetInstance();
    cMap2D = CFloorManager::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/Grenade.png");
}
void Grenade::Update(const double dElapsedTime)
{
    if (!bIsActive) return;
    vec2WSOldCoordinates = vec2WSCoordinate;
	vec2Velocity *= 0.9;
    vec2WSCoordinate += vec2Velocity;
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
	timer -= dElapsedTime;
	// if out of bounds delete
	if (i32vec2Index.x < 0 || i32vec2Index.x > CSettings::GetInstance()->NUM_TILES_XAXIS ||
		i32vec2Index.y < 0 || i32vec2Index.y > CSettings::GetInstance()->NUM_TILES_YAXIS) {
		bIsActive = false;
	}
    if (!bIsActive) return;
    if (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100 && vec2Velocity.y > 0)
    {
        vec2Velocity.y *= -1;
        CSoundController::GetInstance()->Replay(CSoundController::RICOCHET);
    } else if (cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) >= 100 && vec2Velocity.y < 0 && i32vec2NumMicroSteps.y <= cSettings->NUM_STEPS_PER_TILE_YAXIS * 0.5f)
    {
        vec2Velocity.y *= -1;
        CSoundController::GetInstance()->Replay(CSoundController::RICOCHET);
    }
    if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100 && vec2Velocity.x > 0)
    {
        vec2Velocity.x *= -1;
        CSoundController::GetInstance()->Replay(CSoundController::RICOCHET);
    } else if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) >= 100 && vec2Velocity.x < 0 && i32vec2NumMicroSteps.x <= cSettings->NUM_STEPS_PER_TILE_XAXIS * 0.25f)
    {
        vec2Velocity.x *= -1;
        CSoundController::GetInstance()->Replay(CSoundController::RICOCHET);
    }
	if (timer <= 0)
	{
        setExplode(true);
	}
    if (vec2WSOldCoordinates != vec2WSCoordinate) {
        if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
            vec2WSCoordinate = vec2WSOldCoordinates;
        }
    }
    rotation = atan2f(vec2Velocity.y, vec2Velocity.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
}

void Grenade::setExplode(bool explode)
{
    this->explode = explode;
    if (this->explode) {
        ParticleManager::GetInstance()->SpawnParticle(Particle::PARTICLE_TYPE::EXPLOSION, vec2WSCoordinate, 4);
        EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(this));
        EventHandler::GetInstance()->CallThenDelete(new GrenadeExplodeEvent(this));
        for (int j = 0; j < 3; j++)
        {
            EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, glm::vec2(CSettings::GetInstance()->Random(-2, 2), CSettings::GetInstance()->Random(-2, 2)), glm::vec3(1, 1, 1), E_FRAGMENT);
        }
        bIsActive = false;
    }
}


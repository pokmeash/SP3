#include "Bullet.h"

#include "RenderControl/ShaderManager.h"
#include "System/ImageLoader.h"
#include "Primitives/MeshBuilder.h"
#include "EventControl/EventHandler.h"
#include "../FloorManager.h"

Bullet::Bullet()
{
    rotation = 0.f;
    scale = glm::vec3(1, 1, 1);
}
Bullet::~Bullet()
{}

bool Bullet::Init() {
    cSettings = CSettings::GetInstance();
    cMap2D = CFloorManager::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/Bullet.tga");
}
void Bullet::Update(const double dElapsedTime)
{
    if (!bIsActive) return;
    vec2WSOldCoordinates = vec2WSCoordinate;
    vec2WSCoordinate += vec2Velocity;
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
    if (i32vec2Index.x < 0 || i32vec2Index.x > CSettings::GetInstance()->NUM_TILES_XAXIS ||
        i32vec2Index.y < 0 || i32vec2Index.y > CSettings::GetInstance()->NUM_TILES_YAXIS) {
        bIsActive = false;
    }
    if (type == E_EBULLET && glm::length(vec2WSCoordinate - CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1.f) {
        CPlayer2D::GetInstance()->PlayerDamaged();
        bIsActive = false;
        EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(this));
    } else if (type == E_FRAGMENT) {
        vec2Velocity *= 0.8;
        if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100 || glm::length(vec2Velocity) <= 0.1f)
        {
            bIsActive = false;
            EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(this));
        }
    }
    if (!bIsActive) return;

    if (type == E_BULLET)
    {
        if (i32vec2Index.y < CSettings::GetInstance()->NUM_TILES_YAXIS && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100 && vec2Velocity.y > 0)
        {
            vec2Velocity.y *= -1;
            counter--;
        }
        else if (i32vec2Index.y > 0 && cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) >= 100 && vec2Velocity.y < 0 && i32vec2NumMicroSteps.y <= cSettings->NUM_STEPS_PER_TILE_YAXIS * 0.5f)
        {
            vec2Velocity.y *= -1;
            counter--;
        }
        if (i32vec2Index.x < CSettings::GetInstance()->NUM_TILES_XAXIS && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100 && vec2Velocity.x > 0)
        {
            vec2Velocity.x *= -1;
            counter--;
        }
        else if (i32vec2Index.x > 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) >= 100 && vec2Velocity.x < 0 && i32vec2NumMicroSteps.x <= cSettings->NUM_STEPS_PER_TILE_XAXIS * 0.25f)
        {
            vec2Velocity.x *= -1;
            counter--;
        }
        if (counter <= 0)
        {
            bIsActive = false;
            EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(this));
        }
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

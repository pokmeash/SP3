#include "Beam.h"

#include "RenderControl/ShaderManager.h"
#include "System/ImageLoader.h"
#include "Primitives/MeshBuilder.h"
#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DDespawnEvent.h"

Beam::Beam()
{
    rotation = 0.f;
    scale = glm::vec3(1, 1, 1);
    timer = 0.1f;
}
Beam::~Beam()
{
    hitEntities.clear();
}

bool Beam::Init() {
    cSettings = CSettings::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/Beam.tga");
}

void Beam::Update(const double dElapsedTime)
{
    rotation = atan2f(vec2Velocity.y, vec2Velocity.x);
    timer -= dElapsedTime;
    if (timer <= 0.f) {
        bIsActive = false;
        EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(this));
        return;
    }
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
    CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
}

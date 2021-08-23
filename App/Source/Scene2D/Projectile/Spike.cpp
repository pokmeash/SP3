#include "Spike.h"

#include"RenderControl/ShaderManager.h"
#include"System/ImageLoader.h"
#include"Primitives/MeshBuilder.h"
#include "EventControl/EventHandler.h"

Spike::Spike()
{}
Spike::~Spike()
{}

bool Spike::Init() {
    cSettings = CSettings::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/Spike.png");
}
void Spike::Update(const double dElapsedTime)
{
    vec2WSOldCoordinates = vec2WSCoordinate;
    vec2WSCoordinate += vec2Velocity;
    if (vec2WSOldCoordinates != vec2WSCoordinate) {
        if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
            vec2WSCoordinate = vec2WSOldCoordinates;
        }
    }
}
#include "Spike.h"

#include"RenderControl/ShaderManager.h"
#include"System/ImageLoader.h"
#include"Primitives/MeshBuilder.h"

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
    vec2WSCoordinate += vec2Velocity;
}
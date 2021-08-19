#include "DoubleShot.h"

#include"RenderControl/ShaderManager.h"
#include"System/ImageLoader.h"
#include"Primitives/MeshBuilder.h"

DoubleShot::DoubleShot()
{}
DoubleShot::~DoubleShot()
{}

bool DoubleShot::Init() {
    cSettings = CSettings::GetInstance();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
    return LoadTexture("Image/DoubleShot.png");
}
void DoubleShot::Update(const double dElapsedTime)
{
}
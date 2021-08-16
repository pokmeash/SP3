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
    EntityVec2Index += EntityVec2Vel;

    vec2UVCoordinate.x = cSettings->ConvertFloatToUVSpace(cSettings->x, EntityVec2Index.x, false);
    vec2UVCoordinate.y = cSettings->ConvertFloatToUVSpace(cSettings->y, EntityVec2Index.y, false);
}

void Spike::PreRender(void)
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CShaderManager::GetInstance()->Use("2DShader");
}

void Spike::Render(void)
{
    glBindVertexArray(VAO);
    unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
    unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
        vec2UVCoordinate.y,
        0.0f));
    transform = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));
    transform = glm::scale(transform, EntityVec3Scale);

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glBindTexture(GL_TEXTURE_2D, iTextureID);

    mesh->Render();

    glBindVertexArray(0);
}

void Spike::PostRender(void)
{
    glDisable(GL_BLEND);
}
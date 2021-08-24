/**
 CEntity2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Entity2D.h"
#include "MeshBuilder.h"

 // Include Shader Manager
#include "..\RenderControl\ShaderManager.h"

// Include ImageLoader
#include "..\System\ImageLoader.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEntity2D::CEntity2D(void)
	: VAO(0)
	, VBO(0)
	, EBO(0)
	, iTextureID(0)
	, cSettings(NULL)
	, mesh(NULL)
	, animatedSprites(NULL)
	, bIsActive(false)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first
	rotation = 0.f;
	scale = glm::vec3(1, 1, 1);
	i32vec2Index = glm::i32vec2(0);
	i32vec2NumMicroSteps = glm::i32vec2(0);
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEntity2D::~CEntity2D(void)
{
	if (mesh)
		delete mesh;
}

/**
  @brief Initialise this instance
  */
bool CEntity2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Draw a quad for a default entity2D
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	iHealth = 4;
	return true;
}


/**
 @brief Set the name of the shader to be used in this class instance
 @param _name The name of the Shader instance in the CShaderManager
 */
 void CEntity2D::SetShader(const std::string& _name)
 {
	 this->sShaderName = _name;
 }

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEntity2D::PreRender(void)
{
	if (!bIsActive) return;
	vec2UVCoordinate.x = cSettings->ConvertFloatToUVSpace(cSettings->x, vec2WSCoordinate.x, false);
	vec2UVCoordinate.y = cSettings->ConvertFloatToUVSpace(cSettings->y, vec2WSCoordinate.y, false);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CShaderManager::GetInstance()->Use("2DShader");
}

/**
 @brief Render this instance
 */
void CEntity2D::Render(void)
{
	if (!bIsActive) return;
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x, vec2UVCoordinate.y, 0.0f));
	transform = glm::rotate(transform, rotation, glm::vec3(0, 0, 1));
	transform = glm::scale(transform, scale);
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	glBindVertexArray(VAO);

	//CS: Use mesh to render
	if (mesh) mesh->Render();
	else if (animatedSprites) animatedSprites->Render();

	glBindVertexArray(0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEntity2D::PostRender(void)
{
	if (!bIsActive) return;
	// Disable blending
	glDisable(GL_BLEND);
}

glm::vec2 CEntity2D::vec2GetCenter()
{
	glm::vec2 temp;
	temp.x = i32vec2Index.x + 1;
	temp.y = i32vec2Index.y + 1;
	return temp;
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CEntity2D::LoadTexture(const char* filename)
{
	// Variables used in loading the texture
	int width, height, nrChannels;
	
	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

bool CEntity2D::LoadTexture(const char* filename, GLuint& iTextureID)
{
	// Variables used in loading the texture
	int width, height, nrChannels;

	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char* data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}


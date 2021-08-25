/**
 CEntity2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include Settings
#include "..\GameControl\Settings.h"
#include <iostream>
#include <string>
#include <map>

//CS: Include Mesh.h to use to draw (include vertex and index buffers)
#include "Mesh.h"
#include "SpriteAnimation.h"
#include "DynamicMetadata.h"

using namespace std;

class CEntity2D
{
protected:
	// Name of Shader Program instance
	std::string sShaderName;

	//CS: The mesh that is used to draw objects
	CMesh* mesh;

	CSpriteAnimation* animatedSprites;

	// OpenGL objects
	unsigned int VBO, VAO, EBO;

	// The texture ID in OpenGL
	unsigned int iTextureID;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// Settings
	CSettings* cSettings;

	std::map<std::string, AbstractMetadata*> metaData;
public:
	// Constructor
	CEntity2D(void);

	// Destructor
	virtual ~CEntity2D(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime) { std::cout << "CEntity2D::Update\n"; }

	// PreRender
	virtual void PreRender(void);

	// Render
	virtual void Render(void);

	// PostRender
	virtual void PostRender(void);

	// Set the name of the shader to be used in this class instance
	virtual void SetShader(const std::string& _name);

	virtual bool LoadTexture(const char* filename);

	virtual bool LoadTexture(const char* filename, GLuint& iTextureID);

	// The i32vec2 which stores the indices of an Entity2D in the Map2D
	glm::i32vec2 i32vec2Index;

	// The i32vec2 variable which stores The number of microsteps from the tile indices for the Entity2D. 
	// A tile's width or height is in multiples of these microsteps
	glm::i32vec2 i32vec2NumMicroSteps;

	// The vec2 variable which stores the UV coordinates to render the Entity2D
	glm::vec2 vec2UVCoordinate;

	glm::vec2 vec2WSOldCoordinates;

	glm::vec2 vec2WSCoordinate;

	glm::vec2 vec2Velocity;

	glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);

	int counter;

	float timer;

	glm::vec2 vec2GetCenter();

	float rotation = 0.f;

	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	enum ENTITY_TYPE {
		E_NULL,

		//Projectile
		E_BULLET,
		E_EBULLET,
		E_SPIKE,
		E_FRAGMENT,
		E_BEAM,

		//Powerup
		E_DOUBLESHOT,
		E_ENEMY,
		E_GRENADE,
	};
	ENTITY_TYPE type;
	bool bIsActive;
	int iHealth;
	void addMetadata(std::string s, AbstractMetadata* metadata) {
		if (hasMetadata(s)) return;
		metaData.insert(std::pair<std::string, AbstractMetadata*>(s, metadata));
	}
	bool hasMetadata(std::string s) {
		return metaData.count(s) != 0;
	}
	AbstractMetadata* getMetadata(std::string s) {
		if (hasMetadata(s)) {
			return metaData.at(s);
		}
		return nullptr;
	}
};

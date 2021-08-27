/**
 Map2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Floor2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include Filesystem
#include "System\filesystem.h"
// Include ImageLoader
#include "System\ImageLoader.h"
#include "Primitives/MeshBuilder.h"
#include "System\MyMath.h"
#include <iostream>
#include <vector>

#include "Scene2D.h"

#include "Enemies/SpaceGoop.h"
#include "Enemies/SpaceFly.h"
#include "Enemies/SpaceTurret.h"
#include "Enemies/SpaceSkeleton.h"
#include "Enemies/SpaceCannon.h"
#include "Bosses/Boss2D.h"
#include "Bosses/ContagionBoss.h"


using namespace std;

// For AStar PathFinding
using namespace std::placeholders;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CFloor2D::CFloor2D(void)
	: uiCurRoom(0)
{
	
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CFloor2D::~CFloor2D(void)
{
	if (!cSettings) return;
	// Delete AStar lists
	DeleteAStarLists();

	// Dynamically deallocate the 3D array used to store the map information
	for (unsigned int uiLevel = 0; uiLevel < uiNumRooms; uiLevel++)
	{
		for (unsigned int iRow = 0; iRow < cSettings->NUM_TILES_YAXIS; iRow++)
		{
			delete[] arrMapInfo[uiLevel][iRow];
		}
		delete [] arrMapInfo[uiLevel];
	}
	arrMapInfo.clear();

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
@brief Init Initialise this instance
*/ 
bool CFloor2D::Init(	const unsigned int uiNumLevels,
					const unsigned int uiNumRows,
					const unsigned int uiNumCols)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	// Create the arrMapInfo and initialise to 0
	// Start by initialising the number of levels
	arrMapInfo.reserve(uiNumLevels);
	for (unsigned uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		arrMapInfo.push_back(new Grid * [uiNumRows]);
		for (unsigned uiRow = 0; uiRow < uiNumRows; uiRow++)
		{
			arrMapInfo[uiLevel][uiRow] = new Grid[uiNumCols];
			for (unsigned uiCol = 0; uiCol < uiNumCols; uiCol++)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
			}
		}
	}
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load and create textures
	// Load the tree texture
	if (LoadTexture("Image/Scene2D_StarTile.tga", 2) == false)
	{
		std::cout << "Failed to load star tile texture" << std::endl;
		return false;
	}
	// Load the wings texture
	if (LoadTexture("Image/Scene2D_WingsTile.tga", 4) == false)
	{
		std::cout << "Failed to load Wing tile texture" << std::endl;
		return false;
	}
	// Load the Life texture
	if (LoadTexture("Image/Scene2D_Lives.tga", 10) == false)
	{
		std::cout << "Failed to load Scene2D_Lives tile texture" << std::endl;
		return false;
	}
	// Load Speed up power up
	if (LoadTexture("Image/SpeedUp.tga", 11) == false)
	{
		std::cout << "Failed to load SpeedUp tile texture" << std::endl;
		return false;
	}
	// Load Attack up power up
	if (LoadTexture("Image/attackup.tga", 12) == false)
	{
		std::cout << "Failed to load Attack up tile texture" << std::endl;
		return false;
	}
	// Load Health up power up
	if (LoadTexture("Image/GoldenHeart.png", 13) == false)
	{
		std::cout << "Failed to load Health up tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/health.tga", 14) == false)
	{
		std::cout << "Failed to load Health up tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Mirror.png", 15) == false)
	{
		std::cout << "Failed to load Ricochet power up tile texture" << std::endl;
		return false;

	}
	// Load the spike texture
	if (LoadTexture("Image/Scene2D_Lava.tga", 20) == false)
	{
		std::cout << "Failed to load Scene2D_Lava tile texture" << std::endl;
		return false;
	}
	// Load the Spa texture
	if (LoadTexture("Image/Scene2D_Spa.tga", 21) == false)
	{
		std::cout << "Failed to load Scene2D_Spa tile texture" << std::endl;
		return false;
	}
	// Load the Invis texture
	if (LoadTexture("Image/Scene2D_Exit.tga", 96) == false)
	{
		std::cout << "Failed to load Exit tile texture" << std::endl;
		return false;
	}
	// Load the Exit texture
	if (LoadTexture("Image/Scene2D_UsedExit.tga", 97) == false)
	{
		std::cout << "Failed to load Exit tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Scene2D_Exit.tga", 98) == false)
	{
		std::cout << "Failed to load Exit tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Scene2D_Exit.tga", 99) == false)
	{
		std::cout << "Failed to load Exit tile texture" << std::endl;
		return false;
	}
	// Load the Locked Exit texture
	if (LoadTexture("Image/Scene2D_LockedExit.tga", 100) == false)
	{
		std::cout << "Failed to load LockedExit tile texture" << std::endl;
		return false;
	}
	// Load the ground texture
	if (LoadTexture("Image/Scene2D_GroundTile.tga", 101) == false)
	{
		std::cout << "Failed to load ground tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Scene2D_GroundTile.tga", 102) == false)
	{
		std::cout << "Failed to load ground tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Scene2D_GroundTile.tga", 103) == false)
	{
		std::cout << "Failed to load ground tile texture" << std::endl;
		return false;
	}
	if (LoadTexture("Image/Scene2D_GroundTile.tga", 104) == false)
	{
		std::cout << "Failed to load ground tile texture" << std::endl;
		return false;
	}

	// Store the map sizes in cSettings
	uiCurRoom = 0;
	this->uiNumRooms = uiNumLevels;
	cSettings->NUM_TILES_XAXIS = uiNumCols;
	cSettings->NUM_TILES_YAXIS = uiNumRows;
	cSettings->UpdateSpecifications();

	// Initialise the variables for AStar
	m_weight = 1;
	m_startPos = glm::i32vec2(0, 0);
	m_targetPos = glm::i32vec2(0, 0);
	//m_size = cSettings->NUM_TILES_YAXIS* cSettings->NUM_TILES_XAXIS;

	m_nrOfDirections = 4;
	m_directions = { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 },
						{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 } };

	// Resize these 2 lists
	m_cameFromList.resize(cSettings->NUM_TILES_YAXIS * cSettings->NUM_TILES_XAXIS);
	m_closedList.resize(cSettings->NUM_TILES_YAXIS * cSettings->NUM_TILES_XAXIS, false);

	//// Clear AStar memory
	//ClearAStar();

	return true;
}

/**
@brief Update Update this instance
*/
void CFloor2D::Update(const double dElapsedTime)
{
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CFloor2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 */
void CFloor2D::Render(void)
{
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// Render
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			transform = glm::translate(transform, glm::vec3(cSettings->ConvertIndexToUVSpace(cSettings->x, uiCol, false, 0),
															cSettings->ConvertIndexToUVSpace(cSettings->y, uiRow, true, 0),
															0.0f));
			//transform = glm::rotate(transform, Math::HALF_PI, glm::vec3(0.0f, 0.0f, 1.0f));
			//transform = glm::scale(transform, glm::vec3(1.5, 1, 1));

			// Update the shaders with the latest transform
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

			// Render a tile
			RenderTile(uiRow, uiCol);
		}
	}
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CFloor2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

// Set the specifications of the map
void CFloor2D::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumTiles() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_TILES_XAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_TILES_YAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

// Set the specifications of the map
void CFloor2D::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumSteps() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_STEPS_PER_TILE_XAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_STEPS_PER_TILE_YAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

/**
 @brief Set the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to set to
 @param iCol A const int variable containing the column index of the element to set to
 @param iValue A const int variable containing the value to assign to this arrMapInfo
 */
void CFloor2D::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	if (bInvert)
		arrMapInfo[uiCurRoom][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value = iValue;
	else
		arrMapInfo[uiCurRoom][uiRow][uiCol].value = iValue;
}

void CFloor2D::SetDoorInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	if (bInvert)
		arrMapInfo[uiCurRoom][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].roomid = iValue;
	else
		arrMapInfo[uiCurRoom][uiRow][uiCol].roomid = iValue;
}
/**
 @brief Get the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to get from
 @param iCol A const int variable containing the column index of the element to get from
 @param bInvert A const bool variable which indicates if the row information is inverted
 */
int CFloor2D::GetMapInfo(const unsigned int uiRow, const int unsigned uiCol, const bool bInvert) const
{
	if (bInvert)
		return arrMapInfo[uiCurRoom][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value;
	else
		return arrMapInfo[uiCurRoom][uiRow][uiCol].value;
}

int CFloor2D::GetDoorInfo(const unsigned int uiRow, const int unsigned uiCol, const bool bInvert) const
{
	if (bInvert)
		return arrMapInfo[uiCurRoom][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].roomid;
	else
		return arrMapInfo[uiCurRoom][uiRow][uiCol].roomid;
}

/**
 @brief Load a map
 */ 
bool CFloor2D::LoadMap(string filename, const unsigned int uiCurLevel)
{
	GeneratePreset(uiCurLevel);
	return true;
	doc = rapidcsv::Document(FileSystem::getPath(filename).c_str());

	// Check if the sizes of CSV data matches the declared arrMapInfo sizes
	if ((cSettings->NUM_TILES_XAXIS != (unsigned int)doc.GetColumnCount()) ||
		(cSettings->NUM_TILES_YAXIS != (unsigned int)doc.GetRowCount()))
	{
		cout << "Sizes of CSV map does not match declared arrMapInfo sizes." << endl;
		return false;
	}

	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		// Read a row from the CSV file
		std::vector<std::string> row = doc.GetRow<std::string>(uiRow);
		
		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
		{
			arrMapInfo[uiCurLevel][uiRow][uiCol].value = (int)stoi(row[uiCol]);
		}
	}

	return true;
}

/**
 @brief Save the tilemap to a text file
 @param filename A string variable containing the name of the text file to save the map to
 */
bool CFloor2D::SaveMap(string filename, const unsigned int uiCurLevel)
{
	// Update the rapidcsv::Document from arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			doc.SetCell(uiCol, uiRow, arrMapInfo[uiCurLevel][uiRow][uiCol].value);
		}
		cout << endl;
	}

	// Save the rapidcsv::Document to a file
	doc.Save(FileSystem::getPath(filename).c_str());

	return true;
}

/**
@brief Find the indices of a certain value in arrMapInfo
@param iValue A const int variable containing the row index of the found element
@param iRow A const int variable containing the row index of the found element
@param iCol A const int variable containing the column index of the found element
@param bInvert A const bool variable which indicates if the row information is inverted
*/
bool CFloor2D::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert)
{
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (arrMapInfo[uiCurRoom][uiRow][uiCol].value == iValue)
			{
				if (bInvert)
					uirRow = cSettings->NUM_TILES_YAXIS - uiRow - 1;
				else
					uirRow = uiRow;
				uirCol = uiCol;
				return true;	// Return true immediately if the value has been found
			}
		}
	}
	return false;
}

/**
 @brief Set current level
 */
void CFloor2D::SetCurrentLevel(unsigned int uiCurLevel)
{
	if (uiCurLevel < uiNumRooms)
	{
		this->uiCurRoom = uiCurLevel;
	}
}
/**
 @brief Get current level
 */
unsigned int CFloor2D::GetCurrentLevel(void) const
{
	return uiCurRoom;
}


/**
 @brief Load a texture, assign it a code and store it in MapOfTextureIDs.
 @param filename A const char* variable which contains the file name of the texture
 @param iTextureCode A const int variable which is the texture code.
 */
bool CFloor2D::LoadTexture(const char* filename, const int iTextureCode)
{
	// Variables used in loading the texture
	int width, height, nrChannels;
	unsigned int textureID;

	// texture 1
	// ---------
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
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

		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(iTextureCode, textureID));
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

/**
 @brief Render a tile at a position based on its tile index
 @param iRow A const int variable containing the row index of the tile
 @param iCol A const int variable containing the column index of the tile
 */
void CFloor2D::RenderTile(const unsigned int uiRow, const unsigned int uiCol)
{
	if (arrMapInfo[uiCurRoom][uiRow][uiCol].value > 1)
	{
		//if (arrMapInfo[uiCurLevel][uiRow][uiCol].value < 3)
		glBindTexture(GL_TEXTURE_2D, MapOfTextureIDs.at(arrMapInfo[uiCurRoom][uiRow][uiCol].value));

		glBindVertexArray(VAO);
		//CS: Render the tile
		quadMesh->Render();
		glBindVertexArray(0);
	}
}


/**
 @brief Find a path
 */
std::vector<glm::i32vec2> CFloor2D::PathFind(	const glm::i32vec2& startPos, 
											const glm::i32vec2& targetPos, 
											HeuristicFunction heuristicFunc, 
											const int weight)
{
	// Check if the startPos and targetPost are blocked
	if (isBlocked(startPos.y, startPos.x) ||
		(isBlocked(targetPos.y, targetPos.x)))
	{
		cout << "Invalid start or target position." << endl;
		// Return an empty path
		std::vector<glm::i32vec2> path;
		return path;
	}

	// Set up the variables and lists
	m_startPos = startPos;
	m_targetPos = targetPos;
	m_weight = weight;
	m_heuristic = std::bind(heuristicFunc, _1, _2, _3);

	// Reset AStar lists
	ResetAStarLists();

	// Add the start pos to 2 lists
	m_cameFromList[ConvertTo1D(m_startPos)].parent = m_startPos;
	m_openList.push(Grid(m_startPos, 0));

	unsigned int fNew, gNew, hNew;
	glm::i32vec2 currentPos;

	// Start the path finding...
	while (!m_openList.empty())
	{
		// Get the node with the least f value
		currentPos = m_openList.top().pos;
		//cout << endl << "*** New position to check: " << currentPos.x << ", " << currentPos.y << endl;
		//cout << "*** targetPos: " << m_targetPos.x << ", " << m_targetPos.y << endl;

		// If the targetPos was reached, then quit this loop
		if (currentPos == m_targetPos)
		{
			//cout << "=== Found the targetPos: " << m_targetPos.x << ", " << m_targetPos.y << endl;
			while (m_openList.size() != 0)
				m_openList.pop();
			break;
		}

		m_openList.pop();
		m_closedList[ConvertTo1D(currentPos)] = true;

		// Check the neighbors of the current node
		for (unsigned int i = 0; i < m_nrOfDirections; ++i)
		{
			const auto neighborPos = currentPos + m_directions[i];
			const auto neighborIndex = ConvertTo1D(neighborPos);

			//cout << "\t#" << i << ": Check this: " << neighborPos.x << ", " << neighborPos.y << ":\t";
			if (!isValid(neighborPos) || 
				isBlocked(neighborPos.y, neighborPos.x) || 
				m_closedList[neighborIndex] == true)
			{
				//cout << "This position is not valid. Going to next neighbour." << endl;
				continue;
			}

			gNew = m_cameFromList[ConvertTo1D(currentPos)].g + 1;
			hNew = m_heuristic(neighborPos, m_targetPos, m_weight);
			fNew = gNew + hNew;

			if (m_cameFromList[neighborIndex].f == 0 || fNew < m_cameFromList[neighborIndex].f)
			{
				//cout << "Adding to Open List: " << neighborPos.x << ", " << neighborPos.y;
				//cout << ". [ f : " << fNew << ", g : " << gNew << ", h : " << hNew << "]" << endl;
				m_openList.push(Grid(neighborPos, fNew));
				m_cameFromList[neighborIndex] = { neighborPos, currentPos, fNew, gNew, hNew };
			}
			else
			{
				//cout << "Not adding this" << endl;
			}
		}
		//system("pause");
	}

	return BuildPath();
}

/**
 @brief Build a path
 */
std::vector<glm::i32vec2> CFloor2D::BuildPath() const
{
	std::vector<glm::i32vec2> path;
	auto currentPos = m_targetPos;
	auto currentIndex = ConvertTo1D(currentPos);

	while (!(m_cameFromList[currentIndex].parent == currentPos))
	{
		path.push_back(currentPos);
		currentPos = m_cameFromList[currentIndex].parent;
		currentIndex = ConvertTo1D(currentPos);
	}

	// If the path has only 1 entry, then it is the the target position
	if (path.size() == 1)
	{
		if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 1)
			path.clear();
	}
	else
		std::reverse(path.begin(), path.end());

	return path;
}

/**
 @brief Toggle the checks for diagonal movements
 */
void CFloor2D::SetDiagonalMovement(const bool bEnable)
{
	m_nrOfDirections = (bEnable) ? 8 : 4;
}

/**
 @brief Print out the details about this class instance in the console
 */
void CFloor2D::PrintSelf(void) const
{
	cout << endl << "AStar::PrintSelf()" << endl;

	for (unsigned uiLevel = 0; uiLevel < uiNumRooms; uiLevel++)
	{
		cout << "Level: " << uiLevel << endl;
		for (unsigned uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			for (unsigned uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
			{
				cout.fill('0');
				cout.width(3);
				cout << arrMapInfo[uiLevel][uiRow][uiCol].value;
				if (uiCol != cSettings->NUM_TILES_XAXIS - 1)
					cout << ", ";
				else
					cout << endl;
			}
		}
	}

	cout << "m_openList: " << m_openList.size() << endl;
	cout << "m_cameFromList: " << m_cameFromList.size() << endl;
	cout << "m_closedList: " << m_closedList.size() << endl;

	cout << "===== AStar::PrintSelf() =====" << endl;
}

void CFloor2D::GeneratePreset(int uiLevel)
{
	if (uiLevel > arrMapInfo.size() - 1) {
		arrMapInfo.push_back(new Grid * [CSettings::GetInstance()->NUM_TILES_YAXIS]);
	}
	int random;
	switch (uiLevel)
	{
	case 0:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				//Power up testing
				else if (uiRow == 5 && uiCol == 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 11;
				}
				else if (uiRow == 6 && uiCol == 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 12;
				}
				else if (uiRow == 6 && uiCol == 6)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 13;
				}
				else if (uiRow == 7 && uiCol == 6)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 14;
				}
				else if (uiRow == 7 && uiCol == 7)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 15;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 3;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}

			}
		}
		break;
	}
	case 1:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS - 5 && uiCol == cSettings->NUM_TILES_XAXIS - 5 || uiRow == 5 && uiCol == 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 2:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS - 5 && uiCol == cSettings->NUM_TILES_XAXIS - 5 || uiRow == 5 && uiCol == 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS - 5 && uiCol == 5 || uiRow == 5 && uiCol == cSettings->NUM_TILES_XAXIS - 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1005;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 3:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1005;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS/2 - 5 && uiCol == cSettings->NUM_TILES_XAXIS/2 - 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	case 4:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 5:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 6:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
				else if (uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2 || uiRow == cSettings->NUM_TILES_YAXIS / 2 + 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 7:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow <= 11 && uiCol == 2 || uiRow <= 11 && uiCol == 5 || uiRow >= cSettings->NUM_TILES_YAXIS - 11 && uiCol == cSettings->NUM_TILES_XAXIS - 3 || uiRow >= cSettings->NUM_TILES_YAXIS - 11 && uiCol == cSettings->NUM_TILES_XAXIS - 6)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS - 3 && uiCol == cSettings->NUM_TILES_XAXIS - 4 || uiRow == 2 && uiCol == 3)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 8:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow == cSettings->NUM_TILES_YAXIS - 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == 1 && uiCol == cSettings->NUM_TILES_XAXIS / 2) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == 1) || (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS - 2))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 100;
				}
				else if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow <= 11 && uiCol == 15 || uiRow == 11 && uiCol <= 12 || uiRow >= cSettings->NUM_TILES_YAXIS - 11 && uiCol == cSettings->NUM_TILES_XAXIS - 15 || uiRow == cSettings->NUM_TILES_YAXIS - 11 && uiCol >= cSettings->NUM_TILES_XAXIS - 12)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS - 3 && uiCol == cSettings->NUM_TILES_XAXIS - 3 || uiRow == 2 && uiCol == 2 )
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1001;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 9:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow >= cSettings->NUM_TILES_YAXIS / 2 - 5 && uiRow <= cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol == cSettings->NUM_TILES_XAXIS / 2 + 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 - 5 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5 || uiRow == cSettings->NUM_TILES_YAXIS / 2 + 5 && uiCol <= cSettings->NUM_TILES_XAXIS / 2 + 5 && uiCol >= cSettings->NUM_TILES_XAXIS / 2 - 5)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 102;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1102;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	case 10:
	{
		for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			// Load a particular CSV value into the arrMapInfo
			for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].roomid = -1;
				if ((uiRow < 2) || (uiCol < 2) || (uiCol > cSettings->NUM_TILES_XAXIS - 3) || (uiRow > cSettings->NUM_TILES_YAXIS - 3))
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 101;
				}
				else if (uiRow == cSettings->NUM_TILES_YAXIS / 2 && uiCol == cSettings->NUM_TILES_XAXIS / 2)
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 1102;
				}
				else
				{
					arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
				}
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}
	}
}

void CFloor2D::LoadEnemies()
{
	// Create and initialise the CEnemy2D
	for (int i = 0; i < CScene2D::GetInstance()->enemyVector.size(); i++)
	{

		delete CScene2D::GetInstance()->enemyVector[i];
		CScene2D::GetInstance()->enemyVector[i] = NULL;
	}
	CScene2D::GetInstance()->enemyVector.clear();
	while (true)
	{
		//CEnemy2D* cEnemy2D = new CEnemy2D();
		//SpaceFly* cEnemy = new SpaceFly();
		//cEnemy->SetShader("2DColorShader");
		CEnemy2D* cEnemy2D = new CSpaceGoop();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	while (true)
	{
		CEnemy2D* cEnemy2D = new CSpaceFly();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	while (true)
	{
		CEnemy2D* cEnemy2D = new CSpaceTurret();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	while (true)
	{
		CEnemy2D* cEnemy2D = new CSpaceSkeleton();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	while (true)
	{
		CEnemy2D* cEnemy2D = new CBoss2D();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	while (true)
	{
		CEnemy2D* cEnemy2D = new CSpaceCannon();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	while (true)
	{
		CEnemy2D* cEnemy2D = new CContagionBoss();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("2DColorShader");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			CScene2D::GetInstance()->enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

}

/**
 @brief Check if a position is valid
 */
bool CFloor2D::isValid(const glm::i32vec2& pos) const {
	//return (pos.x >= 0) && (pos.x < m_dimensions.x) &&
	//	(pos.y >= 0) && (pos.y < m_dimensions.y);
	return (pos.x >= 0) && (pos.x < cSettings->NUM_TILES_XAXIS) &&
		(pos.y >= 0) && (pos.y < cSettings->NUM_TILES_YAXIS);
}

/**
 @brief Check if a grid is blocked
 */
bool CFloor2D::isBlocked(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	if (bInvert == true)
	{
		if ((arrMapInfo[uiCurRoom][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value >= 100) &&
			(arrMapInfo[uiCurRoom][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value < 200))
			return true;
		else
			return false;
	}
	else
	{
		if ((arrMapInfo[uiCurRoom][uiRow][uiCol].value >= 100) &&
			(arrMapInfo[uiCurRoom][uiRow][uiCol].value < 200))
			return true;
		else
			return false;
	}
}

/**
 @brief Returns a 1D index based on a 2D coordinate using row-major layout
 */
int CFloor2D::ConvertTo1D(const glm::i32vec2& pos) const
{
	//return (pos.y * m_dimensions.x) + pos.x;
	return (pos.y * cSettings->NUM_TILES_XAXIS) + pos.x;
}

/**
 @brief Delete AStar lists
 */
bool CFloor2D::DeleteAStarLists(void)
{
	// Delete m_openList
	while (m_openList.size() != 0)
		m_openList.pop();
	// Delete m_cameFromList
	m_cameFromList.clear();
	// Delete m_closedList
	m_closedList.clear();

	return true;
}


/**
 @brief Reset AStar lists
 */
bool CFloor2D::ResetAStarLists(void)
{
	// Delete m_openList
	while (m_openList.size() != 0)
		m_openList.pop();
	// Reset m_cameFromList
	for (int i = 0; i < m_cameFromList.size(); i++)
	{
		m_cameFromList[i].pos = glm::i32vec2(0,0);
		m_cameFromList[i].parent = glm::i32vec2(0, 0);
		m_cameFromList[i].f = 0;
		m_cameFromList[i].g = 0;
		m_cameFromList[i].h = 0;
	}
	// Reset m_closedList
	for (int i = 0; i < m_closedList.size(); i++)
	{
		m_closedList[i] = false;
	}

	return true;
}


/**
 @brief manhattan calculation method for calculation of h
 */
unsigned int heuristic::manhattan(const glm::i32vec2& v1, const glm::i32vec2& v2, int weight)
{
	glm::i32vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * (delta.x + delta.y));
}

/**
 @brief euclidean calculation method for calculation of h
 */
unsigned int heuristic::euclidean(const glm::i32vec2& v1, const glm::i32vec2& v2, int weight)
{
	glm::i32vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * sqrt((delta.x * delta.x) + (delta.y * delta.y)));
}
/**
 CImageLoader
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "ImageLoader.h"

#include <iostream>
using namespace std;

// Include GLEW
#include <GL/glew.h>

// STB Image
#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include <includes/stb_image.h>
#include <fstream>
#include "filesystem.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CImageLoader::CImageLoader(void)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CImageLoader::~CImageLoader(void)
{
}

/**
@brief Init Initialise this instance
*/ 
bool CImageLoader::Init(void)
{
	return false;
}

/**
@brief Load an image and return as unsigned char*
*/
unsigned char * CImageLoader::Load(const char* filename, int& width, int& height, int& nrChannels, const bool bInvert)
{
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(bInvert);

	// load image, create texture and generate mipmaps
	unsigned char *data = stbi_load(FileSystem::getPath(filename).c_str(),
									&width, &height, &nrChannels, 0);

	// Return the data but you must remember to free up the memory of the file data read in
	return data;
}

unsigned char* CImageLoader::LoadImg(const char* file_path, int& width, int& height) {
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open()) {
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return 0;
	}

	GLubyte		header[18];									// first 6 useful header bytes
	GLuint		bytesPerPixel;								    // number of bytes per pixel in TGA gile
	GLuint		imageSize;									    // for setting memory
	GLubyte* data;

	fileStream.read((char*)header, 18);
	width = header[12] + header[13] * 256;
	height = header[14] + header[15] * 256;

	if (width <= 0 ||								// is width <= 0
		height <= 0 ||								// is height <=0
		(header[16] != 24 && header[16] != 32))		// is TGA 24 or 32 Bit
	{
		fileStream.close();							// close file on failure
		std::cout << "File header error.\n";
		return 0;
	}

	bytesPerPixel = header[16] / 8;						//divide by 8 to get bytes per pixel
	imageSize = width * height * bytesPerPixel;	// calculate memory required for TGA data

	data = new GLubyte[imageSize];
	fileStream.seekg(18, std::ios::beg);
	fileStream.read((char*)data, imageSize);
	fileStream.close();

	return data;
}

unsigned CImageLoader::LoadTextureGetID(const char* filename,  const bool bInvert)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	int nrChannels = 0;

	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(bInvert);

	// load image, create texture and generate mipmaps
	unsigned char* data = stbi_load(FileSystem::getPath(filename).c_str(),
		&image_width, &image_height, &nrChannels, 0);

	if (data == NULL)
		return 0;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

				  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	return image_texture;
}


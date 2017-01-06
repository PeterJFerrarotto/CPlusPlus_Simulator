#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef _WINDOWS
#include<GL/glew.h>
#endif
#include "rapidxml.hpp"
#include <string>
#include <vector>

class ShaderProgram;

enum TEXTURE_TYPE { IMAGE, EVEN_SPRITESHEET, UNEVEN_SPRITESHEET, TEXTURE_TYPE_COUNT };

class Texture
{
protected:
	//For all texture types:
	GLuint textureID;
	TEXTURE_TYPE textureType;
	unsigned textureLayer;
	std::vector<GLfloat> textureCoordinates;
	std::vector<GLfloat> objectVertices;
	//Done

public:
	Texture();
	~Texture();
	//Constructor for image texture type:
	Texture(GLuint textureID, unsigned textureLayer = 0);
	//Done

	
	std::vector<GLfloat>& getTextureCoordinates();
	std::vector<GLfloat>& getObjectCoordinates();
	GLuint getTextureID();


	void setTextureLayer(unsigned textureLayer);
	//Done

	virtual void deepCopy(Texture* toCopy);
};

#endif
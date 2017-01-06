#include "Texture.h"
#include "ShaderProgram.h"
using namespace std;
using namespace rapidxml;


Texture::Texture(){
	this->textureID = 0;
	textureType = TEXTURE_TYPE_COUNT;
}

Texture::Texture(GLuint textureID, unsigned textureLayer){
	this->textureID = textureID;
	textureType = IMAGE;
	this->textureLayer = textureLayer;
}

vector<GLfloat>& Texture::getTextureCoordinates(){
	textureCoordinates = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	
	return textureCoordinates;
}

GLuint Texture::getTextureID(){
	return textureID;
}

void Texture::setTextureLayer(unsigned textureLayer){
	this->textureLayer = textureLayer;
}

Texture::~Texture()
{
}

void Texture::deepCopy(Texture* toCopy){
	this->textureID = toCopy->textureID;
	this->textureLayer = toCopy->textureLayer;
	this->textureType = toCopy->textureType;
}
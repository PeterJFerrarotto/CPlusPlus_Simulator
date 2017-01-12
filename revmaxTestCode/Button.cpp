#include "Button.h"
#include <vector>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include "Texture.h"
#include "ShaderProgram.h"

Button::Button()
{
}


Button::~Button()
{
}

bool Button::getIsClicked(float x, float y){
	return (x >= position.x - size.x / 2 && x <= position.x + size.x / 2) && (y >= position.y - size.y / 2 && y <= position.y + size.y / 2);
}

void Button::draw(ShaderProgram* program){
	std::vector<GLfloat> objectVertices;
	std::vector<GLfloat> textureCoordinates;
	objectVertices = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
	textureCoordinates = texture->getTextureCoordinates();
	Matrix modelMatrix;
	modelMatrix.Translate(position.x, position.y, 0);
	modelMatrix.Scale(size.x, size.y, 0);

	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	program->setModelMatrix(modelMatrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, objectVertices.data());

	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoordinates.data());

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}
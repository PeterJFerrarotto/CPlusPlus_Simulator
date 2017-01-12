#ifndef _BUTTON_H
#define _BUTTON_H
#include "Vector3.h"
#include "Matrix.h"
#include <windows.h>
#include <string>
#include "enumHelper.h"

class Texture;
class ShaderProgram;
class Button
{
protected:
	Vector3 position, size, bounding;
	Texture* texture;
	std::string buttonText;
public:
	Button();
	~Button();

	void setTexture(Texture* texture){ this->texture = texture; }

	void setPosition(float x, float y, float z = 0){ position.x = x; position.y = y; position.z = z; }
	Vector3 getPosition(){ return position; }

	void setSize(float x, float y, float z = 0){ size.x = x; size.y = y; size.z = z; }
	Vector3 getSize(){ return size; }

	void setBounding(float x, float y, float z = 0){ bounding.x = x; bounding.y = y; bounding.z = z; }

	void setText(const std::string& buttonText){ this->buttonText = buttonText; }

	void draw(ShaderProgram* shaderProgram);

	bool getIsClicked(float x, float y);
};

#endif
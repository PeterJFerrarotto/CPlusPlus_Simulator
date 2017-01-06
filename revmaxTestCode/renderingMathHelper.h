#ifndef _RENDERING_MATH_HELPER_H
#define _RENDERING_MATH_HELPER_H

#include <unordered_map>
#include <SDL_image.h>

inline float motion(float pos1, float pos2, float time1, float time2){
	float diff = pos2 - pos1;
	return (float)(pos1 + (diff * pow((time1 / time2), 5)));
}

inline GLuint loadTexture(const char* imagePath){
	static std::unordered_map<std::string, GLuint> loadedTextures;
	if (loadedTextures.find(imagePath) == loadedTextures.end()){
		SDL_Surface *surface = IMG_Load(imagePath);
		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		GLenum externalFormat = GL_RGBA;
		if (surface->format->BytesPerPixel == 3) {
			externalFormat = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		char* x = new char[360];
		strcpy(x, imagePath);
		loadedTextures[x] = textureID;
		return textureID;
	}
	return loadedTextures.at(imagePath);
}

#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(WIN82) || defined(_WIN82) || defined(_WINDOWS)
#include <GL/glew.h>
#include <GL/GLU.h>
#endif
#include "Vehicle.h"
#include "RequestManager.h"
#include "RideRequest.h"
#include "mathHelper.h"
#include "Simulator.h"
#include "ShaderProgram.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Button.h"
#include "Matrix.h"

SDL_Window* displayWindow;

void drawText(ShaderProgram* program, float posX, float posY, float sizeX, float sizeY, const std::string& text, Texture* textSheet){
	std::vector<GLfloat> vertexData;
	std::vector<GLfloat> texCoordData;
	float texture_size = 1.0 / 16.0;
	float size = 0.5;
	float spacing = 0.2;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y,
			texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
		});
	}
	Matrix modelMatrix;
	modelMatrix.Translate(posX, posY, 0);
	modelMatrix.Scale(sizeX, sizeY, 0);
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	program->setModelMatrix(modelMatrix);


	glBindTexture(GL_TEXTURE_2D, textSheet->getTextureID());
	glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 2);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

static const std::string pressEnterToFinishEditing = "Press Enter to finish editing the name.";

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	float lastFrameTicks = 0.0f;
	glViewport(0, 0, 720, 800);
	float winX = 720;
	float winY = 800;

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	//ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::cout << "Begin!" << std::endl;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-1.78, 1.78, -2.0f, 2.0f, -1.0f, 1.0f);
	SDL_Event event;
	Texture* increaseTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/increase_texture.png"), 1);
	Texture* decreaseTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/decrease_texture.png"), 1);
	Texture* useCustomTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/use_custom.png"), 1);
	Texture* useFilesTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/use_files.png"), 1);
	Texture* doneTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/done_texture.png"), 1);
	Texture* textSheet = new Texture(loadTexture(RESOURCE_FOLDER"Assets/text_format.png"), 1);

	Button useCustom;
		useCustom.setTexture(useCustomTexture);
		useCustom.setSize(2, 1, 0);
		useCustom.setPosition(0, 0.5);

	Button useFiles;
		useFiles.setTexture(useFilesTexture);
		useFiles.setSize(2, 1, 0);
		useFiles.setPosition(0, -0.7);


	Button doneButton;
		doneButton.setTexture(doneTexture);
		doneButton.setSize(4, 2);
		doneButton.setPosition(-1, -18);

	Button increaseTimesToRun, decreaseTimesToRun;
		increaseTimesToRun.setTexture(increaseTexture);
		increaseTimesToRun.setSize(2, 2, 0);
		increaseTimesToRun.setPosition(-15, 16, 0);

		decreaseTimesToRun.setTexture(decreaseTexture);
		decreaseTimesToRun.setSize(2, 2, 0);
		decreaseTimesToRun.setPosition(-15, 14, 0);

	Button increaseTripWeight, decreaseTripWeight;
		increaseTripWeight.setTexture(increaseTexture);
		increaseTripWeight.setSize(2, 2, 0);
		increaseTripWeight.setPosition(-15, 11, 0);

		decreaseTripWeight.setTexture(decreaseTexture);
		decreaseTripWeight.setSize(2, 2, 0);
		decreaseTripWeight.setPosition(-15, 9, 0);

	Button increaseRadiusMin, decreaseRadiusMin;
		increaseRadiusMin.setTexture(increaseTexture);
		increaseRadiusMin.setSize(2, 2, 0);
		increaseRadiusMin.setPosition(-15, 6, 0);

		decreaseRadiusMin.setTexture(decreaseTexture);
		decreaseRadiusMin.setSize(2, 2, 0);
		decreaseRadiusMin.setPosition(-15, 4, 0);

	Button increaseRadiusStep, decreaseRadiusStep;
		increaseRadiusStep.setTexture(increaseTexture);
		increaseRadiusStep.setSize(2, 2, 0);
		increaseRadiusStep.setPosition(-15, 1, 0);

		decreaseRadiusStep.setTexture(decreaseTexture);
		decreaseRadiusStep.setSize(2, 2, 0);
		decreaseRadiusStep.setPosition(-15, -1, 0);

	Button increaseRadiusMax, decreaseRadiusMax;
		increaseRadiusMax.setTexture(increaseTexture);
		increaseRadiusMax.setSize(2, 2, 0);
		increaseRadiusMax.setPosition(-15, -4, 0);

		decreaseRadiusMax.setTexture(decreaseTexture);
		decreaseRadiusMax.setSize(2, 2, 0);
		decreaseRadiusMax.setPosition(-15, -6, 0);

	Button increaseTimeRadius, decreaseTimeRadius;
		increaseTimeRadius.setTexture(increaseTexture);
		increaseTimeRadius.setSize(2, 2, 0);
		increaseTimeRadius.setPosition(-15, -9, 0);

		decreaseTimeRadius.setTexture(decreaseTexture);
		decreaseTimeRadius.setSize(2, 2, 0);
		decreaseTimeRadius.setPosition(-15, -11, 0);

	Button increaseMinScore, decreaseMinScore;
		increaseMinScore.setTexture(increaseTexture);
		increaseMinScore.setSize(2, 2, 0);
		increaseMinScore.setPosition(-15, -14, 0);

		decreaseMinScore.setTexture(decreaseTexture);
		decreaseMinScore.setSize(2, 2, 0);
		decreaseMinScore.setPosition(-15, -16, 0);

	Button increaseMaxRequests, decreaseMaxRequests;
		increaseMaxRequests.setTexture(increaseTexture);
		increaseMaxRequests.setSize(2, 2, 0);
		increaseMaxRequests.setPosition(2, 16, 0);

		decreaseMaxRequests.setTexture(decreaseTexture);
		decreaseMaxRequests.setSize(2, 2, 0);
		decreaseMaxRequests.setPosition(2, 14, 0);

	Button increaseFleetSize, decreaseFleetSize;
		increaseFleetSize.setTexture(increaseTexture);
		increaseFleetSize.setSize(2, 2, 0);
		increaseFleetSize.setPosition(2, 11, 0);

		decreaseFleetSize.setTexture(decreaseTexture);
		decreaseFleetSize.setSize(2, 2, 0);
		decreaseFleetSize.setPosition(2, 9, 0);

	Button increaseVenueCount, decreaseVenueCount;
		increaseVenueCount.setTexture(increaseTexture);
		increaseVenueCount.setSize(2, 2, 0);
		increaseVenueCount.setPosition(2, 6, 0);

		decreaseVenueCount.setTexture(decreaseTexture);
		decreaseVenueCount.setSize(2, 2, 0);
		decreaseVenueCount.setPosition(2, 4, 0);

	Button increaseRequestCount, decreaseRequestCount;
		increaseRequestCount.setTexture(increaseTexture);
		increaseRequestCount.setSize(2, 2, 0);
		increaseRequestCount.setPosition(2, 1, 0);

		decreaseRequestCount.setTexture(decreaseTexture);
		decreaseRequestCount.setSize(2, 2, 0);
		decreaseRequestCount.setPosition(2, -1, 0);

	Button increaseSectionSize, decreaseSectionSize;
		increaseSectionSize.setTexture(increaseTexture);
		increaseSectionSize.setSize(2, 2, 0);
		increaseSectionSize.setPosition(2, -4, 0);

		decreaseSectionSize.setTexture(decreaseTexture);
		decreaseSectionSize.setSize(2, 2, 0);
		decreaseSectionSize.setPosition(2, -6, 0);

	Button increaseMaxLat, decreaseMaxLat;
		increaseMaxLat.setTexture(increaseTexture);
		increaseMaxLat.setSize(2, 2, 0);
		increaseMaxLat.setPosition(2, -9, 0);

		decreaseMaxLat.setTexture(decreaseTexture);
		decreaseMaxLat.setSize(2, 2, 0);
		decreaseMaxLat.setPosition(2, -11, 0);

	Button increaseMaxLong, decreaseMaxLong;
		increaseMaxLong.setTexture(increaseTexture);
		increaseMaxLong.setSize(2, 2, 0);
		increaseMaxLong.setPosition(2, -14, 0);

		decreaseMaxLong.setTexture(decreaseTexture);
		decreaseMaxLong.setSize(2, 2, 0);
		decreaseMaxLong.setPosition(2, -16, 0);

	Matrix viewMatrix;
	program.setViewMatrix(viewMatrix);

	bool choiceMade = false;
	bool getCustomParams = false;
	float unitX = 0;
	float unitY = 0;
	while (!choiceMade){
		glClear(GL_COLOR_BUFFER_BIT);
		if (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
				SDL_Quit();
				return 0;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN){
				if (event.button.button == SDL_BUTTON_LEFT){
					unitX = (((float)event.button.x / winX) * (1.78 * 2)) - 1.78;
					unitY = (((winY - (float)event.button.y) / winY) * 4) - 2;
					if (useCustom.getIsClicked(unitX, unitY)){
						choiceMade = true;
						getCustomParams = true;
					}
					else if (useFiles.getIsClicked(unitX, unitY)){
						choiceMade = true;
					}
				}
			}
		}
		useCustom.draw(&program);
		useFiles.draw(&program);
		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);
		SDL_GL_SwapWindow(displayWindow);
	}

	Simulator tester(getCustomParams);
	if (getCustomParams){
		std::string customTestName = "Custom Test 1";
		unsigned customTimesToRun = 10;
		float customTripWeight = 20;
		int customRadiusMin = 5;
		int customRadiusStep = 5;
		int customRadiusMax = 2;
		int customTimeRadius = 5;
		float customMinimumScore = 5;
		unsigned customMaximumRideRequests = 30;
		unsigned customFleetSize = 10;
		unsigned customVenueCount = 10;
		unsigned customRideCount = 10;
		int customMaxLat = 4;
		int customMaxLong = 4;
		int customSectionSize = 5;
		bool paramsDialedIn = false;
		bool clicked = false;
		int clickCount = 0;
		int clickTarget = 200;
		int clickTargetInit = 200;
		int coeff = 1;
		viewMatrix.Scale(.1, .1, 1);

		while (!paramsDialedIn){
			glClear(GL_COLOR_BUFFER_BIT);
			std::stringstream tripW;
			tripW.precision(2);
			if (customTripWeight >= 10){
				tripW.precision(3);
			}
			tripW << customTripWeight;
			std::stringstream minScore;
			minScore.precision(2);
			if (customMinimumScore >= 10){
				minScore.precision(3);
			}
			minScore << customMinimumScore;
			while (SDL_PollEvent(&event)){
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
					SDL_Quit();
					return 0;
				}
				else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT){
					clicked = false;
				}
				else if ((event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) || clicked){
					unitX = (((float)event.button.x / winX) * (1.78 * 2)) - 1.78;
					unitY = (((winY - (float)event.button.y) / winY) * 4) - 2;
					unitX *= 10;
					unitY *= 10;
					if (!clicked){
						if (abs(unitX) <= customTestName.size() / 2 && unitY >= 17 && unitY <= 19){
							SDL_bool done = SDL_FALSE;

							SDL_StartTextInput();
							clicked = false;
							while (!done) {
								glClear(GL_COLOR_BUFFER_BIT);

								SDL_Event event;
								if (SDL_PollEvent(&event)) {
									switch (event.type) {
									case SDL_QUIT:
										/* Quit */
										done = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										/* Add new text onto the end of our text */
										//strcat(customTestName, event.text.text);
										customTestName += event.text.text;
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											done = SDL_TRUE;
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (customTestName.size() > 0){
												customTestName.erase(customTestName.end() - 1);
											}
										}
										break;
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName + '|', textSheet);
								drawText(&program, -3, 16, 0.5, 1, pressEnterToFinishEditing, textSheet);

								increaseTimesToRun.draw(&program);
								decreaseTimesToRun.draw(&program);
								drawText(&program, -13, 15, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								increaseTripWeight.draw(&program);
								decreaseTripWeight.draw(&program);
								drawText(&program, -13, 10, 1, 1, "Weight of Trip: " + tripW.str(), textSheet);

								increaseRadiusMin.draw(&program);
								decreaseRadiusMin.draw(&program);
								drawText(&program, -13, 5.5, 1, 1, "Minimum Search", textSheet);
								drawText(&program, -13, 4.5, 1, 1, "Radius: " + std::to_string(customRadiusMin), textSheet);

								increaseRadiusStep.draw(&program);
								decreaseRadiusStep.draw(&program);
								drawText(&program, -13, 0, 1, 1, "Radius Step: " + std::to_string(customRadiusStep), textSheet);

								increaseRadiusMax.draw(&program);
								decreaseRadiusMax.draw(&program);
								int valToDraw = customRadiusMin + customRadiusStep * customRadiusMax;
								drawText(&program, -13, -4.5, 1, 1, "Maximum Search", textSheet);
								drawText(&program, -13, -5.5, 1, 1, "Radius: " + std::to_string(valToDraw), textSheet);

								increaseTimeRadius.draw(&program);
								decreaseTimeRadius.draw(&program);
								drawText(&program, -13, -10, 1, 1, "Time Radius: " + std::to_string(customTimeRadius), textSheet);

								increaseMinScore.draw(&program);
								decreaseMinScore.draw(&program);
								drawText(&program, -13, -14.5, 1, 1, "Minimum", textSheet);
								drawText(&program, -13, -15.5, 1, 1, "Score: " + minScore.str(), textSheet);

								increaseMaxRequests.draw(&program);
								decreaseMaxRequests.draw(&program);
								drawText(&program, 4, 15.5, 1, 1, "Destination Request", textSheet);
								drawText(&program, 4, 14.5, 1, 1, "Count Threshold: " + std::to_string(customMaximumRideRequests), textSheet);

								increaseFleetSize.draw(&program);
								decreaseFleetSize.draw(&program);
								drawText(&program, 4, 10, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								increaseVenueCount.draw(&program);
								decreaseVenueCount.draw(&program);
								drawText(&program, 4, 5, 1, 1, "Venue Count: " + std::to_string(customVenueCount), textSheet);

								increaseRequestCount.draw(&program);
								decreaseRequestCount.draw(&program);
								drawText(&program, 4, 0, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								increaseSectionSize.draw(&program);
								decreaseSectionSize.draw(&program);
								drawText(&program, 4, -5, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								increaseMaxLat.draw(&program);
								decreaseMaxLat.draw(&program);
								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								increaseMaxLong.draw(&program);
								decreaseMaxLong.draw(&program);
								drawText(&program, 4, -14.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -15.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);

								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
						}
						if (increaseTimesToRun.getIsClicked(unitX, unitY)){
							customTimesToRun += 5;
						}
						if (decreaseTimesToRun.getIsClicked(unitX, unitY) && customTimesToRun > 5){
							customTimesToRun -= 5;
						}

						if (increaseTripWeight.getIsClicked(unitX, unitY)){
							customTripWeight += 0.5;
						}
						if (decreaseTripWeight.getIsClicked(unitX, unitY) && customTripWeight > 0.5){
							customTripWeight -= 0.5;
						}

						if (increaseRadiusMin.getIsClicked(unitX, unitY)){
							customRadiusMin++;
						}
						if (decreaseRadiusMin.getIsClicked(unitX, unitY) && customRadiusMin > 1){
							customRadiusMin--;
						}

						if (increaseRadiusStep.getIsClicked(unitX, unitY)){
							customRadiusStep++;
						}
						if (decreaseRadiusStep.getIsClicked(unitX, unitY) && customRadiusStep > 1){
							customRadiusStep--;
						}

						if (increaseRadiusMax.getIsClicked(unitX, unitY)){
							customRadiusMax++;
						}
						if (decreaseRadiusMax.getIsClicked(unitX, unitY) && customRadiusStep > 1){
							customRadiusMax--;
						}

						if (increaseTimeRadius.getIsClicked(unitX, unitY)){
							customTimeRadius++;
						}
						if (decreaseTimeRadius.getIsClicked(unitX, unitY) && customTimeRadius > 1){
							customTimeRadius--;
						}

						if (increaseFleetSize.getIsClicked(unitX, unitY)){
							customFleetSize++;
						}
						if (decreaseFleetSize.getIsClicked(unitX, unitY) && customFleetSize > 1){
							customFleetSize--;
						}

						if (increaseMinScore.getIsClicked(unitX, unitY)){
							customMinimumScore += 0.5;
						}
						if (decreaseMinScore.getIsClicked(unitX, unitY) && customMinimumScore > 0){
							customMinimumScore -= 0.5;
						}

						if (increaseMaxRequests.getIsClicked(unitX, unitY)){
							customMaximumRideRequests++;
						}
						if (decreaseMaxRequests.getIsClicked(unitX, unitY) && customMaximumRideRequests > 1){
							customMaximumRideRequests--;
						}

						if (increaseVenueCount.getIsClicked(unitX, unitY)){
							customVenueCount++;
						}
						if (decreaseVenueCount.getIsClicked(unitX, unitY) && customVenueCount > 0){
							customVenueCount--;
						}

						if (increaseRequestCount.getIsClicked(unitX, unitY)){
							customRideCount++;
						}
						if (decreaseRequestCount.getIsClicked(unitX, unitY) && customRideCount > 1){
							customRideCount--;
						}

						if (increaseSectionSize.getIsClicked(unitX, unitY)){
							customSectionSize++;
						}
						if (decreaseSectionSize.getIsClicked(unitX, unitY) > 1){
							customSectionSize--;
						}

						if (increaseMaxLat.getIsClicked(unitX, unitY)){
							customMaxLat++;
						}
						if (decreaseMaxLat.getIsClicked(unitX, unitY) && customMaxLat > 1){
							customMaxLat--;
						}

						if (increaseMaxLong.getIsClicked(unitX, unitY)){
							customMaxLong++;
						}
						if (decreaseMaxLong.getIsClicked(unitX, unitY) && customMaxLong > 1){
							customMaxLong--;
						}

						if (doneButton.getIsClicked(unitX, unitY)){
							paramsDialedIn = true;
						}
					}
					clicked = true;
				}
			}
			if (clicked){
				clickCount++;
				if (clickCount % clickTarget == 0){
					if (clickCount < clickTargetInit * 5){
						clickTarget = clickTargetInit;
					}
					else if (clickCount >= clickTargetInit * 5 && clickCount < clickTargetInit * 10){
						clickTarget = 100;
					}
					else if (clickCount >= clickTargetInit * 10 && clickCount < clickTargetInit * 20){
						clickTarget = 50;
					}
					else if (clickCount >= clickTargetInit * 20){
						clickTarget = 25;
					}

					if (increaseTimesToRun.getIsClicked(unitX, unitY)){
						customTimesToRun += 5 * coeff;
					}
					if (decreaseTimesToRun.getIsClicked(unitX, unitY) && customTimesToRun > 5){
						customTimesToRun -= 5 * coeff;
					}

					if (increaseTripWeight.getIsClicked(unitX, unitY)){
						customTripWeight += 0.5 * coeff;
					}
					if (decreaseTripWeight.getIsClicked(unitX, unitY) && customTripWeight > 0.5){
						customTripWeight -= 0.5 * coeff;
					}

					if (increaseRadiusMin.getIsClicked(unitX, unitY)){
						customRadiusMin += coeff;
					}
					if (decreaseRadiusMin.getIsClicked(unitX, unitY) && customRadiusMin > 1){
						customRadiusMin -= coeff;
					}

					if (increaseRadiusStep.getIsClicked(unitX, unitY)){
						customRadiusStep += coeff;
					}
					if (decreaseRadiusStep.getIsClicked(unitX, unitY) && customRadiusStep > 1){
						customRadiusStep -= coeff;
					}

					if (increaseRadiusMax.getIsClicked(unitX, unitY)){
						customRadiusMax += coeff;
					}
					if (decreaseRadiusMax.getIsClicked(unitX, unitY) && customRadiusStep > 1){
						customRadiusMax -= coeff;
					}

					if (increaseTimeRadius.getIsClicked(unitX, unitY)){
						customTimeRadius += coeff;
					}
					if (decreaseTimeRadius.getIsClicked(unitX, unitY) && customTimeRadius > 1){
						customTimeRadius -= coeff;
					}

					if (increaseMinScore.getIsClicked(unitX, unitY)){
						customMinimumScore += 0.5 * coeff;
					}
					if (decreaseMinScore.getIsClicked(unitX, unitY) && customMinimumScore > 0){
						customMinimumScore -= 0.5 * coeff;
					}

					if (increaseMaxRequests.getIsClicked(unitX, unitY)){
						customMaximumRideRequests += coeff;
					}
					if (decreaseMaxRequests.getIsClicked(unitX, unitY) && customMaximumRideRequests > 1){
						customMaximumRideRequests -= coeff;
					}

					if (increaseFleetSize.getIsClicked(unitX, unitY)){
						customFleetSize += coeff;
					}
					if (decreaseFleetSize.getIsClicked(unitX, unitY) && customFleetSize > 1){
						customFleetSize -= coeff;
					}

					if (increaseVenueCount.getIsClicked(unitX, unitY)){
						customVenueCount += coeff;
					}
					if (decreaseVenueCount.getIsClicked(unitX, unitY) && customVenueCount > 0){
						customVenueCount -= coeff;
					}

					if (increaseRequestCount.getIsClicked(unitX, unitY)){
						customRideCount += coeff;
					}
					if (decreaseRequestCount.getIsClicked(unitX, unitY) && customRideCount > 1){
						customRideCount -= coeff;
					}

					if (increaseSectionSize.getIsClicked(unitX, unitY)){
						customSectionSize += coeff;
					}
					if (decreaseSectionSize.getIsClicked(unitX, unitY) > 1){
						customSectionSize -= coeff;
					}

					if (increaseMaxLat.getIsClicked(unitX, unitY)){
						customMaxLat += coeff;
					}
					if (decreaseMaxLat.getIsClicked(unitX, unitY) && customMaxLat > 1){
						customMaxLat -= coeff;
					}

					if (increaseMaxLong.getIsClicked(unitX, unitY)){
						customMaxLong += coeff;
					}
					if (decreaseMaxLong.getIsClicked(unitX, unitY) && customMaxLong > 1){
						customMaxLong -= coeff;
					}

					if (doneButton.getIsClicked(unitX, unitY)){
						paramsDialedIn = true;
					}
				}
			}
			else{
				clickCount = 0;
			}
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			drawText(&program, -((float)customTestName.size() / 2.0), 18, 1, 1, customTestName, textSheet);

			increaseTimesToRun.draw(&program);
			decreaseTimesToRun.draw(&program);
			drawText(&program, -13, 15, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

			increaseTripWeight.draw(&program);
			decreaseTripWeight.draw(&program);
			drawText(&program, -13, 10, 1, 1, "Weight of Trip: " + tripW.str(), textSheet);

			increaseRadiusMin.draw(&program);
			decreaseRadiusMin.draw(&program);
			drawText(&program, -13, 5.5, 1, 1, "Minimum Search", textSheet);
			drawText(&program, -13, 4.5, 1, 1, "Radius: " + std::to_string(customRadiusMin), textSheet);

			increaseRadiusStep.draw(&program);
			decreaseRadiusStep.draw(&program);
			drawText(&program, -13, 0, 1, 1, "Radius Step: " + std::to_string(customRadiusStep), textSheet);

			increaseRadiusMax.draw(&program);
			decreaseRadiusMax.draw(&program);
			int valToDraw = customRadiusMin + customRadiusStep * customRadiusMax;
			drawText(&program, -13, -4.5, 1, 1, "Maximum Search", textSheet);
			drawText(&program, -13, -5.5, 1, 1, "Radius: " + std::to_string(valToDraw), textSheet);

			increaseTimeRadius.draw(&program);
			decreaseTimeRadius.draw(&program);
			drawText(&program, -13, -10, 1, 1, "Time Radius: " + std::to_string(customTimeRadius), textSheet);

			increaseMinScore.draw(&program);
			decreaseMinScore.draw(&program);
			drawText(&program, -13, -14.5, 1, 1, "Minimum", textSheet);
			drawText(&program, -13, -15.5, 1, 1, "Score: " + minScore.str(), textSheet);

			increaseMaxRequests.draw(&program);
			decreaseMaxRequests.draw(&program);
			drawText(&program, 4, 15.5, 1, 1, "Destination Request", textSheet);
			drawText(&program, 4, 14.5, 1, 1, "Count Threshold: " + std::to_string(customMaximumRideRequests), textSheet);

			increaseFleetSize.draw(&program);
			decreaseFleetSize.draw(&program);
			drawText(&program, 4, 10, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);
			
			increaseVenueCount.draw(&program);
			decreaseVenueCount.draw(&program);
			drawText(&program, 4, 5, 1, 1, "Venue Count: " + std::to_string(customVenueCount), textSheet);

			increaseRequestCount.draw(&program);
			decreaseRequestCount.draw(&program);
			drawText(&program, 4, 0, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

			increaseSectionSize.draw(&program);
			decreaseSectionSize.draw(&program);
			drawText(&program, 4, -5, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

			increaseMaxLat.draw(&program);
			decreaseMaxLat.draw(&program);
			drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
			drawText(&program, 4, -10.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);
			
			increaseMaxLong.draw(&program);
			decreaseMaxLong.draw(&program);
			drawText(&program, 4, -14.5, 1, 1, "Maximum", textSheet);
			drawText(&program, 4, -15.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);

			doneButton.draw(&program);
			program.setViewMatrix(viewMatrix);
			program.setProjectionMatrix(projectionMatrix);
			SDL_GL_SwapWindow(displayWindow);
		}
		tester.initializeSimulatorWithParams(customTestName, customTimesToRun, customTripWeight, customRadiusMin, customRadiusStep, (customRadiusMax * customRadiusStep) + customRadiusMin,
			customTimeRadius, customMinimumScore, customMaximumRideRequests, customFleetSize, customVenueCount, customRideCount, (customMaxLat * customSectionSize),
			(customMaxLong * customSectionSize), customSectionSize);
	}
	tester.runTests();
	std::vector<std::string> testNames = tester.getTestNames();
	bool escapePressed = false;
	for (std::string testName : testNames){
		//displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 720, SDL_WINDOW_OPENGL);
		//projectionMatrix.setOrthoProjection(-1.78, 1.78, -2.0f, 2.0f, -1.0f, 1.0f);
		int timesToRun = tester.getTimesToRun(testName);
		float timesRun = 0;
		bool done = false;
		while (!done) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
			}
			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;
			if (timesRun < timesToRun){
				timesRun += 0.008;
			}
			else{
				timesRun = timesToRun;
			}
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			program.setProjectionMatrix(projectionMatrix);
			tester.visualize(timesRun, state, event, &program, testName, displayWindow);
			SDL_SetWindowTitle(displayWindow, testName.c_str());
			SDL_GL_SwapWindow(displayWindow);
			//}
			if (/*timesRun >= timesToRun*/state[SDL_SCANCODE_ESCAPE] && !escapePressed){
				done = true;
			}
			escapePressed = state[SDL_SCANCODE_ESCAPE];
		}
	}
	tester.freeMemory();
	SDL_Quit();
	return 0;
}
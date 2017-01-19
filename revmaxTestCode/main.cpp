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
	Texture* useRangedTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/use_ranged.png"), 1);
	Texture* useCustomTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/use_custom.png"), 1);
	Texture* useFilesTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/use_files.png"), 1);
	Texture* doneTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/done_texture.png"), 1);
	Texture* addTestTexture = new Texture(loadTexture(RESOURCE_FOLDER"Assets/add_test.png"), 1);
	Texture* textSheet = new Texture(loadTexture(RESOURCE_FOLDER"Assets/text_format.png"), 1);

	std::vector<Button> modeSelectionButtons;
	std::vector<Button> nonRangedButtonsShared;
	std::vector<Button> nonRangedButtonsPrivate;
	std::vector<Button> rangedButtons;

	Button useRanged;
		useRanged.setTexture(useRangedTexture);
		useRanged.setSize(2, 1, 0);
		useRanged.setPosition(0, 1.2);
		modeSelectionButtons.push_back(useRanged);

	Button useCustom;
		useCustom.setTexture(useCustomTexture);
		useCustom.setSize(2, 1, 0);
		useCustom.setPosition(0, 0);
		modeSelectionButtons.push_back(useCustom);


	Button useFiles;
		useFiles.setTexture(useFilesTexture);
		useFiles.setSize(2, 1, 0);
		useFiles.setPosition(0, -1.2);
		modeSelectionButtons.push_back(useFiles);

	
	Button addTest;
		addTest.setTexture(addTestTexture);
		addTest.setSize(4, 2);
		addTest.setPosition(-5, -18);

	Button doneButton;
		doneButton.setTexture(doneTexture);
		doneButton.setSize(4, 2);
		doneButton.setPosition(0, -18);

	Button increaseTimesToRun, decreaseTimesToRun;
		increaseTimesToRun.setTexture(increaseTexture);
		increaseTimesToRun.setSize(2, 2, 0);
		increaseTimesToRun.setPosition(-15, 16, 0);
		nonRangedButtonsShared.push_back(increaseTimesToRun);

		decreaseTimesToRun.setTexture(decreaseTexture);
		decreaseTimesToRun.setSize(2, 2, 0);
		decreaseTimesToRun.setPosition(-15, 14, 0);
		nonRangedButtonsShared.push_back(decreaseTimesToRun);

	Button increaseTripWeight, decreaseTripWeight;
		increaseTripWeight.setTexture(increaseTexture);
		increaseTripWeight.setSize(2, 2, 0);
		increaseTripWeight.setPosition(-15, 11, 0);
		nonRangedButtonsPrivate.push_back(increaseTripWeight);

		decreaseTripWeight.setTexture(decreaseTexture);
		decreaseTripWeight.setSize(2, 2, 0);
		decreaseTripWeight.setPosition(-15, 9, 0);
		nonRangedButtonsPrivate.push_back(decreaseTripWeight);

	Button increaseRadiusMin, decreaseRadiusMin;
		increaseRadiusMin.setTexture(increaseTexture);
		increaseRadiusMin.setSize(2, 2, 0);
		increaseRadiusMin.setPosition(-15, 6, 0);
		nonRangedButtonsPrivate.push_back(increaseRadiusMin);

		decreaseRadiusMin.setTexture(decreaseTexture);
		decreaseRadiusMin.setSize(2, 2, 0);
		decreaseRadiusMin.setPosition(-15, 4, 0);
		nonRangedButtonsPrivate.push_back(decreaseRadiusMin);

	Button increaseRadiusStep, decreaseRadiusStep;
		increaseRadiusStep.setTexture(increaseTexture);
		increaseRadiusStep.setSize(2, 2, 0);
		increaseRadiusStep.setPosition(-15, 1, 0);
		nonRangedButtonsPrivate.push_back(increaseRadiusStep);

		decreaseRadiusStep.setTexture(decreaseTexture);
		decreaseRadiusStep.setSize(2, 2, 0);
		decreaseRadiusStep.setPosition(-15, -1, 0);
		nonRangedButtonsPrivate.push_back(decreaseRadiusStep);

	Button increaseRadiusMax, decreaseRadiusMax;
		increaseRadiusMax.setTexture(increaseTexture);
		increaseRadiusMax.setSize(2, 2, 0);
		increaseRadiusMax.setPosition(-15, -4, 0);
		nonRangedButtonsPrivate.push_back(increaseRadiusMax);

		decreaseRadiusMax.setTexture(decreaseTexture);
		decreaseRadiusMax.setSize(2, 2, 0);
		decreaseRadiusMax.setPosition(-15, -6, 0);
		nonRangedButtonsPrivate.push_back(decreaseRadiusMax);

	Button increaseTimeRadius, decreaseTimeRadius;
		increaseTimeRadius.setTexture(increaseTexture);
		increaseTimeRadius.setSize(2, 2, 0);
		increaseTimeRadius.setPosition(-15, -9, 0);
		nonRangedButtonsPrivate.push_back(increaseTimeRadius);

		decreaseTimeRadius.setTexture(decreaseTexture);
		decreaseTimeRadius.setSize(2, 2, 0);
		decreaseTimeRadius.setPosition(-15, -11, 0);
		nonRangedButtonsPrivate.push_back(decreaseTimeRadius);

	Button increaseMinScore, decreaseMinScore;
		increaseMinScore.setTexture(increaseTexture);
		increaseMinScore.setSize(2, 2, 0);
		increaseMinScore.setPosition(-15, -14, 0);
		nonRangedButtonsPrivate.push_back(increaseMinScore);

		decreaseMinScore.setTexture(decreaseTexture);
		decreaseMinScore.setSize(2, 2, 0);
		decreaseMinScore.setPosition(-15, -16, 0);
		nonRangedButtonsPrivate.push_back(decreaseMinScore);

	Button increaseMaxRequests, decreaseMaxRequests;
		increaseMaxRequests.setTexture(increaseTexture);
		increaseMaxRequests.setSize(2, 2, 0);
		increaseMaxRequests.setPosition(2, 16, 0);
		nonRangedButtonsPrivate.push_back(increaseMaxRequests);

		decreaseMaxRequests.setTexture(decreaseTexture);
		decreaseMaxRequests.setSize(2, 2, 0);
		decreaseMaxRequests.setPosition(2, 14, 0);
		nonRangedButtonsPrivate.push_back(decreaseMaxRequests);

	Button increaseFleetSize, decreaseFleetSize;
		increaseFleetSize.setTexture(increaseTexture);
		increaseFleetSize.setSize(2, 2, 0);
		increaseFleetSize.setPosition(2, 11, 0);
		nonRangedButtonsShared.push_back(increaseFleetSize);

		decreaseFleetSize.setTexture(decreaseTexture);
		decreaseFleetSize.setSize(2, 2, 0);
		decreaseFleetSize.setPosition(2, 9, 0);
		nonRangedButtonsShared.push_back(decreaseFleetSize);

	Button increaseRequestCount, decreaseRequestCount;
		increaseRequestCount.setTexture(increaseTexture);
		increaseRequestCount.setSize(2, 2, 0);
		increaseRequestCount.setPosition(2, 6, 0);
		nonRangedButtonsShared.push_back(increaseRequestCount);

		decreaseRequestCount.setTexture(decreaseTexture);
		decreaseRequestCount.setSize(2, 2, 0);
		decreaseRequestCount.setPosition(2, 4, 0);
		nonRangedButtonsShared.push_back(decreaseRequestCount);

	Button increaseSectionSize, decreaseSectionSize;
		increaseSectionSize.setTexture(increaseTexture);
		increaseSectionSize.setSize(2, 2, 0);
		increaseSectionSize.setPosition(2, 1, 0);
		nonRangedButtonsShared.push_back(increaseSectionSize);

		decreaseSectionSize.setTexture(decreaseTexture);
		decreaseSectionSize.setSize(2, 2, 0);
		decreaseSectionSize.setPosition(2, -1, 0);
		nonRangedButtonsShared.push_back(decreaseSectionSize);

	Button increaseMaxLat, decreaseMaxLat;
		increaseMaxLat.setTexture(increaseTexture);
		increaseMaxLat.setSize(2, 2, 0);
		increaseMaxLat.setPosition(2, -4, 0);
		nonRangedButtonsShared.push_back(increaseMaxLat);

		decreaseMaxLat.setTexture(decreaseTexture);
		decreaseMaxLat.setSize(2, 2, 0);
		decreaseMaxLat.setPosition(2, -6, 0);
		nonRangedButtonsShared.push_back(decreaseMaxLat);

	Button increaseMaxLong, decreaseMaxLong;
		increaseMaxLong.setTexture(increaseTexture);
		increaseMaxLong.setSize(2, 2, 0);
		increaseMaxLong.setPosition(2, -9, 0);
		nonRangedButtonsShared.push_back(increaseMaxLong);

		decreaseMaxLong.setTexture(decreaseTexture);
		decreaseMaxLong.setSize(2, 2, 0);
		decreaseMaxLong.setPosition(2, -11, 0);
		nonRangedButtonsShared.push_back(decreaseMaxLong);

	Matrix viewMatrix;
	program.setViewMatrix(viewMatrix);

	bool choiceMade = false;
	bool getCustomParams = false;
	bool getCustomRangedParams = false;
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
					else if (useRanged.getIsClicked(unitX, unitY)){
						choiceMade = true;
						getCustomRangedParams = true;
					}
				}
			}
		}
		useRanged.draw(&program);
		useCustom.draw(&program);
		useFiles.draw(&program);
		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);
		SDL_GL_SwapWindow(displayWindow);
	}

	Simulator tester(getCustomParams || getCustomRangedParams);
	if (getCustomParams){
		std::string customTestName = "Custom Test 1";
		std::string previousTestName = "";
		unsigned customTimesToRun = 10;
		float customTripWeight = 0.002;
		int customRadiusMin = 5;
		int customRadiusStep = 5;
		int customRadiusMax = 2;
		int customTimeRadius = 5;
		float customMinimumScore = 30;
		unsigned customMaximumRideRequests = 30;
		unsigned customFleetSize = 10;
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
		int timesTitleUsed = 0;
		int numberOfTests = 0;
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
			minScore.precision(4);
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

								for (Button button : nonRangedButtonsShared){
									button.draw(&program);
								}

								for (Button button : nonRangedButtonsPrivate){
									button.draw(&program);
								}

								drawText(&program, -13, 15, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, 11.5, 1, 1, "Weight of Trip:", textSheet);
								drawText(&program, -13, 10.5, 1, 1, tripW.str(), textSheet);

								drawText(&program, -13, 5.5, 1, 1, "Minimum Search", textSheet);
								drawText(&program, -13, 4.5, 1, 1, "Radius: " + std::to_string(customRadiusMin), textSheet);

								drawText(&program, -13, 0, 1, 1, "Radius Step: " + std::to_string(customRadiusStep), textSheet);

								int valToDraw = customRadiusMin + customRadiusStep * customRadiusMax;
								drawText(&program, -13, -4.5, 1, 1, "Maximum Search", textSheet);
								drawText(&program, -13, -5.5, 1, 1, "Radius: " + std::to_string(valToDraw), textSheet);

								drawText(&program, -13, -10, 1, 1, "Time Radius: " + std::to_string(customTimeRadius), textSheet);

								drawText(&program, -13, -14.5, 1, 1, "Minimum", textSheet);
								drawText(&program, -13, -15.5, 1, 1, "Score: " + minScore.str(), textSheet);

								drawText(&program, 4, 15.5, 1, 1, "Destination Request", textSheet);
								drawText(&program, 4, 14.5, 1, 1, "Count Threshold: " + std::to_string(customMaximumRideRequests), textSheet);

								drawText(&program, 4, 10, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, 4, 5, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);

								doneButton.draw(&program);

								addTest.draw(&program);
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
							customTripWeight += 0.001;
						}
						if (decreaseTripWeight.getIsClicked(unitX, unitY) && customTripWeight > 0.001){
							customTripWeight -= 0.001;
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
							customMinimumScore += 5;
						}
						if (decreaseMinScore.getIsClicked(unitX, unitY) && customMinimumScore > 0){
							customMinimumScore -= 5;
						}

						if (increaseMaxRequests.getIsClicked(unitX, unitY)){
							customMaximumRideRequests++;
						}
						if (decreaseMaxRequests.getIsClicked(unitX, unitY) && customMaximumRideRequests > 1){
							customMaximumRideRequests--;
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
						if (decreaseSectionSize.getIsClicked(unitX, unitY) && customSectionSize > 1){
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
							paramsDialedIn = numberOfTests >= 1;
						}

						if (addTest.getIsClicked(unitX, unitY) && !clicked){
							std::string testNameToUse = customTestName;
							if (customTestName == previousTestName){
								timesTitleUsed++;
								testNameToUse += "_" + std::to_string(timesTitleUsed);
							}
							else{
								timesTitleUsed = 0;
								previousTestName = customTestName;
							}
							tester.initializeSimulatorWithParams(testNameToUse, customTimesToRun, customTripWeight, customRadiusMin, customRadiusStep, (customRadiusMax * customRadiusStep) + customRadiusMin,
								customTimeRadius, customMinimumScore, customMaximumRideRequests, customFleetSize, customRideCount, (customMaxLat * customSectionSize),
								(customMaxLong * customSectionSize), customSectionSize);
							numberOfTests++;
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
					else if (clickCount >= clickTargetInit * 10 && clickCount < clickTargetInit * 15){
						clickTarget = 50;
					}
					else if (clickCount >= clickTargetInit * 15){
						clickTarget = 25;
					}

					if (increaseTimesToRun.getIsClicked(unitX, unitY)){
						customTimesToRun += 5 * coeff;
					}
					if (decreaseTimesToRun.getIsClicked(unitX, unitY) && customTimesToRun > 5){
						customTimesToRun -= 5 * coeff;
					}

					if (increaseTripWeight.getIsClicked(unitX, unitY)){
						customTripWeight += 0.001;
					}
					if (decreaseTripWeight.getIsClicked(unitX, unitY) && customTripWeight > 0.001){
						customTripWeight -= 0.001;
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
						customMinimumScore += 5 * coeff;
					}
					if (decreaseMinScore.getIsClicked(unitX, unitY) && customMinimumScore > 0){
						customMinimumScore -= 5 * coeff;
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

					if (increaseRequestCount.getIsClicked(unitX, unitY)){
						customRideCount += coeff;
					}
					if (decreaseRequestCount.getIsClicked(unitX, unitY) && customRideCount > 1){
						customRideCount -= coeff;
					}

					if (increaseSectionSize.getIsClicked(unitX, unitY)){
						customSectionSize += coeff;
					}
					if (decreaseSectionSize.getIsClicked(unitX, unitY) && customSectionSize > 1){
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
						paramsDialedIn = numberOfTests >= 1;
					}

					if (addTest.getIsClicked(unitX, unitY)){
						std::string testNameToUse = customTestName;
						if (customTestName == previousTestName){
							timesTitleUsed++;
							testNameToUse += "_" + std::to_string(timesTitleUsed);
						}
						else{
							timesTitleUsed = 0;
						}
						tester.initializeSimulatorWithParams(customTestName, customTimesToRun, customTripWeight, customRadiusMin, customRadiusStep, (customRadiusMax * customRadiusStep) + customRadiusMin,
							customTimeRadius, customMinimumScore, customMaximumRideRequests, customFleetSize, customRideCount, (customMaxLat * customSectionSize),
							(customMaxLong * customSectionSize), customSectionSize);
					}
				}
			}
			else{
				clickCount = 0;
			}
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			drawText(&program, -((float)customTestName.size() / 2.0), 18, 1, 1, customTestName, textSheet);

			for (Button button : nonRangedButtonsShared){
				button.draw(&program);
			}

			for (Button button : nonRangedButtonsPrivate){
				button.draw(&program);
			}

			drawText(&program, -13, 15, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

			drawText(&program, -13, 11.5, 1, 1, "Weight of Trip:", textSheet);
			drawText(&program, -13, 10.5, 1, 1, tripW.str(), textSheet);

			drawText(&program, -13, 5.5, 1, 1, "Minimum Search", textSheet);
			drawText(&program, -13, 4.5, 1, 1, "Radius: " + std::to_string(customRadiusMin), textSheet);

			drawText(&program, -13, 0, 1, 1, "Radius Step: " + std::to_string(customRadiusStep), textSheet);

			int valToDraw = customRadiusMin + customRadiusStep * customRadiusMax;
			drawText(&program, -13, -4.5, 1, 1, "Maximum Search", textSheet);
			drawText(&program, -13, -5.5, 1, 1, "Radius: " + std::to_string(valToDraw), textSheet);

			drawText(&program, -13, -10, 1, 1, "Time Radius: " + std::to_string(customTimeRadius), textSheet);

			drawText(&program, -13, -14.5, 1, 1, "Minimum", textSheet);
			drawText(&program, -13, -15.5, 1, 1, "Score: " + minScore.str(), textSheet);

			drawText(&program, 4, 15.5, 1, 1, "Destination Request", textSheet);
			drawText(&program, 4, 14.5, 1, 1, "Count Threshold: " + std::to_string(customMaximumRideRequests), textSheet);

			drawText(&program, 4, 10, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

			drawText(&program, 4, 5, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

			drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

			drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
			drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

			drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
			drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);

			doneButton.draw(&program);

			addTest.draw(&program);
			program.setViewMatrix(viewMatrix);
			program.setProjectionMatrix(projectionMatrix);
			SDL_GL_SwapWindow(displayWindow);
		}

	}


	if (getCustomRangedParams){
		//Alter location of buttons:
		increaseTimesToRun.setPosition(-15, 1, 0);
		decreaseTimesToRun.setPosition(-15, -1, 0);

		increaseFleetSize.setPosition(-15, -4, 0);
		decreaseFleetSize.setPosition(-15, -6, 0);

		increaseRequestCount.setPosition(-15, -9, 0);
		decreaseRequestCount.setPosition(-15, -11, 0);

		increaseSectionSize.setPosition(2, 1, 0);
		decreaseSectionSize.setPosition(2, -1, 0);

		increaseMaxLat.setPosition(2, -4, 0);
		decreaseMaxLat.setPosition(2, -6, 0);

		increaseMaxLong.setPosition(2, -9, 0);
		decreaseMaxLong.setPosition(2, -11, 0);

		std::string customTestName = "Custom Ranged Test";
		//Non-Ranged Values:
		unsigned customTimesToRun = 100;
		unsigned customFleetSize = 10;
		unsigned customRideCount = 100;
		int customMaxLat = 4;
		int customMaxLong = 4;
		int customSectionSize = 5;
		//End of non-ranged values.

		//Values for moving through range:
		float customTripWeightChange = 0.001;

		//CustomRadiusChange applies to min, max, and step radius, as well as time radius
		int customRadiusChange = 1;

		float customScoreChange = 5;

		unsigned customRideRequestsChange = 1;
		//

		//Ranged values:
		float customTripWeightTop = 0.003;
		float customTripWeightBottom = 0.002;

		int customRadiusMinTop = 6;
		int customRadiusMinBottom = 5;

		int customRadiusStepTop = 6;
		int customRadiusStepBottom = 5;

		int customRadiusMaxTop = 3;
		int customRadiusMaxBottom = 2;

		int customTimeRadiusTop = 6;
		int customTimeRadiusBottom = 5;

		float customMinimumScoreTop = 55;
		float customMinimumScoreBottom = 50;

		unsigned customMaximumRideRequestsTop = 31;
		unsigned customMaximumRideRequestsBottom = 30;
		//End of ranged values.

		bool paramsDialedIn = false;
		bool clicked = false;
		int clickCount = 0;
		int clickTarget = 200;
		int clickTargetInit = 200;
		int coeff = 1;
		viewMatrix.Scale(.1, .1, 1);
		while (!paramsDialedIn){
			glClear(GL_COLOR_BUFFER_BIT);
			std::stringstream customTripWeightTopStr, customTripWeightBottomStr;
			customTripWeightTopStr.precision(4);
			customTripWeightBottomStr.precision(4);
			customTripWeightTopStr << customTripWeightTop;
			customTripWeightBottomStr << customTripWeightBottom;

			std::stringstream customMinScoreTopStr, customMinScoreBottomStr;
			customMinScoreTopStr.precision(customMinimumScoreTop < 10 ? 2 : 3);
			customMinScoreBottomStr.precision(customMinimumScoreTop < 10 ? 2 : 3);
			customMinScoreTopStr << customMinimumScoreTop;
			customMinScoreBottomStr << customMinimumScoreBottom;

			while (SDL_PollEvent(&event)){
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
					SDL_Quit();
					return 0;
				}
				else if (event.type == SDL_MOUSEBUTTONDOWN){
					if (event.button.button == SDL_BUTTON_LEFT){
						unitX = (((float)event.button.x / winX) * (1.78 * 2)) - 1.78;
						unitY = (((winY - (float)event.button.y) / winY) * 4) - 2;
						unitX *= 10;
						unitY *= 10;

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

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);


								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);

								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
						}

						if (unitX >= 7 && unitX <= 7 + (std::to_string(customRadiusMinBottom).size() + std::to_string(customRadiusMaxTop).size() + 3) && unitY >= 16.5 && unitY <= 17.5){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = std::to_string(customRadiusMinBottom);
							std::string numRight = std::to_string(customRadiusMinTop);
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customRadiusMinBottom = std::stoi(numLeft);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, numLeft + "| : " + numRight, textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customRadiusMinTop = std::stoi(numRight);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, numLeft + " : " + numRight + "|", textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customRadiusMinBottom > customRadiusMinTop){
								int x = customRadiusMinBottom;
								customRadiusMinBottom = customRadiusMinTop;
								customRadiusMinTop = x;
							}
						}

						if (unitX >= 7 && unitX <= 7 + (std::to_string(customRadiusStepBottom).size() + std::to_string(customRadiusStepTop).size() + 3) && unitY >= 14.5 && unitY <= 15.5){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = std::to_string(customRadiusStepBottom);
							std::string numRight = std::to_string(customRadiusStepTop);
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customRadiusStepBottom = std::stoi(numLeft);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, numLeft + "| : " + numRight, textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customRadiusStepTop = std::stoi(numRight);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, numLeft + " : " + numRight + "|", textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customRadiusStepBottom > customRadiusStepTop){
								int x = customRadiusStepBottom;
								customRadiusStepBottom = customRadiusStepTop;
								customRadiusStepTop = x;
							}
						}

						/*if (unitX >= 7 && unitX <= 7 + (std::to_string(customRadiusMaxBottom).size() + std::to_string(customRadiusMaxTop).size() + 3) && unitY >= 12.5 && unitY <= 13.5){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = std::to_string(customRadiusMaxBottom);
							std::string numRight = std::to_string(customRadiusMaxTop);
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customRadiusMaxBottom = std::stoi(numLeft);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(std::stoi(numLeft) * customRadiusStepBottom + customRadiusMinBottom) + "| : " + std::to_string(std::stoi(numRight) * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customRadiusMaxTop = std::stoi(numRight);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(std::stoi(numLeft) * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(std::stoi(numRight) * customRadiusStepTop + customRadiusMinTop) + "|", textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customRadiusMaxBottom > customRadiusMaxTop){
								int x = customRadiusMaxBottom;
								customRadiusMaxBottom = customRadiusMaxTop;
								customRadiusMaxTop = x;
							}
						}*/

						if (unitX >= 7 && unitX <= 7 + (customTripWeightBottomStr.str().size() + customTripWeightTopStr.str().size() + 3) && unitY >= 10 && unitY <= 12){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = customTripWeightBottomStr.str();
							std::string numRight = customTripWeightTopStr.str();
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customTripWeightBottom = std::stof(numLeft);
												customTripWeightBottomStr.precision(4);
												customTripWeightBottomStr.clear();
												customTripWeightBottomStr << customTripWeightBottom;

											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, numLeft + "| : " + numRight, textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customTripWeightTop = std::stof(numRight);
												customTripWeightTopStr.precision(4);
												customTripWeightTopStr.clear();
												customTripWeightTopStr << customTripWeightTop;

											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, numLeft + " : " + numRight + "|", textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customTripWeightBottom > customTripWeightTop){
								int x = customTripWeightBottom;
								customTripWeightBottom = customTripWeightTop;
								customTripWeightTop = x;
							}
						}

						if (unitX >= 7 && unitX <= 7 + (customMinScoreBottomStr.str().size() + customMinScoreTopStr.str().size() + 3) && unitY >= 8.5 && unitY <= 9.5){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = customMinScoreBottomStr.str();
							std::string numRight = customMinScoreTopStr.str();
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customMinimumScoreBottom = std::stof(numLeft);
												customMinScoreBottomStr.precision(4);
												customMinScoreBottomStr.clear();
												customMinScoreBottomStr << customMinimumScoreBottom;

											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, numLeft + "| : " + numRight, textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customMinimumScoreTop = std::stof(numRight);
												customMinScoreTopStr.precision(4);
												customMinScoreTopStr.clear();
												customMinScoreTopStr << customMinimumScoreTop;

											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, numLeft + " : " + numRight + "|", textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customTripWeightBottom > customTripWeightTop){
								int x = customTripWeightBottom;
								customTripWeightBottom = customTripWeightTop;
								customTripWeightTop = x;
							}
						}

						if (unitX >= 7 && unitX <= 7 + (std::to_string(customTimeRadiusBottom).size() + std::to_string(customTimeRadiusTop).size() + 3) && unitY >= 6.5 && unitY <= 7.5){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = std::to_string(customTimeRadiusBottom);
							std::string numRight = std::to_string(customTimeRadiusTop);
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customTimeRadiusBottom = std::stoi(numLeft);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, numLeft + "| : " + numRight, textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customTimeRadiusTop = std::stoi(numRight);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, numLeft + " : " + numRight + "|", textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customTimeRadiusBottom > customTimeRadiusTop){
								int x = customTimeRadiusBottom;
								customTimeRadiusBottom = customTimeRadiusTop;
								customTimeRadiusTop = x;
							}
						}

						if (unitX >= 7 && unitX <= 7 + (std::to_string(customMaximumRideRequestsBottom).size() + std::to_string(customMaximumRideRequestsTop).size() + 3) && unitY >= 4.5 && unitY <= 5.5){
							SDL_bool doneWithFirst = SDL_FALSE;
							SDL_bool doneWithSecond = SDL_FALSE;
							std::string numLeft = std::to_string(customMaximumRideRequestsBottom);
							std::string numRight = std::to_string(customMaximumRideRequestsTop);
							SDL_StartTextInput();
							while (!doneWithFirst){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numLeft += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN || event.key.keysym.scancode == SDL_SCANCODE_TAB){
											doneWithFirst = SDL_TRUE;
											try{
												customMaximumRideRequestsBottom = std::stof(numLeft);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithFirst = SDL_FALSE;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numLeft.size() > 0){
												numLeft.erase(numLeft.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, numLeft + "| : " + numRight, textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}

							while (!doneWithSecond){
								glClear(GL_COLOR_BUFFER_BIT);
								SDL_Event event;
								char let = ' ';
								if (SDL_PollEvent(&event)){
									switch (event.type){
									case SDL_QUIT:
										doneWithFirst = SDL_TRUE;
										break;
									case SDL_TEXTINPUT:
										let = event.text.text[0];
										if (let >= '0' && let <= '9' || let == '.'){
											numRight += let;
										}
										break;
									case SDL_KEYDOWN:
										if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
											doneWithSecond = SDL_TRUE;
											try{
												customMaximumRideRequestsTop = std::stoi(numRight);
											}
											catch (const std::exception& e){
												std::cerr << "Not a valid number!" << std::endl;
												doneWithSecond = SDL_FALSE;
												continue;
											}
										}
										else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
											if (numRight.size() > 0){
												numRight.erase(numRight.end() - 1);
											}
										}
									}
								}
								drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

								drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
								drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

								drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
								drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

								drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
								drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

								drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
								drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

								drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
								drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

								drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
								drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

								drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
								drawText(&program, 7, 5, 1, 1, numLeft + " : " + numRight + "|", textSheet);

								int posY = 1;
								int buttonNum = 0;
								for (Button button : nonRangedButtonsShared){
									button.setPosition(-15, posY, 0);
									if (buttonNum >= 6){
										button.setPosition(2, posY, 0);
									}
									button.draw(&program);
									if (buttonNum % 2 == 0){
										posY -= 2;
									}
									else{
										posY -= 3;
									}
									buttonNum++;

									if (buttonNum == 6){
										posY = 1;
									}
								}

								drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

								drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

								drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

								drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

								drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

								drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
								drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);
								doneButton.draw(&program);
								program.setViewMatrix(viewMatrix);
								program.setProjectionMatrix(projectionMatrix);
								SDL_GL_SwapWindow(displayWindow);
							}
							if (customMaximumRideRequestsBottom > customMaximumRideRequestsTop){
								int x = customMaximumRideRequestsBottom;
								customMaximumRideRequestsBottom = customMaximumRideRequestsTop;
								customMaximumRideRequestsTop = x;
							}
						}
						
						if (increaseTimesToRun.getIsClicked(unitX, unitY)){
							customTimesToRun += 5 * coeff;
						}
						if (decreaseTimesToRun.getIsClicked(unitX, unitY) && customTimesToRun > 5){
							customTimesToRun -= 5 * coeff;
						}

						if (increaseFleetSize.getIsClicked(unitX, unitY)){
							customFleetSize += coeff;
						}
						if (decreaseFleetSize.getIsClicked(unitX, unitY) && customFleetSize > 1){
							customFleetSize -= coeff;
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
						if (decreaseSectionSize.getIsClicked(unitX, unitY) && customSectionSize > 1){
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
							int testNum = 0;
							testNum += /*customTripWeightTop == customTripWeightBottom ? 1 : */std::round((customTripWeightTop - customTripWeightBottom + customTripWeightChange) / customTripWeightChange);
							testNum *= /*customRadiusMinTop == customRadiusMinBottom ? 1 : */(customRadiusMinTop - customRadiusMinBottom + customRadiusChange) / customRadiusChange;
							testNum *= /*customRadiusStepTop == customRadiusStepBottom ? 1 : */(customRadiusStepTop - customRadiusStepBottom + customRadiusChange) / customRadiusChange;
							testNum *= /*customRadiusMaxTop == customRadiusMaxBottom ? 1 : */(customRadiusMaxTop - customRadiusMaxBottom + customRadiusChange) / customRadiusChange;
							testNum *= /*customTimeRadiusTop == customTimeRadiusBottom ? 1 : */(customTimeRadiusTop - customTimeRadiusBottom + customRadiusChange) / customRadiusChange;
							testNum *= /*customMinimumScoreBottom == customMinimumScoreTop ? 1 : */std::round((customMinimumScoreTop - customMinimumScoreBottom + customScoreChange) / customScoreChange);
							testNum *=/* customMaximumRideRequestsTop == customMaximumRideRequestsBottom ? 1 : */(customMaximumRideRequestsTop - customMaximumRideRequestsBottom + customRideRequestsChange) / customRideRequestsChange;
							std::string answer;
							std::cout << std::endl << "These parameters will result in the creation and execution of " << std::to_string(testNum) << " tests. Are you sure you want to continue? (y/n)" << std::endl << ">: ";
							do{
								std::getline(std::cin, answer);
							} while (answer != "y" && answer != "n");
							paramsDialedIn = answer == "y";
						}
					}
				}
			}
			drawText(&program, -(((float)customTestName.size() + 1) / 2.0), 18, 1, 1, customTestName, textSheet);

			drawText(&program, -17, 17, 1, 1, "Minimum Search Radius Range: ", textSheet);
			drawText(&program, 7, 17, 1, 1, std::to_string(customRadiusMinBottom) + " : " + std::to_string(customRadiusMinTop), textSheet);

			drawText(&program, -17, 15, 1, 1, "Search Radius Step Range: ", textSheet);
			drawText(&program, 7, 15, 1, 1, std::to_string(customRadiusStepBottom) + " : " + std::to_string(customRadiusStepTop), textSheet);

			drawText(&program, -17, 13, 1, 1, "Maximum Search Radius Range: ", textSheet);
			drawText(&program, 7, 13, 1, 1, std::to_string(customRadiusMaxBottom * customRadiusStepBottom + customRadiusMinBottom) + " : " + std::to_string(customRadiusMaxTop * customRadiusStepTop + customRadiusMinTop), textSheet);

			drawText(&program, -17, 11, 1, 1, "Trip Weight Range: ", textSheet);
			drawText(&program, 7, 11, 1, 1, customTripWeightBottomStr.str() + " : " + customTripWeightTopStr.str(), textSheet);

			drawText(&program, -17, 9, 1, 1, "Minimum Score Range: ", textSheet);
			drawText(&program, 7, 9, 1, 1, customMinScoreBottomStr.str() + " : " + customMinScoreTopStr.str(), textSheet);

			drawText(&program, -17, 7, 1, 1, "Time Radius Range: ", textSheet);
			drawText(&program, 7, 7, 1, 1, std::to_string(customTimeRadiusBottom) + " : " + std::to_string(customTimeRadiusTop), textSheet);

			drawText(&program, -17, 5, 1, 1, "Destination Ride Request Maximum: ", textSheet);
			drawText(&program, 7, 5, 1, 1, std::to_string(customMaximumRideRequestsBottom) + " : " + std::to_string(customMaximumRideRequestsTop), textSheet);

			int posY = 1;
			int buttonNum = 0;
			for (Button button : nonRangedButtonsShared){
				button.setPosition(-15, posY, 0);
				if (buttonNum >= 6){
					button.setPosition(2, posY, 0);
				}
				button.draw(&program);
				if (buttonNum % 2 == 0){
					posY -= 2;
				}
				else{
					posY -= 3;
				}
				buttonNum++;

				if (buttonNum == 6){
					posY = 1;
				}
			}

			drawText(&program, -13, 0, 1, 1, "Times to run: " + std::to_string(customTimesToRun), textSheet);

			drawText(&program, -13, -5, 1, 1, "Fleet Size: " + std::to_string(customFleetSize), textSheet);

			drawText(&program, -13, -10, 1, 1, "Request Count: " + std::to_string(customRideCount), textSheet);

			drawText(&program, 4, 0, 1, 1, "Section Size: " + std::to_string(customSectionSize), textSheet);

			drawText(&program, 4, -4.5, 1, 1, "Maximum", textSheet);
			drawText(&program, 4, -5.5, 1, 1, "Latitude: " + std::to_string(customMaxLat * customSectionSize), textSheet);

			drawText(&program, 4, -9.5, 1, 1, "Maximum", textSheet);
			drawText(&program, 4, -10.5, 1, 1, "Longitude: " + std::to_string(customMaxLong * customSectionSize), textSheet);

			doneButton.draw(&program);
			program.setViewMatrix(viewMatrix);
			program.setProjectionMatrix(projectionMatrix);
			SDL_GL_SwapWindow(displayWindow);
		}

		int i = 1;
		for (float customTripWeight = customTripWeightBottom; customTripWeight <= customTripWeightTop; customTripWeight += customTripWeightChange){
			for (int customRadiusMin = customRadiusMinBottom; customRadiusMin <= customRadiusMinTop; customRadiusMin += customRadiusChange){
				for (int customRadiusStep = customRadiusStepBottom; customRadiusStep <= customRadiusStepTop; customRadiusStep += customRadiusChange){
					for (int customRadiusMax = customRadiusMaxBottom; customRadiusMax <= customRadiusMaxTop; customRadiusMax += customRadiusChange){
						for (int customTimeRadius = customTimeRadiusBottom; customTimeRadius <= customTimeRadiusTop; customTimeRadius += customRadiusChange){
							for (float customMinimumScore = customMinimumScoreBottom; customMinimumScore <= customMinimumScoreTop; customMinimumScore += customScoreChange){
								for (unsigned customMaxRideRequests = customMaximumRideRequestsBottom; customMaxRideRequests <= customMaximumRideRequestsTop; customMaxRideRequests += customRideRequestsChange){
									std::string customNameToUse = customTestName + '_' + std::to_string(i++);
									tester.initializeSimulatorWithParams(customNameToUse, customTimesToRun, customTripWeight, customRadiusMin, customRadiusStep, 
										customRadiusMax * customRadiusStep + customRadiusMin, customTimeRadius, customMinimumScore, customMaxRideRequests, customFleetSize, customRideCount, customMaxLat * customSectionSize, 
										customMaxLong * customSectionSize, customSectionSize, true);
								}
							}
						}
					}
				}
			}
		}
	}

	tester.runTests();
	tester.prepareToRender();
	std::vector<std::string> testNames = tester.getTestNames();
	bool escapePressed = false;
	int escapedPressedGoal = 100;
	int escapePressedTime = 0;
	for (std::string testName : testNames){
		//displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 720, SDL_WINDOW_OPENGL);
		//projectionMatrix.setOrthoProjection(-1.78, 1.78, -2.0f, 2.0f, -1.0f, 1.0f);
		int timesToRun = tester.getTimesToRun(testName);
		float timesRun = 0;
		bool done = false;
		bool paused = false;
		bool kPressed = false;
		bool jPressed = false;
		bool lPressed = false;
		float speed = 0.008;
		while (!done) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
					done = true;
				}
			}
			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;
			if (!paused){
					if (timesRun < timesToRun){
						timesRun += speed;
					}
					else{
						timesRun = timesToRun;
					}
			}
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_K] && !kPressed){
				paused = !paused;
			}
			kPressed = state[SDL_SCANCODE_K];

			if (state[SDL_SCANCODE_J] && !jPressed){
				speed -= 0.001;
			}

			jPressed = state[SDL_SCANCODE_J];

			if (speed <= 0){
				speed = 0.001;
			}

			if (state[SDL_SCANCODE_L] && !lPressed){
				speed += 0.001;
			}
			
			lPressed = state[SDL_SCANCODE_L];

			if (speed > 1){
				speed = 1;
			}
			program.setProjectionMatrix(projectionMatrix);
			tester.visualize(timesRun, state, event, &program, testName, displayWindow);
			SDL_SetWindowTitle(displayWindow, testName.c_str());
			SDL_GL_SwapWindow(displayWindow);
			//}
			if (/*timesRun >= timesToRun*/state[SDL_SCANCODE_ESCAPE] && (!escapePressed || escapePressedTime % escapedPressedGoal == 0)){
				done = true;
			}
			escapePressed = state[SDL_SCANCODE_ESCAPE];
			if (escapePressed){
				escapePressedTime++;
			}
			else{
				escapePressedTime = 0;
			}
		}
	}
	tester.freeMemory();
	SDL_Quit();
	return 0;
}
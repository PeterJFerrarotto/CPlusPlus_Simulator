#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(WIN82) || defined(_WIN82) || defined(_WINDOWS)
#include <GL/glew.h>
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
#include "Matrix.h"

SDL_Window* displayWindow;

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	float lastFrameTicks = 0.0f;
	glViewport(0, 0, 640, 720);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	//ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Matrix projectionMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	Simulator tester;
	tester.runTests();
	std::vector<std::string> testNames = tester.getTestNames();
	SDL_Event event;
	for (std::string testName : testNames){
		//Matrix viewMatrix;
		//program.setViewMatrix(viewMatrix);
		tester.prepareToRender(testName);
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
			//if (elapsed < 10){
			timesRun += 0.008;
			const Uint8 *state = SDL_GetKeyboardState(NULL);
			program.setProjectionMatrix(projectionMatrix);
			tester.visualize(timesRun, state, event, &program, testName);
			SDL_GL_SwapWindow(displayWindow);
			//}
			if (timesRun >= timesToRun){
				done = true;
			}
		}
	}

	SDL_Quit();
	return 0;
}
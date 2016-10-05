/**
* Project Name: Random fractal trees in 3D
* Author: ashiswin (Isaac Ashwin)
* File modified: 11.07.15
* Based of the project: Random generated fractal trees
* Author: teaprog
* File created: 02.03.12
*/

#define GLM_SWIZZLE

#include <GL/glew.h>
#include <GL/glxew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include "Shader.h"
#include "Tree.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using std::vector;

const GLfloat PI = 3.14159265;

int screenWidth = 1920;
int screenHeight = 1080;
bool quit = false;
SDL_Event event;

void init();
void reshape(int, int);
void display();
void input();

SDL_Window *screen;
SDL_GLContext context;
GLfloat rot;
GLfloat translateZ;

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 translatedViewMatrix;
glm::mat4 modelMatrix(1.0f);

int projectionMatrixLocation;
int viewMatrixLocation;
int modelMatrixLocation;

Shader shader;
Tree tree;

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	screen = SDL_CreateWindow("Fractal Tree", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	if(screen == NULL) {
		printf("Could not create screen: %s\n", SDL_GetError());
	}
	
	context = SDL_GL_CreateContext(screen);
	
	glewExperimental = GL_TRUE;
	int error = glewInit();
	if(error != GLEW_OK) {
		printf("Glew failed to init: %s\n", glewGetErrorString(error));
		exit(1);
	}
	glGetError(); // Clear INVALID_ENUM error from glew

	glClearColor(0.3f, 0.5f, 0.8f, 0.0f);
	glClearDepth(1.0f);
	glLineWidth(2.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	reshape(screenWidth, screenHeight);

	srand(SDL_GetTicks());
	
	shader.addShader(GL_VERTEX_SHADER, "shader.vert");
	shader.addShader(GL_FRAGMENT_SHADER, "shader.frag");
	shader.compileShader();
	
	projectionMatrixLocation = shader.getUniformLocation("projectionMatrix");
	viewMatrixLocation = shader.getUniformLocation("viewMatrix");
	modelMatrixLocation = shader.getUniformLocation("modelMatrix");
	
	tree.clear();
	do {
		tree.generateFractalTree(screenHeight);
	} while(0.0001 * tree.getBranches() < 1);
	
	printf("Projection Location: %d\nView Location: %d\nModel Location: %d\n", projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation);
}

void reshape(int screenW, int screenH) {
	screenWidth = screenW;
	screenHeight = screenH;

	glViewport(0, 0, screenW, screenH);
	projectionMatrix = glm::perspective(45.0f, (float)1920/1080, 0.1f, 10000.f);
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(screenHeight / 4), -500.0f));
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.bind();
	
	modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
	translatedViewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, translateZ));
	
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &translatedViewMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	tree.drawTree();
	
	shader.unbind();
	
	SDL_GL_SwapWindow(screen);
}

void input() {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_n:
						tree.clear();
						do {
							tree.generateFractalTree(screenHeight);
						} while(0.0001 * tree.getBranches() < 1);
						break;
					case SDLK_LEFT:
						rot += 5.0f;
						break;
					case SDLK_RIGHT:
						rot -= 5.0f;
						break;
					case SDLK_w:
						translateZ -= 5.0f;
						break;
					case SDLK_s:
						translateZ += 5.0f;
						break;
				}
				break;
		}
	}
}

int main(int argc, char** argv) {
	printf("Begin initialization...\n");
	init();
	
	rot = 0.0f;
	translateZ = 0.0f;
	
	printf("Initialization complete\n");
	
	while (!quit) {
		input();
		display();
	}

	SDL_Quit();

	return 0;
}

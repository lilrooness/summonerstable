#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>
#include <stdio.h>
#include <string>
#include "solitaire/shader_reader.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Graphics program
GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

bool initGL() {
	
	bool success = true;

	gProgramID = create_shader_program("shader.vs", "shader.fs");
	gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
	if (gVertexPos2DLocation == -1) {
		printf("LVertexPos2D is not a valid glsl program variable!\n");
		success = false;
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		GLfloat vertexData[] = {
			-0.5, -0.5f,
			0.5, -0.5f,
			0.5, 0.5f,
			-0.5f, 0.5f
		};

		GLuint indexData[] = { 0, 1, 2, 3 };

		glGenBuffers(1, &gVBO);
		glBindBuffer(GL_ARRAY_BUFFER, gVBO);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

		glGenBuffers(1, &gIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
	}
	return success;
}

void render_fun() {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gProgramID);
	glEnableVertexAttribArray(gVertexPos2DLocation);

	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(gVertexPos2DLocation);

	glUseProgram(NULL);
}


int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	SDL_GLContext gcontext;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		//Create window
		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			gcontext = SDL_GL_CreateContext(window);
			if (gcontext == NULL) {
				printf("OpenGL context could not be craeted! SDL Error: %s\n", SDL_GetError());
			}
			else {
				glewExperimental = GL_TRUE;
				GLenum glewError = glewInit();
				if (glewError != GLEW_OK) {
					printf("Error initialising Glew: %s\n", glewGetErrorString(glewError));
				}

				//use vsync 
				if (SDL_GL_SetSwapInterval(1) < 0) {
					printf("Warning unable to set Vsync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialise OpenGL
				if (!initGL()) {
					printf("unable to initialise opengl");
					return 1;
				}
			}
		}
	}

	bool quit = false;

	SDL_Event e;

	while (!quit) {

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}
		render_fun();
		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(gProgramID);

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}


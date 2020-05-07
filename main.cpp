#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>
#include <stdio.h>
#include <string>
#include "solitaire/shader_reader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "solitaire/stb_image.h"

#include "solitaire/glm/glm.hpp"
#include "solitaire/glm/gtc/matrix_transform.hpp"
#include "solitaire/glm/gtc/type_ptr.hpp"

#include "solitaire/game.h"
#include "solitaire/material.h";

//Screen dimensions
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//Graphics program
GLuint gProgramID = 0;
GLuint gCandelProgramID = 0;

GLint gVertexPos2DLocation = -1;
GLint gTextureUVLocation = -1;
GLint gVertexTranslationLocation = -1;
GLint gTextureTranslationLocation = -1;

GLint gModelLocation = -1;
GLint gViewLocation = -1;
GLint gProjectionLocation = -1;

unsigned int cardTexture;
unsigned int cardNumberTexture;
unsigned int candelTexture;

//GLfloat *transformationData;
//GLfloat *textureTransformationData;
//GLfloat *numbersTextureTransformationData;
//GLfloat* candelsTextureTransformationData;
//GLfloat* candelsTransformationData;

InstancedSpriteMeshMaterial2D cardMeshMaterial;
InstancedSpriteMeshMaterial2D numbersMeshMaterial;
InstancedSpriteMeshMaterial2D candelsMeshMaterial;

glm::mat4 proj;
glm::mat4 model;
glm::mat4 view;

//Game game;

bool initGL(Game *game) {

	proj = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f, 1.0f, 100.0f);
	model = glm::mat4(1.0);
	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0f));
	model = glm::scale(model, glm::vec3(256.0f, 256.0f, 1.0f));
	bool success = true;

	glGenTextures(1, &candelTexture);
	glBindTexture(GL_TEXTURE_2D, candelTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &cardNumberTexture);
	glBindTexture(GL_TEXTURE_2D, cardNumberTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &cardTexture);
	glBindTexture(GL_TEXTURE_2D, cardTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	int height, width, nColourChannels;
	unsigned char* cardTextureData = stbi_load("textures/cards_sprites.png", &width, &height, &nColourChannels, 0);

	glBindTexture(GL_TEXTURE_2D, cardTexture);
	if (cardTextureData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cardTextureData);
	} else {
		std::cout << "Failed to load card texture" << std::endl;
	}

	stbi_image_free(cardTextureData);

	unsigned char* numberTextureData = stbi_load("textures/numbers_sprites.png", &width, &height, &nColourChannels, 0);

	glBindTexture(GL_TEXTURE_2D, cardNumberTexture);
	if (numberTextureData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, numberTextureData);
	}
	else {
		std::cout << "Failed to load number texture" << std::endl;
	}

	stbi_image_free(numberTextureData);

	unsigned char* candelTextureData = stbi_load("textures/candels.png", &width, &height, &nColourChannels, 0);

	glBindTexture(GL_TEXTURE_2D, candelTexture);
	if (candelTextureData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, candelTextureData);
	}
	else {
		std::cout << "Failed to load candel texture" << endl;
	}

	gProgramID = create_shader_program("shaders/shader.vs", "shaders/shader.fs");
	gCandelProgramID = create_shader_program("shaders/candel_shader.vs", "shaders/candel_shader.fs");

	gModelLocation = glGetUniformLocation(gProgramID, "model");
	gViewLocation = glGetUniformLocation(gProgramID, "view");
	gProjectionLocation = glGetUniformLocation(gProgramID, "projection");

	gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
	gTextureUVLocation = glGetAttribLocation(gProgramID, "ATexCoord");
	gVertexTranslationLocation = glGetAttribLocation(gProgramID, "APos");
	gTextureTranslationLocation = glGetAttribLocation(gProgramID, "AUVOffset");

	if (gTextureTranslationLocation == -1) {
		cout << "Error getting AUVOffset attribute location" << endl;
		cout << glGetError() << endl;
	}

	if (gVertexPos2DLocation == -1) {
		printf("LVertexPos2D is not a valid glsl program variable!\n");
		success = false;
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		GLfloat cardVertexData[] = {
			//Vertex Coordinates
			0.0f, 0.0f, 0.0f,
			0.5f, 0.0f, 0.0f,
			0.5f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			//Texture Coordinates
			0.0f, 0.25f,
			0.125f, 0.25f,
			0.125f, 0.0f,
			0.0f, 0.0f
		};

		GLfloat numberVertexData[] = {
			//Vertex Coordinates
			0.0f, 0.0f, 0.1f,
			0.1f, 0.0f, 0.1f,
			0.1f, 0.1f, 0.1f,
			0.0f, 0.1f, 0.1f,
			//Texture Coordinates
			0.0f, 0.1f,
			0.1f, 0.1f,
			0.1f, 0.0f,
			0.0f, 0.0f
		};

		GLfloat candelVertexData[] = {
			//Vertex Coordinates
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			//Texture Coordinates
			0.0f, 0.0625f,
			0.0625f, 0.0625f,
			0.0625f, 0.0f,
			0.0f, 0.0f
		};

		initInstancedSpriteMeshMaterial_2D(
			gVertexPos2DLocation,
			gTextureTranslationLocation,
			gTextureUVLocation,
			gVertexTranslationLocation,
			cardVertexData, 20,
			game->Buffer_cardsTextureOffsetData.data(), game->Buffer_cardsTextureOffsetData.size(),
			game->Buffer_cardsVertexOffsetData.data(), game->Buffer_cardsVertexOffsetData.size(),
			std::vector<InstancedSpriteShaderAttribute_Float>(),
			&cardMeshMaterial);

		initInstancedSpriteMeshMaterial_2D(
			gVertexPos2DLocation,
			gTextureTranslationLocation,
			gTextureUVLocation,
			gVertexTranslationLocation,
			numberVertexData, 20,
			game->Buffer_numbersTextureOffsetData.data(), game->Buffer_numbersTextureOffsetData.size(),
			game->Buffer_cardsVertexOffsetData.data() , game->Buffer_cardsVertexOffsetData.size(),
			std::vector<InstancedSpriteShaderAttribute_Float>(),
			&numbersMeshMaterial);

		InstancedSpriteShaderAttribute_Float candleOnShaderAttrib;
		candleOnShaderAttrib.id = 0;
		candleOnShaderAttrib.name = "candleOn";
		candleOnShaderAttrib.dim = 1;
		candleOnShaderAttrib.location = glGetAttribLocation(gCandelProgramID, candleOnShaderAttrib.name);
		candleOnShaderAttrib.instancedDataSize = game->Buffer_candlesStateData.size();
		candleOnShaderAttrib.instancedData = game->Buffer_candlesStateData.data();

		initInstancedSpriteMeshMaterial_2D(
			gVertexPos2DLocation,
			gTextureTranslationLocation,
			gTextureUVLocation,
			gVertexTranslationLocation,
			candelVertexData, 20,
			game->Buffer_candlesTextureOffsetData.data(), game->Buffer_candlesTextureOffsetData.size(),
			game->Buffer_candlesVertexOffsetData.data(), game->Buffer_candlesVertexOffsetData.size(),
			std::vector<InstancedSpriteShaderAttribute_Float> { candleOnShaderAttrib },
			&candelsMeshMaterial);
	}
	return success;
}

void render_fun(Game *game) {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(gProgramID);
	
	glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(gViewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(gProjectionLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glBindTexture(GL_TEXTURE_2D, cardTexture);
	glBindVertexArray(cardMeshMaterial.vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->Buffer_cardsVertexOffsetData.size() /3);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, cardNumberTexture);
	glBindVertexArray(numbersMeshMaterial.vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->Buffer_numbersTextureOffsetData.size() / 2);
	glDisable(GL_BLEND);

	glUseProgram(gCandelProgramID);
	glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(gViewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(gProjectionLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glBindTexture(GL_TEXTURE_2D, candelTexture);
	glBindVertexArray(candelsMeshMaterial.vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->Buffer_candlesTextureOffsetData.size() / 2);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(NULL);
}


int main( int argc, char* args[] )
{
	Game game;
	init_game(&game);

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
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		//Create window
		window = SDL_CreateWindow( "Summoners Tower", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 960/*NULL, NULL*/, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/);
		SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
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
				if (!initGL(&game)) {
					printf("unable to initialise opengl");
					return 1;
				}
			}
		}
	}

	bool quit = false;

	SDL_Event e;

	glm::vec2 mouseTransformation = glm::vec2(0,0);

	Uint32 ticks = SDL_GetTicks();

	while (!quit) {

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
				//else if (e.key.keysym.sym == SDLK_SPACE) {
				//	end_turn(&game);
				//}
			}
			else if (e.type == SDL_MOUSEMOTION) {

				float mouseX = 1.0f + (-1.0f + ((float)e.motion.x / (float)SCREEN_WIDTH * 2.0f));
				float mouseY = 1 + (1.0f - ((float)e.motion.y / (float)SCREEN_HEIGHT * 2.0f));

				glm::mat4 inverseProjectionViewMatrix = glm::inverse(proj * view);
				glm::vec4 mouseWorldCoord = glm::vec4(mouseX, mouseY, 0, 1) * inverseProjectionViewMatrix;
				mouseTransformation.x = mouseWorldCoord.x;
				mouseTransformation.y = mouseWorldCoord.y;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				game.lmbDown = true;
			}
			else if (e.type == SDL_MOUSEBUTTONUP) {
				game.lmbDown = false;
			}

		}

		Uint32 currentTicks = SDL_GetTicks();
		if (SDL_TICKS_PASSED(currentTicks, ticks+16)) {
			Uint32 diff = currentTicks - ticks;
			float dt = (float)diff / 1000.0f;
			ticks = currentTicks;
			tick(&game, mouseTransformation.x, mouseTransformation.y, dt);
		}

		if (game.BufferRefreshFlag_cardsVertexOffsetData) {
			glBindBuffer(GL_ARRAY_BUFFER, cardMeshMaterial.positionOffsetVBO);
			glBufferData(GL_ARRAY_BUFFER, game.Buffer_cardsVertexOffsetData.size() * sizeof(GLfloat), game.Buffer_cardsVertexOffsetData.data(), GL_STATIC_DRAW);
			

			glBindBuffer(GL_ARRAY_BUFFER, numbersMeshMaterial.positionOffsetVBO);
			glBufferData(GL_ARRAY_BUFFER, game.Buffer_cardsVertexOffsetData.size() * sizeof(GLfloat), game.Buffer_cardsVertexOffsetData.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			game.BufferRefreshFlag_cardsVertexOffsetData = false;
		}

		//if (game.cardTextureTransformDataUpdated) {
		//	textureTransformationData = game.textureOffsetData.data();
		//	glBindBuffer(GL_ARRAY_BUFFER, gTextureTranslationLocation);
		//	glBufferData(GL_ARRAY_BUFFER, game.textureOffsetData.size(), textureTransformationData, GL_STATIC_DRAW);
		//	glBindBuffer(GL_ARRAY_BUFFER, 0);
		//	cout << glGetError() << endl;
		//	game.cardTextureTransformDataUpdated = false;
		//}

		//if (game.candleStateChanged) {
		//	for (InstancedSpriteShaderAttribute_Float& attrib : candelsMeshMaterial.shaderAttributes) {
		//		if (attrib.id == 0) {
		//			attrib.instancedData = game.candles.data();
		//			attrib.instancedDataSize = game.candles.size();
		//			glBindBuffer(GL_ARRAY_BUFFER, attrib.bufferHandle);
		//			glBufferData(GL_ARRAY_BUFFER, attrib.instancedDataSize * sizeof(GLfloat), attrib.instancedData, GL_STATIC_DRAW);
		//			glBindBuffer(GL_ARRAY_BUFFER, 0);
		//			game.candleStateChanged = false;
		//		}
		//	}
		//}

		render_fun(&game);
		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(gProgramID);

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}


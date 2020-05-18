#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
//#include <SDL_mixer.h>
#include <GL/GLU.h>
#include <stdio.h>
#include <string>
#include "solitaire/sound.h"
#include "solitaire/shader_reader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "solitaire/stb_image.h"

#include "solitaire/glm/glm.hpp"
#include "solitaire/glm/gtc/matrix_transform.hpp"
#include "solitaire/glm/gtc/type_ptr.hpp"

#include "solitaire/game.h"
#include "solitaire/material.h"
#include "solitaire/sprite_material.h"

//Screen dimensions
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

//Graphics program
GLuint gProgramID = 0;
GLuint gCandelProgramID = 0;
GLuint spriteShaderProgramID = 0;

GLint gVertexPos2DLocation = -1;
GLint gTextureUVLocation = -1;
GLint gVertexTranslationLocation = -1;
GLint gTextureTranslationLocation = -1;

GLint gModelLocation = -1;
const GLint MODEL_LOCATION = 0;
GLint gViewLocation = -1;
const GLint VIEW_LOCATION = 1;
GLint gProjectionLocation = -1;
const GLint PROJECTION_LOCATION = 2;

unsigned int candelTexture;

InstancedSpriteMeshMaterial2D candelsMeshMaterial;
InstancedSpriteMeshMaterial2D attackMeshMaterial;

SpriteMaterial attackSpriteMaterial;
SpriteMaterial summonCircleSpriteMaterial;


glm::mat4 proj;
glm::mat4 model;
glm::mat4 view;

bool initGL(Game *game) {

	proj = glm::ortho(0.0f, 1800.0f, 0.0f, 1400.0f, 1.0f, 100.0f);
	model = glm::mat4(1.0);
	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -50.0f));
	model = glm::scale(model, glm::vec3(DEFUALT_MODEL_SCALE, DEFUALT_MODEL_SCALE, 1.0f));
	bool success = true;

	glGenTextures(1, &candelTexture);
	glBindTexture(GL_TEXTURE_2D, candelTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	int height, width, nColourChannels;
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
	spriteShaderProgramID = create_shader_program("shaders/sprite_shader.vs", "shaders/sprite_shader.fs");

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
			0.0f, CARD_SPRITE_HEIGHT,
			CARD_SPRITE_WIDTH, CARD_SPRITE_HEIGHT,
			CARD_SPRITE_WIDTH, 0.0f,
			0.0f, 0.0f
		};

		//0.0625f = 1/64
		GLfloat candelVertexData[] = {
			//Vertex Coordinates
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 2.0f, 0.0f,
			0.0f, 2.0f, 0.0f,
			//Texture Coordinates
			0.0f, NEW_CANDLE_SPRITE_HEIGHT,
			NEW_CANDLE_SPRITE_WIDTH, NEW_CANDLE_SPRITE_HEIGHT,
			NEW_CANDLE_SPRITE_WIDTH, 0.0f,
			0.0f, 0.0f
		};

		//0.0625f = 1/64
		GLfloat summonCircleVertexData[] = {
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

		//0.0625f = 1/64
		GLfloat spellsVertexData[] = {
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

		//0.03125f = 1/32
		GLfloat AttacksVertexData[] = {
			//Vertex Coordinates
			0.0f, 0.0f, 0.0f,
			0.5f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
			//Texture Coordinates
			0.0f, 0.03125f,
			0.03125f, 0.03125f,
			0.03125f, 0.0f,
			0.0f, 0.0f
		};

		initSpriteMaterial(cardVertexData, 20,
			game->cardSpriteClass.Buffer_tintValueData,
			game->cardSpriteClass.Buffer_scaleValueData,
			game->cardSpriteClass.Buffer_vertexOffsetData,
			game->cardSpriteClass.Buffer_textureOffsetData,
			&game->cardSpriteClass.material);

		initSpriteMaterial(summonCircleVertexData, 20,
			game->Buffer_circleTintValueData,
			game->Buffer_circleScaleValueData,
			game->Buffer_circleVertexOffsetData,
			game->Buffer_circleTextureOffsetData,
			&summonCircleSpriteMaterial);

		initSpriteMaterial(AttacksVertexData, 20,
			game->Buffer_attacksTintValueData,
			game->Buffer_attacksScaleValueData,
			game->Buffer_attacksVertexOffsetData,
			game->Buffer_attacksTextureOffsetData,
			&attackSpriteMaterial);

		initSpriteMaterial(spellsVertexData, 20,
			game->spellSpriteClass.Buffer_tintValueData,
			game->spellSpriteClass.Buffer_scaleValueData,
			game->spellSpriteClass.Buffer_vertexOffsetData,
			game->spellSpriteClass.Buffer_textureOffsetData,
			&game->spellSpriteClass.material);

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(spriteShaderProgramID);
	glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(gViewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(gProjectionLocation, 1, GL_FALSE, glm::value_ptr(proj));
	glBindTexture(GL_TEXTURE_2D, candelTexture);

	glBindVertexArray(summonCircleSpriteMaterial.BufferHandle_VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->Buffer_circleVertexOffsetData.size() / 3);

	glBindVertexArray(attackSpriteMaterial.BufferHandle_VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->Buffer_attacksVertexOffsetData.size() / 3);
	
	glBindVertexArray(game->spellSpriteClass.material.BufferHandle_VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->spellSpriteClass.Buffer_textureOffsetData.size() / 2);

	//glBindTexture(GL_TEXTURE_2D, cardTexture);
	glBindVertexArray(game->cardSpriteClass.material.BufferHandle_VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->cardSpriteClass.Buffer_vertexOffsetData.size() / 3);

	glUseProgram(gCandelProgramID);
	glUniformMatrix4fv(gModelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(gViewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(gProjectionLocation, 1, GL_FALSE, glm::value_ptr(proj));

	glBindTexture(GL_TEXTURE_2D, candelTexture);
	glBindVertexArray(candelsMeshMaterial.vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, game->Buffer_candlesTextureOffsetData.size() / 2);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glUseProgram(NULL);
}


int main( int argc, char* args[] )
{
	Game game;
	

	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	
	SDL_GLContext gcontext;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

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

				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096*2) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				}

				init_game(&game);

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
				else if (e.key.keysym.sym == SDLK_SPACE) {
					game.turnEndedByPlayer = true;
				}
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
		if (SDL_TICKS_PASSED(currentTicks, ticks+(1000.0f/100.0f))) {
			Uint32 diff = currentTicks - ticks;
			float dt = (float)diff / /*16.0f*/ (1000.0f / 100.0f);
			ticks = currentTicks;
			tick(&game, mouseTransformation.x, mouseTransformation.y, dt);
		}

		if (game.BufferRefreshFlag_candlesStateData) {
			candelsMeshMaterial.shaderAttributes[0].instancedData = game.Buffer_candlesStateData.data();
			candelsMeshMaterial.shaderAttributes[0].instancedDataSize = game.Buffer_candlesStateData.size();
			glBindBuffer(GL_ARRAY_BUFFER, candelsMeshMaterial.shaderAttributes[0].bufferHandle);
			glBufferData(GL_ARRAY_BUFFER, game.Buffer_candlesStateData.size() * sizeof(GLfloat), game.Buffer_candlesStateData.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			game.BufferRefreshFlag_candlesStateData = false;
		}

		if (game.BufferRefreshFlag_candlesTextureOffsetData) {
			glBindBuffer(GL_ARRAY_BUFFER, candelsMeshMaterial.textureOffsetVBO_instanced);
			glBufferData(GL_ARRAY_BUFFER, game.Buffer_candlesTextureOffsetData.size() * sizeof(GLfloat), game.Buffer_candlesTextureOffsetData.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			game.BufferRefreshFlag_candlesTextureOffsetData = false;
		}

		if (game.cardSpriteClass.BufferRefreshFlag_vertexOffsetData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, game.cardSpriteClass.material.BufferHandle_vertexOffsetData, game.cardSpriteClass.Buffer_vertexOffsetData, GL_STATIC_DRAW);
			game.cardSpriteClass.BufferRefreshFlag_vertexOffsetData = false;
		}

		if (game.cardSpriteClass.BufferRefreshFlag_textureOffsetData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, game.cardSpriteClass.material.BufferHandle_textureOffsetData, game.cardSpriteClass.Buffer_textureOffsetData, GL_STATIC_DRAW);
			game.cardSpriteClass.BufferRefreshFlag_textureOffsetData = false;
		}

		if (game.cardSpriteClass.BufferRefreshFlag_scaleValueData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, game.cardSpriteClass.material.BufferHandleInstanced_scaleData, game.cardSpriteClass.Buffer_scaleValueData, GL_STATIC_DRAW);
			game.cardSpriteClass.BufferRefreshFlag_scaleValueData = false;
		}

		if (game.cardSpriteClass.BufferRefreshFlag_tintValueData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, game.cardSpriteClass.material.BufferHandleInstanced_tintData, game.cardSpriteClass.Buffer_tintValueData, GL_STATIC_DRAW);
			game.cardSpriteClass.BufferRefreshFlag_tintValueData;
		}

		if (game.BufferRefreshFlag_attacksVertexOffsetData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, attackSpriteMaterial.BufferHandle_vertexOffsetData, game.Buffer_attacksVertexOffsetData, GL_STATIC_DRAW);
			game.BufferRefreshFlag_attacksVertexOffsetData = false;
		}

		if (game.BufferRefreshFlag_attacksTextureOffsetData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, attackSpriteMaterial.BufferHandle_textureOffsetData, game.Buffer_attacksTextureOffsetData, GL_STATIC_DRAW);
			game.BufferRefreshFlag_attacksTextureOffsetData = false;
		}

		if (game.BufferRefreshFlag_attacksScaleValueData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, attackSpriteMaterial.BufferHandleInstanced_scaleData, game.Buffer_attacksScaleValueData, GL_STATIC_DRAW);
			game.BufferRefreshFlag_attacksScaleValueData = false;
		}

		if (game.BufferRefreshFlag_circleTintValueData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, summonCircleSpriteMaterial.BufferHandleInstanced_tintData, game.Buffer_circleTintValueData, GL_STATIC_DRAW);
			game.BufferRefreshFlag_circleTintValueData = false;
		}

		if (game.spellSpriteClass.BufferRefreshFlag_scaleValueData) {
			refreshBuffer<GLfloat>(GL_ARRAY_BUFFER, game.spellSpriteClass.material.BufferHandleInstanced_scaleData, game.spellSpriteClass.Buffer_scaleValueData, GL_STATIC_DRAW);
			game.spellSpriteClass.BufferRefreshFlag_tintValueData = false;
		}

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


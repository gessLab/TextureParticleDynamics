/*
Author: Daniel Rehberg, Finley Huggins
Date Created: Janurary 26, 2025
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TexDyn.hpp"

constexpr int RESX = 1280;
constexpr int RESY = 720;

GLuint vbo, vao, vert, frag, program, tex;
GLsizei verts;
GLint uniProj, uniView, uniModel, uniTex;

void buildShaders();
void closeShaders();
void buildBuffers();
void closeBuffers();

std::uint32_t testing[1024 * 1024];

int main(int argc, char** argv)
{
	for (size_t i = 0; i < 1024 * 1024; ++i)
	{
		testing[i] = 0x00FF0000;
	}
	//SETUP
	SDL_Window* window = nullptr;
	SDL_GLContext context;
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Failed to initialize SDL: " << SDL_GetError();
		return 1;
	}

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("ViewSpace", RESX, RESY, SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS);
	if (window == nullptr)
	{
		std::cerr << "Failed to create window: " << SDL_GetError();
		SDL_Quit();
		return 2;
	}
	context = SDL_GL_CreateContext(window);

	glewExperimental = true;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
	{
		std::cerr << "Glew error: " << glewGetErrorString(glewErr);
		SDL_GL_DestroyContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 3;
	}

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	buildShaders();
	buildBuffers();

	DynamicTexture dt;
	dt.uploadTexture(tex);

	glm::mat4 camera = glm::lookAt(glm::vec3(0.1f, 2.5f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(
		glm::radians(75.0f), 16.0f / 9.0f,
		1.0f, 10.0f
	);

	SDL_Event e;
	bool quit = false;
	std::chrono::time_point<std::chrono::steady_clock> start;
	while (!quit)
	{
		start = std::chrono::steady_clock::now();
		glm::vec3 rotAxis(0.0f);
		float rotAng = 0.0f;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_EVENT_QUIT: quit = true; break;
			case SDL_EVENT_KEY_DOWN:
			{
				switch (e.key.key)
				{
				case SDLK_UP:    rotAxis += glm::vec3( 1.0f,  0.0f, 0.0f); rotAng = 0.2f; break;
				case SDLK_DOWN:  rotAxis += glm::vec3(-1.0f,  0.0f, 0.0f); rotAng = 0.2f; break;
				case SDLK_RIGHT: rotAxis += glm::vec3( 0.0f,  1.0f, 0.0f); rotAng = 0.2f; break;
				case SDLK_LEFT:  rotAxis += glm::vec3( 0.0f, -1.0f, 0.0f); rotAng = 0.2f; break;
				}
			}
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		dt.updateTexture(rotAxis, rotAng);
		dt.uploadTexture(tex);
		glBindVertexArray(vao);
		glUseProgram(program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		//glUniform1i(uniTex, 0);
		glUniformMatrix4fv(uniProj, 1, GL_FALSE,
			glm::value_ptr(proj));
		glUniformMatrix4fv(uniView, 1, GL_FALSE,
			glm::value_ptr(camera));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE,
			glm::value_ptr(dt.getModel()));
		glDrawArrays(GL_TRIANGLES, 0, verts);

		SDL_GL_SwapWindow(window);
		size_t ticks = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		std::cout << "ticks: " << ticks << " ms\n";
	}

	//DESTRUCTION
	closeShaders();
	closeBuffers();
	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

std::string readFile(std::string file)
{
	std::string data;
	std::string line;
	std::ifstream input(file);
	if (input.is_open())
	{
		while (std::getline(input, line))
		{
			data += line + '\n';
		}
	}
	return data;
}

void buildShaders()
{
	std::string shader = readFile("vert.txt");
	const char* tempVert = shader.c_str();
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &tempVert, NULL);

	glCompileShader(vert);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shader = readFile("frag.txt");
	const char* tempFrag = shader.c_str();
	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &tempFrag, NULL);

	glCompileShader(frag);
	// check for shader compile errors
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "linking failed: " << infoLog << std::endl;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);

	glUseProgram(0);
	uniProj = glGetUniformLocation(program, "proj");
	uniView = glGetUniformLocation(program, "view");
	uniModel = glGetUniformLocation(program, "model");
	uniTex = glGetUniformLocation(program, "tex");
}

void closeShaders()
{
	glDeleteProgram(program);
	program = 0;
}

void buildBuffers()
{
	std::vector<float> mesh = {
		-1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};
	verts = 6;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), &mesh[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(4 * 3 * verts));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "vao errs present: " << err << '\n';
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "texture errs present: " << err << '\n';
	}
}

void closeBuffers()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	vao = 0;
	vbo = 0;
	glDeleteTextures(1, &tex);
	tex = 0;
}

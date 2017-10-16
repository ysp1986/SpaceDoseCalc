#pragma once
#include <fstream>
#include <algorithm>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#ifdef _DEBUG
#define GLCHECK  glCheckError_(__FILE__, __LINE__);
#else
#define GLCHECK
#endif

class GlobalFuncs
{
public:
	/*
	Load texture from file named as filename. return the idx in textures vector
	*/
	static unsigned int LoadTexture(const char * filename, GLuint *width = nullptr, GLuint *height = nullptr);

	static std::wstring ChDirToExePath();

	static std::string SelectSavePath(const TCHAR * titleName, const TCHAR *filter);
};

GLenum glCheckError_(const char *file, int line);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const void *userParam);


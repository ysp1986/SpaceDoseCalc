#pragma once
#include "Mesh.h"
#include "Shader.h"

class WelcomeUI
{
public:
	WelcomeUI();
	~WelcomeUI();

	Mesh myScreenPlaner;
	GLuint myWelcomeImg;
	GLuint  myMaxWidth;
	GLuint myMaxHeight;
	Shader myShader;

	bool Init();
	void Draw(GLFWwindow *wnd);
};


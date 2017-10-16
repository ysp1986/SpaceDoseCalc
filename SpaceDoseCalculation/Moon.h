#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"


class Moon
{
public:

	Moon::Moon():myInited(false), myRevPeridPassed(0.0), myRotPeridPassed(0.0)
	{
	}


	Moon::~Moon()
	{
	}

	bool myInited;
	Mesh myMesh;
	glm::vec3 myPos;
	glm::mat4 myModelMat;
	Shader myShader;
	double myRevPeridPassed;
	double myRotPeridPassed;

	const double RADIUS_MOON = 4;

	bool Initialize();
	 
	void TimePass(double dayCount);
	void SetMat(glm::vec3 earthPos = glm::vec3(0.0f, 0.0f, 0.0f));

	void SetCam(Camera &cam);

	void Draw();
};


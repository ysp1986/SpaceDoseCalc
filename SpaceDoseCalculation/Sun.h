#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Sun
{
public:
	Sun();
	~Sun();


	bool myInited;
	Mesh myMesh;
	glm::vec3 myPos;
	glm::mat4 myModelMat;
	Shader myShader;
	double myRevPeridPassed;
	double myRotPeridPassed;

	const double RADIUS_SUN = 20;// 69.55;

	bool Initialize();

	void TimePass(double dayCount);

	void SetMat(glm::vec3 galaxyPos = glm::vec3(0.0f, 0.0f, 0.0f));

	void SetCam(Camera &cam);

	void Draw();
};


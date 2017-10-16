#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Earth
{
public:
	Earth():myInited(false), myRevPeridPassed(0.0), myRotPeridPassed(0.0) {
	}
	~Earth() {
	}

	bool myInited;
	Mesh myMesh;
	glm::vec3 myPos;
	Shader myShader;


	float myRevAngle;
	float myRotAngle;
	double myRevPeridPassed;
	double myRotPeridPassed;
	float myCloudSpeed;

	glm::mat4 myModelMat;
	

	const float RADIUS_EARTH = 6.3781f;
	const float ANGLE_TO_ZY_RADIAN = 0.40f;

	bool Initialize();

	void TimePass(double dayCount);

	void SetMat(glm::vec3 sunPos = glm::vec3(0.0f,0.0f,0.0f));

	void SetCam(Camera &cam);

	void Draw();
};


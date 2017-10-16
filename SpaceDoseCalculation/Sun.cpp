#include "stdafx.h"
#include "Sun.h"
#include "GlobalFuncs.h"
#include "SolarSys.h"

Sun::Sun():myInited(false), myRevPeridPassed(0.0), myRotPeridPassed(0.0)
{
}


Sun::~Sun()
{
}

bool Sun::Initialize() {
	myShader.LoadShader("Media\\shaders\\sun.vert", "Media\\shaders\\sun.frag");
	myMesh.MakeSphere(SolarSys::SCALE_OTHER_STAR*RADIUS_SUN,true, 10, 20);
	if (!myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\low_sun.jpg")))
		return false;
	myInited = true;
	return true;
}

void Sun::TimePass(double dayCount) {
	myRotPeridPassed += dayCount / 24.47;
	myRotPeridPassed -= floor(myRotPeridPassed);
}

void Sun::SetMat(glm::vec3 glaxyPos) {
	//rotate 
	glm::mat4 sunmat;
	double radRot = myRotPeridPassed * glm::pi<float>() * 2;
	sunmat = glm::rotate<float>(sunmat,radRot,glm::vec3(0.0f,0.0f,1.0f));
	myModelMat = sunmat;
	myShader.use();
	myShader.setMat4("modelTrans", myModelMat);
}

void Sun::SetCam(Camera &cam) {
	//set view related to cam
	myShader.use();
	myShader.setVec3("viewPos", cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);
	myShader.setMat4("perspectTrans", cam.GetPerspectMat());
	myShader.setMat4("viewTrans", cam.GetViewMat());
}

void Sun::Draw() {
	myShader.use();
	myMesh.Draw(myShader);
}
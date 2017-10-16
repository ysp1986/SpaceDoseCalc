#include "stdafx.h"
#include "Moon.h"
#include "GlobalFuncs.h"
#include "SolarSys.h"

bool Moon::Initialize() {
	myShader.LoadShader("Media\\shaders\\moon.vert", "Media\\shaders\\moon.frag");

	myShader.use();
	myShader.setVec3("light.position", 0.0f, 0.0f, 0.0f);
	myShader.setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
	myShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
	myShader.setVec3("light.specular", 0.9f, 0.9f, 0.9f);
	myShader.setFloat("material.shininess", 30.0f);

	myMesh.MakeSphere(SolarSys::SCALE_OTHER_STAR*RADIUS_MOON, true, 10, 20);
	if (!myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\low_mercury.jpg")))
		return false;
	myInited = true;
	return true;
}
void Moon::TimePass(double dayCount) {
	myRevPeridPassed += dayCount / 27.5;
	myRevPeridPassed -= floor(myRevPeridPassed);
	myRotPeridPassed = myRevPeridPassed;
}

void Moon::SetMat(glm::vec3 earthPos) {
	//the position of the moon
	float radius = SolarSys::DIS_EARTH_MOON;
	float radRevAngle = myRevPeridPassed * glm::pi<float>() * 2;
	myPos = glm::vec3(sin(radRevAngle)*radius, cos(radRevAngle)*radius, 0.0f);
	//5.14deg, 0.008971rad,Mean inclination	of orbit to ecliptic
	glm::mat4 moonMat;
	myPos = glm::vec3(glm::rotate(moonMat, 0.008971f, glm::vec3(1.0f, 0.0f, 0.0f))*glm::vec4(myPos, 1.0));
	myPos = myPos + earthPos;
	moonMat = glm::translate(moonMat, myPos);
	//the rotation of the moon
	//rotate alone the x axis 6.68degree, 0.11659 rad
	moonMat = glm::rotate(moonMat, 0.11659f, glm::vec3(1.0f, 0.0f, 0.0f));
	//rotate alone the z axis
	float radRotAngle = myRotPeridPassed*glm::pi<float>() * 2;
	moonMat = glm::rotate(moonMat, radRotAngle, glm::vec3(0.0f, 0.0f, 1.0f));
	myShader.use();
	myModelMat = moonMat;
	myShader.setMat4("modelTrans", myModelMat);
}

void Moon::SetCam(Camera &cam) {
	//set view related to cam
	myShader.use();
	myShader.setVec3("viewPos", cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);
	myShader.setMat4("perspectTrans", cam.GetPerspectMat());
	myShader.setMat4("viewTrans", cam.GetViewMat());
}

void Moon::Draw() {
	myShader.use();
	myMesh.Draw(myShader);
}
#include "stdafx.h"
#include "Earth.h"
#include "GlobalFuncs.h"
#include "SolarSys.h"

bool Earth::Initialize() {
	float radius = RADIUS_EARTH;
	float angleToZY = ANGLE_TO_ZY_RADIAN;

	//make a mesh
	myMesh.MakeSphere(radius, true, 30, 30);

	GlobalFuncs::ChDirToExePath();
	//load shader
	myShader.LoadShader("Media\\shaders\\earth.vert", "Media\\shaders\\earth1.frag");

	myShader.use();
	myShader.setVec3("light.position", 0.0f, 0.0f, 0.0f);
	myShader.setVec3("light.ambient", 0.0f, 0.0f, 0.0f);
	myShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
	myShader.setVec3("light.specular", 0.9f, 0.9f, 0.9f);
	myShader.setFloat("material.shininess", 30.0f);

	//load textures
	myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\low_earth_daymap.jpg"), "diffuse");
	myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\low_earth_nightmap.jpg"), "diffuseNight");
	myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\low_earth_normal_map.jpg"), "normal");
	myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\low_earth_specular_map.jpg"), "specular");

	myMesh.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\earth_clouds.jpg"), "diffuseCloud");


	//other
	myCloudSpeed = 0.06; //about 40 km/h
	myRevAngle = 0.0;
	myRotAngle = 0.0;
	myRevPeridPassed = 0.0f;
	myRotPeridPassed = 0.0f;
	return true;
}

void Earth::TimePass(double dayCount) {
	myRevPeridPassed += dayCount / 365.0f;
	myRevPeridPassed -= floor(myRevPeridPassed);
	myRotPeridPassed += dayCount;
	myRotPeridPassed -= floor(myRotPeridPassed);
}

void Earth::SetMat(glm::vec3 sunPos) {
	myRevAngle = myRevPeridPassed * glm::pi<float>() * 2;
	myRotAngle = myRotPeridPassed * glm::pi<float>() * 2;
	myModelMat = glm::mat4();
	//put to position
	myModelMat = glm::rotate(myModelMat, myRevAngle, glm::vec3(0.0f, 0.0f, 1.0f));
	myModelMat = glm::translate(myModelMat, glm::vec3(SolarSys::DIS_EARTH_SUN, 0.0, 0.0));
	//with angle to the yz plane
	myModelMat = glm::rotate(myModelMat, ANGLE_TO_ZY_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	//rotate to its axis
	myModelMat = glm::rotate(myModelMat, myRotAngle, glm::vec3(0.0f, 0.0f, 1.0f));
	myPos = myModelMat * glm::vec4( glm::vec3(0.0f, 0.0f, 0.0f),1.0);
	myShader.use();
	myShader.setMat4("modelTrans", myModelMat);
	myShader.setFloat("cloudMove", myCloudSpeed*myRotAngle);

}

void Earth::SetCam(Camera &cam) {
	myShader.use();
	//set view related to cam
	myShader.setVec3("viewPos", cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);
	myShader.setMat4("perspectTrans", cam.GetPerspectMat());
	myShader.setMat4("viewTrans", cam.GetViewMat());
}

void Earth::Draw() {
	/*myShader.use();
	myModelMat = glm::translate(myModelMat, glm::vec3(0, 0.0f, 2.0f));
	myShader.setMat4("modelTrans", myModelMat);
	myMesh.Draw(myShader);

	myShader.use();
	myModelMat = glm::translate(myModelMat, glm::vec3(0, 0.0f, -4.0f));
	myShader.setMat4("modelTrans", myModelMat);
	myMesh.Draw(myShader);
	myModelMat = glm::translate(myModelMat, glm::vec3(0, 0.0f, 2.0f));*/
	myShader.use();
	myMesh.Draw(myShader);
}
#include "stdafx.h"
#include "SolarSys.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "GlobalFuncs.h"

double SolarSys::DIS_EARTH_SUN = 1496;
double SolarSys::DIS_EARTH_MOON = 385;
double SolarSys::SCALE_OTHER_STAR = 1;
double SolarSys::SCALE_SATTELITE = 10000;

SolarSys::SolarSys():initState(un_inited)
{
}


SolarSys::~SolarSys()
{
}

bool SolarSys::Initialize() {
	initState = init_earth;
	myEarth.Initialize();
	initState = init_moon;
	myMoon.Initialize();
	initState = init_sun;
	mySun.Initialize();

	galaxyBackground.MakeSphere(2000, false, 30, 30);
	galaxyBackground.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\stars_milky_way.jpg"));
	backgroundShader.LoadShader("Media\\shaders\\background.vert", "Media\\shaders\\background.frag");
	initState = finished;

	return true;
}

void SolarSys::SetMats(double dayCount) {


	myEarth.TimePass(dayCount); 
	myEarth.SetMat(mySun.myPos); //because cam move with earth, so this gonna to be called first
	mySun.TimePass(dayCount);
	mySun.SetMat();
	myMoon.TimePass(dayCount);
	myMoon.SetMat(myEarth.myPos);

	//set cam according to earth
	camera.cameraPos = glm::vec3(myEarth.myModelMat*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	camera.cameraPos = glm::vec3(glm::sin(myEarth.myRevAngle * 10) * 50, glm::cos(myEarth.myRevAngle * 10) * 50, 5.0f) + camera.cameraPos;
	camera.cameraFront = glm::normalize(glm::vec3(myEarth.myPos.x - camera.cameraPos.x
		, myEarth.myPos.y - camera.cameraPos.y 
		,0));

	/*cam.cameraPos = glm::vec3(myEarth.myModelMat*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	cam.cameraPos = glm::vec3(glm::sin(myEarth.myRevAngle * 30) * 100, glm::cos(myEarth.myRevAngle * 30) * 100, 0.0f) + cam.cameraPos;
	cam.cameraFront = myMoon.myPos - cam.cameraPos;
*/
	/*cam.cameraPos = glm::vec3(0.0f,0.0f,0.0f);
	cam.cameraPos = glm::vec3(glm::sin(myEarth.myRevAngle * 10) * 1600, glm::cos(myEarth.myRevAngle * 10) * 1600, 0.0f) + cam.cameraPos;
	cam.cameraFront = glm::vec3(-glm::sin(myEarth.myRevAngle * 10), -glm::cos(myEarth.myRevAngle * 10), 0.0f);*/

	mySun.SetCam(camera);
	myMoon.SetCam(camera);
	myEarth.SetCam(camera);

	backgroundShader.use();
	backgroundShader.setMat4("perspectTrans", camera.GetPerspectMat());
	backgroundShader.setMat4("viewTrans", camera.GetBackgroundViewMat());
	backgroundShader.setMat4("modelTrans", glm::mat4());
	
}

void SolarSys::Draw() {
	backgroundShader.use();
	galaxyBackground.Draw(backgroundShader);
	mySun.Draw();
	myEarth.Draw();
	myMoon.Draw();
}
#include "stdafx.h"
#include "WelcomeUI.h"
#include "GlobalFuncs.h"
#include "imgui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

WelcomeUI::WelcomeUI()
{

}

bool WelcomeUI::Init() {
	GlobalFuncs::ChDirToExePath(); std::cout << __FILE__ << __LINE__ << std::endl;
	myWelcomeImg = GlobalFuncs::LoadTexture("Media\\UI\\back_ground_logo2.png", &myMaxWidth, &myMaxHeight); std::cout << __FILE__ << __LINE__ << std::endl;
	myScreenPlaner.MakeScreenPlanar1(myWelcomeImg); std::cout << __FILE__ << __LINE__ << std::endl;
	myShader.LoadShader("Media\\shaders\\simple.vert", "Media\\shaders\\simple.frag"); std::cout << __FILE__ << __LINE__ << std::endl;
	glm::mat4 viewTrans;
	glm::mat4 persTrans;
	glm::mat4 modelTrans;
	myShader.use();
	myShader.setMat4("modelTrans", modelTrans);
	myShader.setMat4("viewTrans", viewTrans);
	myShader.setMat4("perspectTrans", persTrans);
	return true;
}


WelcomeUI::~WelcomeUI()
{
}

void WelcomeUI::Draw(GLFWwindow *wnd) {
	//calculate the transformation
	if (!wnd)
		return;
	int height, width;
	glfwGetWindowSize(wnd, &width, &height);

	double heightRate = height*1.0 / myMaxHeight;
	double widthRate = width*1.0 / myMaxWidth;
	int displayHeight, displayWidth;
	double properRate = 1.0;
	if (heightRate > widthRate)
		properRate = widthRate > 1.0 ? 1.0 : widthRate;
	else 
		properRate = heightRate > 1.0 ? 1.0 : heightRate;
	displayWidth = myMaxWidth * properRate;
	displayHeight = myMaxHeight * properRate;
	double transScalex = displayWidth*1.0 / width;
	double transScaley = displayHeight*1.0 / height;
	glm::mat4 modelTrans;

	modelTrans = glm::scale(modelTrans,glm::vec3(transScalex, transScaley,1.0f));
	myShader.setMat4("modelTrans", modelTrans);
	myScreenPlaner.Draw(myShader);
}

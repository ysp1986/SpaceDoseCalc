// OpenGLDemo.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "Shader.h"
#include "Camera.h"
#include "GlobalFuncs.h"
#include "Mesh.h"
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include "DoseUIs.h"
#include "FrameTextureBuffer.h"
#include "Earth.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);




//视角矩阵变换
Camera camera;
#include <thread>
void MainGUI(MainUIs &myUI);
int main(int argc, char *argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	MainUIs myUI;
	if (!myUI.Init())
	{
		cout << "本机显卡驱动版本太低，请更新显卡驱动。" << std::endl;
		char a;
		cin >> a;
		return 1;
	}
	//std::thread uiThread(&DoseUIs::ShowUI, std::ref(myUI));
	myUI.ShowUI();
	//loadFontThread.join();
	//uiThread.join();
	myUI.Termiate();
	return 0;
}


//void MainGUI(DoseUIs &myUI)
//{
//
//	// Setup ImGui binding
//	
//	
//	//ImGui_ImplGlfwGL3_Init(window, true);
//	bool show_test_window = true;
//	bool show_another_window = false;
//	ImVec4 clear_color = ImColor(114, 144, 154,40);
//
//	//Mesh galaxyBackground;
//	//galaxyBackground.MakeSphere(2000, false, 30, 30);
//	//galaxyBackground.SetTexture(GlobalFuncs::LoadTexture("Media\\textures\\stars_milky_way.jpg"));
//	//Shader backgroundShader("Media\\shaders\\vertex.shader", "Media\\shaders\\fragment_background.shader");
//	//if (!backgroundShader.success)
//	//	return;// 1;
//
//	//Earth earth;
//	/*SolarSys solarSys;
//	solarSys.Initialize();*/
//	//earth.Initialize();
//	/*Mesh earth;
//	earth.MakeSphere(1, true, 30, 30);
//	earth.SetTexture(GlobalFuncs::LoadTexture("Media\\earth2.jpg"));*/
//
//	
//	
//	//Shader earthShader("Media\\shaders\\vertex.shader","Media\\shaders\\fragment_earth.shader");
//	//if (!earthShader.success)
//	//	return;// 1;
//
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_BLEND);
//	glEnable(GL_CULL_FACE);
//
//	float moveFar = 1.0;
//
//	bool firsttime = true;
//
//	//while (!myUI.Ready()) {
//	//	//draw the welcome picture
//	//	Sleep(20);
//	//}
//	
//	float idx = 0.0f;
//	//FrameTextureBuffer tbuffer(wndWidth, wndHeight,-0.6);
//	//FrameTextureBuffer tbuffer2(wndWidth, wndHeight, 0.6);
//	while (!glfwWindowShouldClose(window))
//	{
//		idx += 0.1f;
//		if(firsttime)
//			std::cout << "Frame update" << std::endl;
//		firsttime = false;
//		glfwPollEvents();
//
//
//		// 输入
//		processInput(window);
//
//		{
//			//tbuffer.BeginRenderToMe();
//			// 渲染指令
//			glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//			//draw background
//			/*backgroundShader.use();
//			backgroundShader.setMat4("perspectTrans", camera.GetPerspectMat());
//			backgroundShader.setMat4("viewTrans", camera.GetBackgroundViewMat());
//			backgroundShader.setMat4("modelTrans", glm::mat4());
//			galaxyBackground.Draw(backgroundShader);*/
//
//			//draw earth
//			/*earthShader.use();
//			earthShader.setMat4("perspectTrans", camera.GetPerspectMat());
//			earthShader.setMat4("viewTrans", camera.GetViewMat());
//			earthShader.setMat4("modelTrans", glm::mat4());
//			earthShader.setVec3("lightDirection", 0.0f, 1.0f, 0.0f);
//			earth.Draw(earthShader);*/
//			//earth.myShader.use();
//			//earth.TimePass(0.01);
//			/*solarSys.SetMats(0.01);
//			solarSys.Draw();*/
//			//earth.SetMat(camera);
//			/*camera.cameraPos = glm::vec3(earth.myModelMat*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
//			camera.cameraPos = glm::vec3(glm::sin(earth.myRevAngle*10)*30,glm::cos(earth.myRevAngle* 10) * 30,0.0f)+camera.cameraPos;
//			camera.cameraFront = glm::vec3(-glm::sin(earth.myRevAngle* 10), -glm::cos(earth.myRevAngle* 10), 0.0f);
//			earth.myShader.setVec3("viewPos", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
//			earth.myShader.setMat4("perspectTrans", camera.GetPerspectMat());
//			earth.myShader.setMat4("viewTrans", camera.GetViewMat());
//			earth.myShader.setVec3("lightDirection", 0.0f, 1.0f, 0.0f);*/
//			//earth.Draw();
//			//tbuffer.EndRenderToMe();
//
//			//tbuffer2.BeginRenderToMe();
//			//glClearColor(0.4f, 0.0f, 0.3f, 0.0f);
//			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//			if (myUI.Ready()) {
//				ImGui_ImplGlfwGL3_NewFrame();
//				myUI.ShowUI();
//				int display_w, display_h;
//				glfwGetFramebufferSize(window, &display_w, &display_h);
//				glViewport(0, 0, display_w, display_h);
//				ImGui::Render();
//			}
//
//			//tbuffer2.EndRenderToMe();
//		}
//		
//		// 检查并调用事件，交换缓冲
//		/*glViewport(0, 0, wndWidth, wndHeight);
//		glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
//		
//		
//		/*tbuffer.DrawToScene();
//		tbuffer2.DrawToScene();*/
//
//		glfwSwapBuffers(window);
//		Sleep(10);
//	}
//	
//
//	return;// 0;
//}

//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//	wndWidth = width;
//	wndHeight = height;
//	camera.ResizeViewPortSize(float(width), float(height));
//}
//
//void processInput(GLFWwindow *window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//	{
//		glfwSetWindowShouldClose(window, true);
//	}
//		
//
//	camera.InputProcess(window);
//
//}
//
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
//		camera.DirectionProcess(window, xpos, ypos);
//	else
//		camera.ReleaseLBtn();
//}
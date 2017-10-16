#pragma once
#include "DoseCalcException.h"
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include<thread>

#include "OMEREController.h"
#include <map>
#include "SolarSys.h"
#include "WelcomeUI.h"
#include "Camera.h"
#include "GlobalFuncs.h"
#include <thread>
#include <mutex>
#include <chrono>
#include "DoseCalculator.h"

//三维场景描述：地球在0，0，0位置，不显示太阳，月球
//地球以真实尺寸定义
//航天器直径设置大一些，以能够观察到

class MainUIs
{
	GLFWwindow *myWindow;             //the gffw window for the main GUI
	GLFWwindow *myThreadSharedWindow; //the glfw window containing the context of the thread for loading all the resources for the background universe
	ImFont *myFont;          //the font for chinese characters
	ImColor myInputEdtColor; //the color for the input control with valid value
	ImColor myOutputEdtColor; 
	ImColor myErrorEdtColor; //the color for the control(mainly the number input) that has an invalid value
	bool myInited;       //signal, if this class is initialized to the point that can be seen(still the background, the data has not yet been loaded, the thread for loading them has just been started)
	bool myFontLoaded;   //signal, if the font is already loaded, this would be true
	bool myResourceLoadFinished;   //signal, if all the neccessary resource has been loaded

	SolarSys mySolarSys; //the background scene (with a sun, a moon, a intricit earth
	WelcomeUI myWelcomeUI; //the first UI, with only a picture on it

	DoseCalculator myCalculationCore; //the calculation core for dose calculation
	enum { UI_PARAMETERS, UI_WAITING_CALCULATION, UI_RESULTS } myCurUI; //the label of the current stage, for choosing UI to display

	//resources for the result UI
	GLuint myResultManTexture;
	GLuint myResultWomanTexture;
	GLuint sMWTexW;
	GLuint sMWTexH;

	map<string, bool> myDetailShowFlag;
	map<string, double> myDoseLimit;

	static map<GLFWwindow*, MainUIs*> sWindowUIMap; //currently no use

	static string sMessage;           //global variable, for displaying the most urgent message to the left corner of the window
	static std::mutex myLock;

protected:
	//these four functions are called in the 
	std::map<string, char *> sUITextBufs;
	bool InputValidDouble(const char *label, double &val, double min=0.0,double max=1e30);

	bool myShowTrajectoriesUI;

	static int wndWidth;
	static int wndHeight;
	std::thread myThreadLoadResource;
	std::thread myThreadCalculation;
public:
	MainUIs():myInputEdtColor()
	, myOutputEdtColor()
	, myErrorEdtColor(), myInited(false), myFontLoaded(false),myCurUI(UI_PARAMETERS)
	, sMWTexW(454)
	, sMWTexH(943)
	, myShowTrajectoriesUI(true){
	}
	~MainUIs() {
		Termiate();
	}

	bool Ready();

	bool Init();

	void Termiate();

	void LoadResources();

	bool LoadDoseLimit();

	void ShowUI();

	void DrawMessage();

	void WelcomePage();

	void ParameterPage();

	void CalculationWaitingPage();

	void ResultsPage();

	void DetailsPage(string gender, string organ ="", double totalDose=0.0);

	void OuputResults(string outputFile = "output.txt");

	static void SetMessage(const char *msg) {
		myLock.lock();
		sMessage = msg;
		myLock.unlock();
	}
	static string GetMessage() {
		string ret;
		myLock.lock();
		ret = sMessage;
		myLock.unlock();
		return ret;
	}



	static MainUIs* GetUIFromWindow(GLFWwindow* window) {
		if (sWindowUIMap.find(window) != sWindowUIMap.end()) {
			return sWindowUIMap[window];
		}
		return nullptr;
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static void processInput(GLFWwindow *window);

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	static void gl_error_callback(int code, const char* info);

	static string DoseToString(double dose);
	static string PercentToString(double percent);
	static wstring ChDirToCurrentExe() {
		HMODULE mHandler = GetModuleHandle(NULL);
		TCHAR exeFilePath[256];
		GetModuleFileName(mHandler, exeFilePath, 256);
		wstring path = exeFilePath;
		path = path.substr(0, path.rfind(L'\\'));
		_wchdir(path.data());
		return path;
	}
};


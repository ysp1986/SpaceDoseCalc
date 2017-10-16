// ForTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include "DualColData.h"
#include "FluxBasedCalculationCore.h"
using namespace std;

class A {
public:
	A(int i) {
		cout << "initializing :" << i << endl;
		myI = i;
	}
	A(const A &otherA) {
		
		myI = otherA.myI;
		cout << "initializing :" << myI << endl;
	}
	int myI;
	
};

ostream &operator <<(ostream &os, A &a) {
	os << a.myI << endl;
	return os;
}

void TestCalculation() {
	//step1: call OMERE to get the result.
	//OMEREController a;
	//a.SetOrbitParametersToOMERE();
	//a.CalculateTRP();

	//step2: calculate the doses and output them for testing
	vector<double> male_TRP;
	vector<double> female_TRP;
	vector<double> male_GCR;
	vector<double> male_SPE;
	vector<double> female_GCR;
	vector<double> female_SPE;

	vector<double> ints({ -1.0,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1,0.09,0.08,0.07,0.06,0.05,0.04,0.03,0.02,0.01,0.009,0.007,0.006,0.005,0.004,0.003,0.002,0.001,1e-4,1e-5,3e-6, 2e-6, 1e-6 ,8e-7,6e-7,5e-7 });

	DoseCalculator calculator;
	calculator.LoadAllData();

	ofstream outStream("test.log");
	DualColData &TRPData = calculator.myTRPDatas[0];
	DualColData &DoseTransData = calculator.myDoseTransParas[0];
	for (auto intval : ints) {
		double start = clock()*1.0 / CLOCKS_PER_SEC;
		double dose = FluxBasedCalculationCore::CalculateDose(intval, TRPData, DoseTransData);
		double end = clock()*1.0 / CLOCKS_PER_SEC;
		outStream << intval << "\t" << dose << "\t" << end - start << endl;
		cout << intval << "\t" << dose << "\t" << end - start << endl;
	}

	for (auto intval : ints) {
		double start = clock()*1.0 / CLOCKS_PER_SEC;
		calculator.myDosTranExpandInt = intval;
		calculator.ClearAllResult();
		calculator.CalcDoses();
		male_TRP.push_back(calculator.AccumulateDose("male", "", "TRP"));
		female_TRP.push_back(calculator.AccumulateDose("female", "", "TRP"));
		male_GCR.push_back(calculator.AccumulateDose("male", "", "GCR-MIN"));
		female_GCR.push_back(calculator.AccumulateDose("female", "", "GCR-MIN"));
		male_SPE.push_back(calculator.AccumulateDose("male", "", "SPE-SPE72"));
		female_SPE.push_back(calculator.AccumulateDose("female", "", "SPE-SPE72"));
		outStream << intval << "\t" << male_TRP.back() << "\t" << female_TRP.back()
			<< "\t" << male_GCR.back() << "\t" << female_GCR.back()
			<< "\t" << male_SPE.back() << "\t" << female_SPE.back();
		double end = clock()*1.0 / CLOCKS_PER_SEC;
		cout << intval << "\t" << end - start << endl;
		outStream << "\ttimeused:\t" << end - start << endl;
	}

	return 0;
}

#include "OMEREController.h"
#include "DoseCalculator.h"
int main()
{
	myResourceLoadFinished = false;
	glfwSetErrorCallback(DoseUIs::gl_error_callback);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	mySolarSys.camera.ResizeViewPortSize(mode->width, mode->height);
	myWindow = glfwCreateWindow(mode->width, mode->height, "Space Dose Calculation", monitor, NULL);
	/*myWindow = glfwCreateWindow(wndWidth, wndHeight, "Space Dose Calculation", NULL, NULL);*/

	glfwWindowHint(GLFW_VISIBLE, false);
	myThreadSharedWindow = glfwCreateWindow(1, 1, "loadwnd", nullptr, myWindow);

	if (myWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(myWindow);
	//wglCreateContext();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	glfwSetFramebufferSizeCallback(myWindow, DoseUIs::framebuffer_size_callback);
	glfwSetCursorPosCallback(myWindow, DoseUIs::mouse_callback);
	ImGui_ImplGlfwGL3_Init(myWindow, true);



	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	glfwMakeContextCurrent(myThreadSharedWindow);
	glfwTerminate();
}


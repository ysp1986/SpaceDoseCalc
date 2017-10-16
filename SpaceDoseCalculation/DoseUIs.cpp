#include "stdafx.h"
#include "DoseUIs.h"
#include "GlobalFuncs.h"
#include "NumberChecker.h"
#include <iomanip>
#include <windows.h>
#include <CommDlg.h>
#include <GLFW/glfw3native.h>
#include "imgui_internal.h"
int MainUIs::wndWidth = 1390;
int MainUIs::wndHeight = 960;
string  MainUIs::sMessage("");
std::mutex MainUIs::myLock;
map<GLFWwindow*, MainUIs*> MainUIs::sWindowUIMap;

#define FONT_SIZE 25.0f


static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


void MainUIs::WelcomePage() {
	myWelcomeUI.Draw(myWindow);

}

bool MainUIs::Ready() {
	return myResourceLoadFinished;
}

bool MainUIs::Init() {
	myResourceLoadFinished = false;
	glfwSetErrorCallback(MainUIs::gl_error_callback);
	static bool fullScreenMode = false;

	if (fullScreenMode) {

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		myWindow = glfwCreateWindow(mode->width, mode->height, "Space Dose Calculation", monitor, NULL);
		if (!myWindow)
			return false;
	}
	else {
		myWindow = glfwCreateWindow(wndWidth, wndHeight, "Space Dose Calculation", NULL, NULL);
		if (!myWindow)
			return false;
		HWND wndHandle = glfwGetWin32Window(myWindow);
		if (!wndHandle)
			return false;
		SendMessage(wndHandle, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	

	
	//the sub invisible window for loading resources
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
	glfwSetFramebufferSizeCallback(myWindow, MainUIs::framebuffer_size_callback);
	glfwSetCursorPosCallback(myWindow, MainUIs::mouse_callback);

	ImGui_ImplGlfwGL3_Init(myWindow, true);

	ImGuiIO& io = ImGui::GetIO();
	std::cout << "DoseUIfs initialized." << std::endl;
	
	sWindowUIMap[myWindow] = this;
	myWelcomeUI.Init();

	myThreadLoadResource = std::thread(&MainUIs::LoadResources, std::ref(*this));
	myInited = true;
	myCurUI = UI_PARAMETERS;

	//set the them color

	ImGuiStyle &style = ImGui::GetCurrentContext()->Style;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	return true;
}



void MainUIs::LoadResources() {
	SetMessage(u8"载入字体...");
	glfwMakeContextCurrent(myThreadSharedWindow);
	myFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(u8"Media\\extra_fonts\\微软雅黑繁简完全版.ttf",
		FONT_SIZE, NULL, ImGui::GetIO().Fonts->GetGlyphRangesChinese());
	
	if (!myFont)
		myFontLoaded = false;
	else {
		ImGui::GetIO().Fonts->Build();
	}
	std::cout << "Font intialized." << std::endl;
	myFontLoaded = true;
	SetMessage(u8"载入场景...");
	mySolarSys.Initialize();
	GlobalFuncs::ChDirToExePath();
	myResultManTexture = GlobalFuncs::LoadTexture("Media\\UI\\ma.png", &sMWTexW, &sMWTexH);
	myResultWomanTexture = GlobalFuncs::LoadTexture("Media\\UI\\wa.png");
	std::cout << "Scene loaded." << std::endl;
	SetMessage(u8"载入数据...");
	myCalculationCore.myMsgReportor = MainUIs::SetMessage;
	myCalculationCore.LoadAllButTRP();
	myResourceLoadFinished = true;
	
	glfwMakeContextCurrent(myWindow);
	glfwSetWindowShouldClose(myThreadSharedWindow, true);
	SetMessage("");
	LoadDoseLimit();
}

bool MainUIs::LoadDoseLimit() {
	GlobalFuncs::ChDirToExePath();
	ifstream doseStrm("Media\\dose_data\\dose_limit.txt");
	if (!doseStrm.is_open() || !doseStrm.good())
		return false;
	try {
		string line;
		getline(doseStrm, line);
		while (getline(doseStrm, line), doseStrm.good())
		{
			vector<string> keys(SPLIT_SPACE(line));
			string &gender = keys[0];
			string &organ = keys[1];
			string &limit = keys[2];
			if (NumberChecker::isNumber(limit)) {
				myDoseLimit[gender + "_" + organ] = atof(limit.data());
			}
		}
	}
	catch (ifstream::failure e) {
		cout << "Error reading dose limit: " << e.what() << endl;
	}
	catch (string &errmsg) {
		cout << "Error reading dose limit: " << errmsg << endl;
	}
	catch (...) {
		cout << "Something reading dose limit" << endl;
	}
	return true;
}

#include "MultisampleFrameBuffer.h"

void MainUIs::Termiate() {
	if (myThreadLoadResource.joinable())
		myThreadLoadResource.join();
	if (myThreadCalculation.joinable())
		myThreadCalculation.join();
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	glfwMakeContextCurrent(myThreadSharedWindow);
	glfwTerminate();
}

void MainUIs::ShowUI() {
	bool show_test_window = true;
	bool show_another_window = false;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	float moveFar = 1.0;

	bool firsttime = true;

	float idx = 0.0f;
	int display_w, display_h;
	glfwGetFramebufferSize(myWindow, &display_w, &display_h);

	MultisampleFrameBuffer msaaFrameBuffer(4, display_w, display_h);
	while (!glfwWindowShouldClose(myWindow))
	{
		idx += 0.1f;
		if (firsttime)
			std::cout << "Frame update" << std::endl;
		firsttime = false;
		glfwPollEvents();
		// 输入
		//processInput(myWindow);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static int i = 0;
		glfwGetFramebufferSize(myWindow, &display_w, &display_h);
		msaaFrameBuffer.Resize(display_w, display_h);
		if (Ready())
		{

			msaaFrameBuffer.BeginRenderToMe();
			mySolarSys.camera.ResizeViewPortSize(display_w, display_h);
			//draw background first
			mySolarSys.SetMats(0.01);
			mySolarSys.Draw();
			msaaFrameBuffer.EndRenderToMe();
			msaaFrameBuffer.DrawToScene();
			//then draw UI
			ImGui_ImplGlfwGL3_NewFrame();

			if (UI_PARAMETERS == myCurUI)
				ParameterPage();
			else if (UI_WAITING_CALCULATION == myCurUI)
				CalculationWaitingPage();
			else if (UI_RESULTS == myCurUI)
				ResultsPage();
			glViewport(0, 0, display_w, display_h);
			if (i % 100 == 0)
				cout << display_w << ":" << display_h << endl;

			DrawMessage();
			ImGui::Render();
			i++;

		}
		else if (myFontLoaded) {
			WelcomePage();
			ImGui_ImplGlfwGL3_NewFrame();
			DrawMessage();
			
			glViewport(0, 0, display_w, display_h);
			ImGui::Render();
		}
		else{
			WelcomePage();
			glViewport(0, 0, display_w, display_h);
		}

		glfwSwapBuffers(myWindow);
		Sleep(10);

	}
}

void MainUIs::DrawMessage() {
	int height, width;
	glfwGetWindowSize(myWindow, &width, &height);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
	//draw message to the left bottom corner:
	ImGui::SetNextWindowPos(ImVec2(0, height-2.3*FONT_SIZE), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(width, 2.3*FONT_SIZE), ImGuiSetCond_Always);
	ImGui::Begin("message_container", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::Text(MainUIs::sMessage.c_str());
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void MainUIs::ParameterPage() {
	int height, width;
	width = ImGui::GetIO().DisplaySize.x;
	height = ImGui::GetIO().DisplaySize.y;
	int wndWidth = width / 4;
	wndWidth = wndWidth > 28*FONT_SIZE ? wndWidth : 28 * FONT_SIZE;
	ImGui::SetNextWindowPos(ImVec2(width - wndWidth, 0), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(wndWidth, height), ImGuiSetCond_Always);
	if (ImGui::Begin("container", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
		ImGui::Text(u8"参数设置");

		if (ImGui::CollapsingHeader(u8"轨道、飞行器参数", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
			//input

			string orbitFilePath;//for the orbits "C:\OMERE\orbit.dat"*/
			MainUIs::InputValidDouble(u8"远地点(km)", myCalculationCore.myOMERE.apogee);
			MainUIs::InputValidDouble(u8"近地点(km)", myCalculationCore.myOMERE.perigee);
			MainUIs::InputValidDouble(u8"轨道倾角(°)", myCalculationCore.myOMERE.inclination);
			MainUIs::InputValidDouble(u8"近地点幅角(°)", myCalculationCore.myOMERE.perigeArgumant);
			MainUIs::InputValidDouble(u8"升交点经度(°)", myCalculationCore.myOMERE.longtitudeofAscendingNode);
			//MainUIs::InputValidDouble(u8"升交点赤经(km)", myCalculationCore.myOMERE.RAAN);
			MainUIs::InputValidDouble(u8"真近地角(°)", myCalculationCore.myOMERE.trueAnormaly);

			myCalculationCore.myOMERE.numberofOrbit = 10;
			myCalculationCore.myOMERE.numberofPointsPerOrbit = 10;
			myCalculationCore.myOMERE.segmentDuration = 100;

			vector<const char *> items;
			for (auto &shieldOpt : myCalculationCore.myShiendOpts) {
				items.push_back(shieldOpt.data());
			}
			ImGui::Combo(u8"屏蔽厚度", &(myCalculationCore.myCurShieldTransParaIdx), items.data(), items.size());
		}

		if (ImGui::CollapsingHeader(u8"输入地磁捕获辐射TRP相关参数", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
			MainUIs::InputValidDouble(u8"近地任务时间(天)", myCalculationCore.myDurationNearEarth);
		}

		if (ImGui::CollapsingHeader(u8"输入银河宇宙射线GCR相关参数", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
			MainUIs::InputValidDouble(u8"星际任务时间(天)", myCalculationCore.myDurationInterstellar);
			//最大最小年
			vector<const char *> items;
			for (auto &gcrOpt : myCalculationCore.myGCROpts) {
				items.push_back(gcrOpt.data());
			}
			ImGui::Combo(u8"银河宇宙射线大小年", &(myCalculationCore.myCurGCRIdx), items.data(), items.size());
		}
		if (ImGui::CollapsingHeader(u8"选择太阳粒子事件SPE", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
			//select the number of all the SPE accident
			for (auto &a : myCalculationCore.mySPEDatas) {
				string name = a.myName.substr(0, a.myName.size() - 7);
				string showname = u8"(" + name + u8")次数";
				//set number of each SPE accident
				ImGui::InputInt(showname.c_str(), &(myCalculationCore.mySPECounts[name]));
				ImGui::SameLine(); ShowHelpMarker(u8"应为正值，代表发生本类太阳粒子事件的次数");
			}
		}

		//calculation button
		if (ImGui::Button(u8"开始计算", ImVec2(wndWidth, 1.5*FONT_SIZE))) {
			myCalculationCore.InitCalculation();
			myThreadCalculation = std::thread(&(DoseCalculator::AllCalc), &myCalculationCore);
			myCurUI = UI_WAITING_CALCULATION;
		}

		//exit button
		if (ImGui::Button(u8"退出", ImVec2(wndWidth, 1.5*FONT_SIZE))) {
			glfwSetWindowShouldClose(myWindow,true);
		}

		ImGui::End();
	}
}

void MainUIs::CalculationWaitingPage() {
	ImGui::OpenPopup(u8"计算进行中");
	if (ImGui::BeginPopupModal(u8"计算进行中", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text(sMessage.data());
		if (ImGui::Button(u8"取消计算", ImVec2(0, 1.5*FONT_SIZE))) {
			myCalculationCore.InterruptCalculation();
			myThreadCalculation.join();
			myCurUI = UI_PARAMETERS;
		}
		if (myCalculationCore.myCalculationFinished) {
			myDetailShowFlag["AllResult"] = true;
			myCurUI = UI_RESULTS;
		}
		ImGui::EndPopup();
	}
	else {
		ImGui::EndPopup();
		myCurUI = UI_PARAMETERS;
	}
}

#include <sstream>

void MainUIs::ResultsPage() {

	if (!myDetailShowFlag["AllResult"]) {
		if (myThreadCalculation.joinable())
			myThreadCalculation.join();
		myCurUI = UI_PARAMETERS;
	}
		
	//show all the results
	//int height, width;
	//width = ImGui::GetIO().DisplaySize.x;
	//height = ImGui::GetIO().DisplaySize.y;
	
	ImGui::SetNextWindowSize(ImVec2(47*FONT_SIZE, 43*FONT_SIZE), ImGuiSetCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Appearing);

	double imgScale = 0.25;
	void *imgText[2] = { (void*)myResultManTexture ,(void*)myResultWomanTexture };
	set<string> *organsets[2] = { &DoseCalculator::sMaleOrgans
		,&DoseCalculator::sFemaleOrgans };
	string genders[2] = { "male","female" };
	string genderInTitle[2] = { u8".男性",u8".女性" };

	int doseWndWidth = ImGui::GetWindowContentRegionMax().x;
	int doseTextWidth = (doseWndWidth - sMWTexW) / 2;
	doseTextWidth = doseTextWidth > 23*FONT_SIZE ? doseTextWidth : 23 * FONT_SIZE;

	if (ImGui::Begin(u8"剂量计算结果", &myDetailShowFlag["AllResult"]))
	{
		for (int gi = 0; gi < 2; ++gi) {
			string imageName = genderInTitle[gi] + "-IMAGE";
			string gender = genders[gi];
			ImGui::BeginChild(imageName.data(), ImVec2(11 * FONT_SIZE/sMWTexH*sMWTexW, 11*FONT_SIZE));
			ImGui::Image(imgText[gi], ImVec2(11 * FONT_SIZE / sMWTexH*sMWTexW, 11 * FONT_SIZE));
			ImGui::EndChild();
			ImGui::SameLine();
			string doseName = genderInTitle[gi] + "-DOSE";
			ImGui::BeginChild(doseName.data(), ImVec2(0, sMWTexH*imgScale), true);
			//dose rates for man's organs and total
			{
				string colName = genderInTitle[gi] + "-DOSE-DETAIL-BUTTON";
				ImGui::Columns(4, colName.data(), false);
				ImGui::Text(u8"器官名"); ImGui::NextColumn();
				ImGui::Text(u8"剂量(Gy)"); ImGui::NextColumn();
				ImGui::Text(u8"限值(Gy)"); ImGui::NextColumn();
				ImGui::Text(u8"详情"); ImGui::NextColumn();
				ImGui::Separator();
				set<string> &organset = *(organsets[gi]);
				for (const string &organ : organset) {
					double dose = myCalculationCore.AccumulateDose(gender.data(), organ.data());
					string detailName = gender + "_" + organ;
					string organChName = DoseCalculator::sOrganDict[organ];
					string dosestr = DoseToString(dose);
					string doselimitstr = myDoseLimit.find(detailName) != myDoseLimit.end() ?
						DoseToString(myDoseLimit[detailName]) : "NA";
					if (myDoseLimit.find(detailName) != myDoseLimit.end()&& dose > myDoseLimit[detailName]) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.4, 0.4, 1));
						ImGui::Text(organChName.data()); ImGui::NextColumn();
						ImGui::Text(dosestr.data()); ImGui::NextColumn();
						ImGui::Text(doselimitstr.data()); ImGui::NextColumn();
						ImGui::PopStyleColor();
					}
					else {
						ImGui::Text(organChName.data()); ImGui::NextColumn();
						ImGui::Text(dosestr.data()); ImGui::NextColumn();
						ImGui::Text(doselimitstr.data()); ImGui::NextColumn();
					}
					
					if (myDetailShowFlag.find(detailName) == myDetailShowFlag.end()) {
						myDetailShowFlag[detailName] = false;
					}
					string btnName = u8"     ...           " + detailName;
					
					if (myDetailShowFlag[detailName]) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2, 0.5, 0.4, 1));
						if (ImGui::Button(btnName.data(), ImVec2(4.6*FONT_SIZE, 1.5*FONT_SIZE)))
							myDetailShowFlag[detailName] = !myDetailShowFlag[detailName];
						ImGui::PopStyleColor();

					}
					else
					{
						if (ImGui::Button(btnName.data(), ImVec2(4.6*FONT_SIZE, 1.5*FONT_SIZE)))
							myDetailShowFlag[detailName] = !myDetailShowFlag[detailName];
					}
					ImGui::NextColumn();
					DetailsPage(gender, organ, dose);
				}
				ImGui::Columns(1);
			}
			ImGui::EndChild();
		}

		if (ImGui::Button(u8"保存结果到报告中",ImVec2(0,1.5*FONT_SIZE))) {
			wstring curdir = GlobalFuncs::ChDirToExePath();
			OuputResults("output.txt");
			
			OPENFILENAME Ofn;
			ZeroMemory(&Ofn, sizeof(Ofn));
			TCHAR szFile[1024];
			szFile[0] = L'\0';
			Ofn.hwndOwner = glfwGetWin32Window(myWindow);
			Ofn.lStructSize = sizeof(Ofn);
			Ofn.lpstrFilter = L"WORD文档（*.docx)\0*.docx\0\0";
			Ofn.lpstrFile = szFile;
			Ofn.nMaxFile = sizeof(szFile) / sizeof(*szFile);
			Ofn.lpstrFileTitle = NULL;
			Ofn.nMaxFileTitle = 0;
			Ofn.lpstrInitialDir = (TCHAR *)curdir.data();
			Ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
			Ofn.lpstrTitle = L"选择结果保存路径";
			BOOL ret = GetSaveFileName(&Ofn); 
			DWORD err = CommDlgExtendedError();
			if (ret) {
				curdir = GlobalFuncs::ChDirToExePath();
				wstring systemCmd = wstring(L"Reporter.exe  output.txt  \"") + Ofn.lpstrFile + L"\"";
				_wsystem(systemCmd.data());
				MainUIs::SetMessage(u8"结果已保存");
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"  返回  ", ImVec2(0, 1.5*FONT_SIZE))) {
			myDetailShowFlag["AllResult"] = false;
			MainUIs::SetMessage(u8"");
		}
		ImGui::End();
	}
	else {
		ImGui::End();
	}
}

void MainUIs::DetailsPage(string gender, string organ, double totalDose) {
	
	string detailName = gender +"_"+ organ;
	if (!myDetailShowFlag[detailName])
		return;
	string cGender = gender == "male" ? u8"男性" : u8"女性";
	string cOrgan = DoseCalculator::sOrganDict[organ];
	string title = cGender + " " + cOrgan + u8" 剂量详细来源";
	stringstream tmpLine;
	double tmpTotalDose = 0.0;
	double tmpDose = 0.0;
	ImGui::SetNextWindowSize(ImVec2(46*FONT_SIZE, 42*FONT_SIZE), ImGuiSetCond_Appearing);
	if(ImGui::Begin(title.data(), &myDetailShowFlag[detailName]))
	{
		ImGui::Columns(4, "4col_source_particle_dose_percentage");
		//titles
		ImGui::Text(u8"源类型"); ImGui::NextColumn();
		ImGui::Text(u8"粒子类型"); ImGui::NextColumn();
		ImGui::Text(u8"剂量值（Gy）"); ImGui::NextColumn();
		ImGui::Text(u8"剂量占比（%%）"); ImGui::NextColumn();
		ImGui::Separator();
		//TRP
		{
			ImGui::Text(u8"地磁俘获粒子(TRP)"); ImGui::NextColumn();
			double dose = myCalculationCore.AccumulateDose(gender, organ, "TRP");
			double percent = dose * 100 / totalDose;
			ImGui::Text(u8"合计(仅Proton)"); ImGui::NextColumn();
			ImGui::Text(MainUIs::DoseToString(dose).data()); ImGui::NextColumn();
			ImGui::Text(MainUIs::PercentToString(percent).data()); ImGui::NextColumn();

		}
		ImGui::Separator();
		//GCR
		{
			string &GCRName = myCalculationCore.myGCROpts[myCalculationCore.myCurGCRIdx];
			string displayName = u8"银河宇宙射线(" + GCRName + ")";
			ImGui::Text(displayName.data());
			//varous particles
			vector<string> sortedParticleNameset({ "Proton"
				,"P"
				,"He"
				,"Li"
				,"Be"
				,"B"
				,"C"
				,"N"
				,"O"
				,"F"
				,"Ne"
				,"Na"
				,"Mg"
				,"Al"
				,"Si"
				,"Fe" });
			for (const string &par : sortedParticleNameset){//DoseCalculator::sAllParticleName) {
				ImGui::NextColumn();
				ImGui::Text(par.data()); ImGui::NextColumn();
				double dose = myCalculationCore.AccumulateDose(gender, organ, GCRName, par);
				double percent = dose * 100 / totalDose;
				ImGui::Text(MainUIs::DoseToString(dose).data()); ImGui::NextColumn();
				ImGui::Text(MainUIs::PercentToString(percent).data()); ImGui::NextColumn();
			}
			double doseGCR = myCalculationCore.AccumulateDose(gender, organ, GCRName);
			double percent = doseGCR * 100 / totalDose;
			ImGui::NextColumn();
			ImGui::Text(u8"合计"); ImGui::NextColumn();
			ImGui::Text(MainUIs::DoseToString(doseGCR).data()); ImGui::NextColumn();
			ImGui::Text(MainUIs::PercentToString(percent).data()); ImGui::NextColumn();
		}
		ImGui::Separator();
		//SPE
		//different years
		for (map<string, int>::iterator it = myCalculationCore.mySPECounts.begin();
			it != myCalculationCore.mySPECounts.end();
			++it) {
			if (it->second < 1)
				continue;
			const string &SPEsrcName = it->first;
			string displayName = u8"太阳粒子事件(" + SPEsrcName + ")";
			ImGui::Text(displayName.data()); ImGui::NextColumn();
			double dose = myCalculationCore.AccumulateDose(gender, organ, SPEsrcName);
			double percent = dose * 100 / totalDose;
			ImGui::Text(u8"合计(仅Proton)"); ImGui::NextColumn();
			ImGui::Text(MainUIs::DoseToString(dose).data()); ImGui::NextColumn();
			ImGui::Text(MainUIs::PercentToString(percent).data()); ImGui::NextColumn();
		}
		ImGui::Text(u8"合计"); ImGui::NextColumn(); ImGui::NextColumn(); 
		ImGui::Text(MainUIs::DoseToString(totalDose).data()); ImGui::NextColumn();
		ImGui::Text(MainUIs::PercentToString(100.00).data());
		ImGui::End();
		return;
	}
	else {
		ImGui::End();
	}
}

#include <fstream>
using std::ofstream;

void MainUIs::OuputResults(string outputFile) {
	ofstream ostrm(outputFile,ios::binary);
	if (!ostrm.is_open() || !ostrm.good())
	{
		MainUIs::SetMessage((u8"不能打开："+ outputFile).c_str());
		return;
	}
	ostrm.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		//标题
		ostrm << "title\t0\t" << u8"深空探索载人航天器空间辐射风险预估报告" << endl;

		//近地轨道参数
		ostrm << "title\t1\t" << u8"一、载人航天任务参数" << endl;
		ostrm << "paragraph\t" << u8"近地轨道参数:" << endl;
		ostrm << "table begin" << endl;
		ostrm << u8"参数名称\t值" << endl;
		ostrm << u8"轨道长轴（km）\t" << myCalculationCore.myOMERE.apogee <<endl;
		ostrm << u8"轨道短轴（km）\t" << myCalculationCore.myOMERE.perigee <<endl;
		ostrm << u8"轨道倾角（°）\t" << myCalculationCore.myOMERE.inclination <<endl;
		ostrm << u8"近地点参数（°）\t" << myCalculationCore.myOMERE.perigeArgumant <<endl;
		ostrm << u8"升点经度（°）\t" << myCalculationCore.myOMERE.longtitudeofAscendingNode <<endl;
		ostrm << u8"升交点赤经（km）\t" << myCalculationCore.myOMERE.RAAN <<endl;
		ostrm << u8"直近点角（°）\t" << myCalculationCore.myOMERE.trueAnormaly <<endl;
		ostrm << u8"屏蔽层厚度\t" << myCalculationCore.myShiendOpts [myCalculationCore.myCurShieldTransParaIdx]<<endl;
		ostrm << u8"近地任务时间\t" << myCalculationCore.myDurationNearEarth <<endl;
		ostrm << "table end" << endl;

		ostrm << "paragraph\t" << u8"星际任务参数:" << endl;
		ostrm << "table begin" << endl;
		ostrm << u8"参数名称\t值" << endl;
		ostrm << u8"星际任务时间（天）\t" << myCalculationCore.myDurationInterstellar<< endl;
		ostrm << u8"银河宇宙射线大小年\t" << myCalculationCore.myGCROpts[myCalculationCore.myCurGCRIdx]<< endl;
		ostrm << "table end" << endl;

		ostrm << "paragraph\t" << u8"太阳粒子事件参数:" << endl;
		ostrm << "table begin" << endl;
		ostrm << u8"太阳粒子事件类型\t次数" << endl;
		for (auto &specount : myCalculationCore.mySPECounts) {
			ostrm << specount.first << "\t" << specount.second << endl;
		}
		ostrm << "table end" << endl;
		ostrm << "title\t1\t" << u8"二、剂量评估结果" << endl;
		//剂量
		set<string> *organsets[2] = { &DoseCalculator::sMaleOrgans
			,&DoseCalculator::sFemaleOrgans };
		string genders[2] = { "male","female" };
		string genderInTitle[2] = {u8".男性",u8".女性"};
		for (int i = 0; i < 2; ++i){
			string gender = genders[i];
			string curGenderTitle = genderInTitle[i];
			set<string> &organset = *organsets[i];
			map<string, double> totalDose;
			//某性别的剂量情况
			ostrm<<"title\t2\t"<<i+1<< genderInTitle[i]<<u8"剂量"<<endl;
			ostrm << "paragraph\t" << u8"剂量概况:" << endl;
			ostrm << "table begin" << endl;
			ostrm << u8"器官名称\t剂量值（Gy）\t限值（Gy）" << endl;
			for (const string &organ : organset) {
				double dose = myCalculationCore.AccumulateDose(gender, organ);
				totalDose[organ] = dose;
				string detailname = gender + "_" + organ;
				string doselimit = myDoseLimit.find(detailname) != myDoseLimit.end() ?
					MainUIs::DoseToString(myDoseLimit[detailname]) : "NA";
				ostrm << DoseCalculator::sOrganDict[organ]<<"\t";
				ostrm << MainUIs::DoseToString(dose) << "\t" << doselimit <<endl;
			}
			ostrm << "table end" << endl;
			//某性别的各器官剂量来源
			for (const string &organ : organset) {
				ostrm << "paragraph\t" << genderInTitle[i]<< DoseCalculator::sOrganDict[organ]<< u8"剂量详细构成:" << endl;
				ostrm << "table begin" << endl;
				ostrm << u8"源类型" << "\t" 
					<< u8"粒子类型" << "\t"
					<< u8"剂量值（Gy）" << "\t"
					<< u8"剂量占比（%）" << endl;
				//地磁俘获
				double dose = myCalculationCore.AccumulateDose(gender, organ, "TRP");
				double percent = dose * 100 / totalDose[organ];
				ostrm << u8"地磁俘获粒子(TRP)" << "\t" << u8"合计(仅Proton)"
					<< MainUIs::DoseToString(dose) << "\t" << MainUIs::PercentToString(percent).data() << endl;
				//GCR
				string &GCRName = myCalculationCore.myGCROpts[myCalculationCore.myCurGCRIdx];
				string displayName = u8"银河宇宙射线(" + GCRName + ")";
				ostrm << displayName;
				//varous particles
				for (const string &par : DoseCalculator::sAllParticleName) {
					ostrm << "\t";
					ostrm << par << "\t";
					double dose = myCalculationCore.AccumulateDose(gender, organ, GCRName, par);
					double percent = dose * 100 / totalDose[organ];
					ostrm << MainUIs::DoseToString(dose).data() << "\t";
					ostrm << MainUIs::PercentToString(percent).data() << endl;
				}
				double doseGCR = myCalculationCore.AccumulateDose(gender, organ, GCRName);
				percent = doseGCR * 100 / totalDose[organ];
				ostrm << "\t";
				ostrm << u8"合计" << "\t";
				ostrm << MainUIs::DoseToString(doseGCR) << "\t";
				ostrm << MainUIs::PercentToString(percent)<<endl;

				//SPE
				for (map<string, int>::iterator it = myCalculationCore.mySPECounts.begin();
					it != myCalculationCore.mySPECounts.end();
					++it) {
					if (it->second < 1)
						continue;
					const string &SPEsrcName = it->first;
					string displayName = u8"太阳粒子事件(" + SPEsrcName + ")";
					ostrm << displayName.data() << "\t";
					double dose = myCalculationCore.AccumulateDose(gender, organ, SPEsrcName);
					double percent = dose * 100 / totalDose[organ];
					ostrm << u8"合计(仅Proton)" << "\t";
					ostrm << MainUIs::DoseToString(dose).data() << "\t";
					ostrm << MainUIs::PercentToString(percent).data() << endl;
				}
				//合计
				ostrm << u8"合计(仅Proton)" << "\t \t"<< totalDose[organ] << "\t" << MainUIs::PercentToString(100)<<endl;
				ostrm << "table end" << endl;
			}
			
		}
	}
	catch (ifstream::failure e) {
		cout << "Error writing results: " << e.what() << endl;
	}
	catch (string &errmsg) {
	}
	catch (...) {
	}
	return;
}

bool MainUIs::InputValidDouble(const char *label, double &val, double min, double max) {
			
	if (sUITextBufs.find(label) == sUITextBufs.end())
	{
		sUITextBufs[label] = new char[64];
		strcpy(sUITextBufs[label], std::to_string(val).data());
		sUITextBufs[label][63] = '\0';
	}

	bool numberValid = true;
	if (!NumberChecker::isNumber(sUITextBufs[label]))
	{
		numberValid = false;
	}
	else {
		val = std::atof(sUITextBufs[label]);
		if(val < min || val > max)
			numberValid = false;
	}

	if (!numberValid)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(0.6,0.1,0.1,1));
		ImGui::InputText(label, sUITextBufs[label], 64);
		ImGui::PopStyleColor();
	}
	else {
		ImGui::InputText(label, sUITextBufs[label], 64);
	}

	return numberValid;
}

void MainUIs::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	wndWidth = width;
	wndHeight = height;
	MainUIs *ui = MainUIs::GetUIFromWindow(window);
	if (ui)
		ui->mySolarSys.camera.ResizeViewPortSize(float(width), float(height));
}

void MainUIs::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	MainUIs *ui = MainUIs::GetUIFromWindow(window);
	if (ui)
		ui->mySolarSys.camera.InputProcess(window);
}

void MainUIs::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	MainUIs *ui = MainUIs::GetUIFromWindow(window);

	if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
		if (ui)
			ui->mySolarSys.camera.DirectionProcess(window, xpos, ypos);
		else
			if (ui)
				ui->mySolarSys.camera.ReleaseLBtn();
}

void MainUIs::gl_error_callback(int code, const char* info) {
	cout << info << endl;
}

string MainUIs::DoseToString(double dose) {
	stringstream tmpLine;
	tmpLine << std::setiosflags(std::ios::scientific) << std::setprecision(5) << dose;
	return tmpLine.str();
}

string MainUIs::PercentToString(double percent) {
	stringstream tmpLine;
	tmpLine << std::setiosflags(std::ios::fixed) << std::setprecision(4) << percent;
	return tmpLine.str();
}
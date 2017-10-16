#pragma once
#include <string>
#include <thread>
using namespace std;

class OMEREController
{
public:
	//轨道参数
	double apogee;  //长轴长度
	double perigee; //短轴长度
	double inclination;     //轨道平面与赤道平面的夹角
	double perigeArgumant;  //长轴与最低点(升点与降点)连线的夹角,如果长轴与短轴相等，那么这个值必须为零
	double longtitudeofAscendingNode; //升点经度
	double RAAN;            //升交点赤经
	double trueAnormaly;

	double segmentDuration;

	//生成轨道参数
	int numberofOrbit;
	int numberofPointsPerOrbit;

	std::wstring orbitFilePath;//for the orbits "C:\OMERE\orbit.dat"
	
	//地磁捕获辐射能谱文件路径
	std::wstring TRPElectronFluxPath;
	std::wstring TRPProtonFluxPath;

	//the thread that runs OMERE
	std::thread myThreadOMERE;

	static int yearsSPE[4];
	std::wstring pathSPE[4];
public:
	OMEREController::OMEREController() : apogee(336.000000)
		, perigee(332.00), inclination(42.420), perigeArgumant(249.000), longtitudeofAscendingNode(25.000), RAAN(0.00000)
		, trueAnormaly(253.000), segmentDuration(15.000), orbitFilePath((L"Media\\MOERE_OUT\\orbit.dat"))
		, numberofOrbit(10), numberofPointsPerOrbit(300)
		, TRPElectronFluxPath((L"Media\\MOERE_OUT\\trpEle.flx")), TRPProtonFluxPath((L"Media\\MOERE_OUT\\trpProt.flx"))
	{
		pathSPE[0] = (L"Media\\MOERE_OUT\\solarFluxPeak1972.flx");
		pathSPE[1] = (L"Media\\MOERE_OUT\\solarFluxPeak1989.flx");
		pathSPE[2] = (L"Media\\MOERE_OUT\\solarFluxPeak2000.flx");
		pathSPE[3] = (L"Media\\MOERE_OUT\\solarFluxPeak2003.flx");
	}

	~OMEREController() {

	}

	bool CheckParameters(std::wstring &msg);

	bool SetOrbitParametersToOMERE();

	bool CalculateTRP(); //计算地磁捕获辐射能谱

	bool CalculateGCR(); //计算银河宇宙射线GCR

	bool CalculateSPE(); //计算太阳粒子事件

	void ActivateMainWndOfThisApp() {
		//activate this window
		HWND ThisMainWnd = FindWindowEx(NULL, NULL, NULL, (L"Space Dose Calculation"));
		if (ThisMainWnd) {
			SwitchToThisWindow(ThisMainWnd,TRUE);
		}
	}
	
protected:

	//open OMERE
	bool StartupOMERE();
	//close OMERE
	bool CloseOMERE();

	bool DismissCalculationFinishedWnd();
	bool WaitForFile(std::wstring &filePath);

	HWND WaitForWindow(TCHAR *wndName, TCHAR *clsName = NULL, HWND parentWnd = NULL);

};


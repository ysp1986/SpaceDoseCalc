#pragma once
#include <string>
#include <thread>
using namespace std;

class OMEREController
{
public:
	//�������
	double apogee;  //���᳤��
	double perigee; //���᳤��
	double inclination;     //���ƽ������ƽ��ļн�
	double perigeArgumant;  //��������͵�(�����뽵��)���ߵļн�,��������������ȣ���ô���ֵ����Ϊ��
	double longtitudeofAscendingNode; //���㾭��
	double RAAN;            //������ྭ
	double trueAnormaly;

	double segmentDuration;

	//���ɹ������
	int numberofOrbit;
	int numberofPointsPerOrbit;

	std::wstring orbitFilePath;//for the orbits "C:\OMERE\orbit.dat"
	
	//�شŲ�����������ļ�·��
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

	bool CalculateTRP(); //����شŲ����������

	bool CalculateGCR(); //����������������GCR

	bool CalculateSPE(); //����̫�������¼�

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


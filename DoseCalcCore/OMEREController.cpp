#include "stdafx.h"
#include "OMEREController.h"
#include "Shlwapi.h"
#include <vector>
#include <iostream>
using namespace std;


#define UI_DELAY 1000
#define TIME_OUT_COUNT 20

bool OMEREController::StartupOMERE() {
	HMODULE mHandler = GetModuleHandle(NULL);
	wchar_t exeFilePath[256];
	GetModuleFileName(mHandler, exeFilePath,256);
	wstring path = exeFilePath;
	path= path.substr(0,path.rfind(L'\\'));
	_wchdir(path.data());
	wstring  OMEREPath = path + L"\\OMERE\\OMERE_US.exe";
	if (PathFileExists(OMEREPath.data())) {
		OMEREPath = wstring(L"call \"") + OMEREPath + wstring(L"\"");
		_wsystem(OMEREPath.data());
	}
	else
		return false;
	ActivateMainWndOfThisApp();
	DWORD asdfadf= GetLastError();
	return true;
}

bool OMEREController::CloseOMERE() {
	HWND OMEREWnd = FindWindowEx(NULL, NULL, NULL, (L"mission.cen - OMERE"));
	if (!OMEREWnd)
		return false;
	while (OMEREWnd) {
		PostMessage(OMEREWnd, WM_CLOSE, 0, 0);
		Sleep(UI_DELAY);

		//if the anoying OMERE warining window is on
		HWND OMEREWarningWnd = FindWindowEx(NULL, NULL, NULL, (L"Omere"));
		if (OMEREWarningWnd) {
			HWND btnWnd = FindWindowEx(OMEREWarningWnd, NULL, (L"Button"), NULL);
			btnWnd = FindWindowEx(OMEREWarningWnd, btnWnd, (L"Button"), NULL);
			::SendMessage(OMEREWarningWnd, WM_COMMAND, MAKEWPARAM(::GetDlgCtrlID(btnWnd), BN_CLICKED), (LPARAM)btnWnd);
		}
		Sleep(UI_DELAY);
		OMEREWnd = FindWindowEx(NULL, NULL, NULL, (L"mission.cen - OMERE"));
	}
	return true;
}


bool OMEREController::SetOrbitParametersToOMERE()
{
	HWND OMEREWnd = WaitForWindow((L"mission.cen - OMERE")); //FindWindow(NULL, (L"mission.cen - OMERE"));
	ActivateMainWndOfThisApp();
	Sleep(3000);
	HWND trajParaWnd = FindWindowEx(NULL, NULL, NULL, (L"+"));
	while (!trajParaWnd) {
		::SetForegroundWindow(OMEREWnd);
		::SendMessage(OMEREWnd, WM_ACTIVATE, 0, 0);
		//press alt+M
		::keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(0x4D, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(0x4D, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		//press alt+P
		::keybd_event(0x50, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(0x50, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		::keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		Sleep(UI_DELAY);
		trajParaWnd = FindWindowEx(NULL, NULL, NULL, (L"+"));
	}
	ActivateMainWndOfThisApp();
	/*
	Sleep(UI_DELAY);
	RECT r1;
	::GetWindowRect(OMEREWnd, &r1);
	::SetCursorPos(r1.left + 122, r1.top + 40);
	::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	Sleep(UI_DELAY);

	::SetCursorPos(r1.left + 137, r1.top + 64);
	::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	Sleep(500);*/

	//set other to mission setting
	//Sleep(UI_DELAY);
	HWND missionsetting = WaitForWindow((L"+"));
	RECT r1;
	HWND btnParameters = FindWindowEx(missionsetting, NULL, NULL, (L"Parameters"));
	while (!FindWindowEx(NULL, NULL, NULL, L"Orbit Parameters"))
	{
		BOOL ret = ::GetWindowRect(missionsetting, &r1);

		HWND cbbObtMis = FindWindowEx(missionsetting, NULL, (L"ComboBox"), NULL);
		::SendMessage(cbbObtMis, CB_SETCURSEL, 0, 0);
		WPARAM param;
		int id = ::GetDlgCtrlID(cbbObtMis);
		
		param = MAKEWPARAM(id, CBN_SELCHANGE);
		::SendMessage(missionsetting, WM_COMMAND, param, (LPARAM)cbbObtMis);


		//SendMessage(cbbObtMis, CBN_SELCHANGE,0,0);
		//GetWindowRect(cbbObtMis, &r1);
		//cout << "set the Orbit/mission to OTHER" << endl;
		////scroll
		//::SetCursorPos((r1.left + r1.right) / 2, (r1.top + r1.bottom) / 2);
		//::mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 1000, 0);
		//Sleep(UI_DELAY);
		Sleep(UI_DELAY);
		btnParameters = FindWindowEx(missionsetting, NULL, NULL, (L"Parameters"));
		::PostMessage(btnParameters, BM_CLICK, 0, 0);
		Sleep(UI_DELAY);
	}
	

	//Sleep(UI_DELAY);
	//settings to Orbit parameters
	HWND parameterWnd = WaitForWindow((L"Orbit Parameters")); //FindWindowEx(NULL, NULL, NULL, (L"Orbit Parameters"));
	if (NULL == parameterWnd)
	{
		cout << "error in popup Orbit Parameters" << endl;
		return false;
	}

	HWND checkBtnOrbitPara = FindWindowEx(parameterWnd, NULL, NULL, (L"Orbit Parameters"));
	::SendMessage(checkBtnOrbitPara, BM_CLICK, 0, BST_CHECKED);

	//set the parameters
	HWND editApogee = FindWindowEx(parameterWnd, checkBtnOrbitPara, (L"Edit"), NULL);
	std::wstring text;
	text = to_wstring(this->apogee);
	::SendMessage(editApogee, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND editPerigee = FindWindowEx(parameterWnd, editApogee, (L"Edit"), NULL);
	text = to_wstring(this->perigee);
	::SendMessage(editPerigee, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND editInclination = FindWindowEx(parameterWnd, editPerigee, (L"Edit"), NULL);
	text = to_wstring(this->inclination);
	::SendMessage(editInclination, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND editPerigeeArg = FindWindowEx(parameterWnd, editInclination, (L"Edit"), NULL);
	text = to_wstring(this->perigeArgumant);
	::SendMessage(editPerigeeArg, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND editLAN = FindWindowEx(parameterWnd, editPerigeeArg, (L"Edit"), NULL);
	text = to_wstring(this->longtitudeofAscendingNode);
	::SendMessage(editLAN, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND editTrueAnomaly = FindWindowEx(parameterWnd, editLAN, (L"Edit"), NULL);
	text = to_wstring(this->trueAnormaly);
	::SendMessage(editTrueAnomaly, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND staticSegmentDuration = FindWindowEx(parameterWnd, NULL, (L"Static"), (L"Segment duration :"));
	HWND editDuration = FindWindowEx(parameterWnd, staticSegmentDuration, (L"Edit"), NULL);
	text = to_wstring(this->segmentDuration);
	::SendMessage(editDuration, WM_SETTEXT, 0, (LPARAM)text.data());


	HWND comboBoxUnitDuration = FindWindowEx(parameterWnd, editDuration, (L"ComboBox"), NULL);
	/*text = L"days";
	::SendMessage(comboBoxUnitDuration, WM_SETTEXT, 0, (LPARAM)text.data());*/
	::SendMessage(comboBoxUnitDuration, CB_SETCURSEL, 0, 0);
	WPARAM param;
	int id = ::GetDlgCtrlID(comboBoxUnitDuration);
	param = MAKEWPARAM(id, CBN_SELCHANGE);
	::SendMessage(parameterWnd, WM_COMMAND, param, (LPARAM)comboBoxUnitDuration);

	//find the combox of setting RAAN or longtitude of ascending node
	HWND staticSegmentName = FindWindowEx(parameterWnd, NULL, (L"Static"), (L"Segment name :"));
	HWND cbbRAANLAN = FindWindowEx(parameterWnd, staticSegmentName, (L"ComboBox"), NULL);

	::SendMessage(cbbRAANLAN, CB_SETCURSEL, 1, 0);
	id = ::GetDlgCtrlID(cbbRAANLAN);
	param = MAKEWPARAM(id, CBN_SELCHANGE);
	::SendMessage(parameterWnd, WM_COMMAND, param, (LPARAM)cbbRAANLAN);
	//set longtitude of ascending node
	/*GetWindowRect(cbbRAANLAN, &r1);
	cout << "set longtitude of ascending node" << endl;
	::SetCursorPos((r1.left + r1.right) / 2, (r1.top + r1.bottom) / 2);
	::mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -1000, 0);*/
	
	text = to_wstring(this->longtitudeofAscendingNode);
	::SendMessage(editLAN, WM_SETTEXT, 0, (LPARAM)text.data());
	
	////set RAAN

	//::SendMessage(cbbRAANLAN, CB_SETCURSEL, 0, 0);
	//id = ::GetDlgCtrlID(cbbRAANLAN);
	//param = MAKEWPARAM(id, CBN_SELCHANGE);
	//::SendMessage(parameterWnd, WM_COMMAND, param, (LPARAM)cbbRAANLAN);
	///*::SetCursorPos((r1.left + r1.right) / 2, (r1.top + r1.bottom) / 2);
	//::mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 1000, 0);*/
	////Sleep(1000);
	//text = to_wstring(this->RAAN);
	//::SendMessage(editLAN, WM_SETTEXT, 0, (LPARAM)text.data());

	//the output dir
	HWND staticOutputFile = FindWindowEx(parameterWnd, NULL, (L"Button"), (L"Output File"));
	HWND editOutputDir = FindWindowEx(parameterWnd, staticOutputFile, (L"Edit"), NULL);
	::SendMessage(editOutputDir, WM_SETTEXT, 0, (LPARAM)orbitFilePath.data());
	Sleep(UI_DELAY);
	//set the orbit number (predefined, no need to alter in later usage
	HWND staticNumberOrbits = FindWindowEx(parameterWnd, NULL, (L"Static"), (L"Number of orbits:"));
	HWND editNumberOrbits = FindWindowEx(parameterWnd, staticNumberOrbits, (L"Edit"), NULL);
	text = to_wstring(this->numberofOrbit);
	::SendMessage(editNumberOrbits, WM_SETTEXT, 0, (LPARAM)text.data());

	HWND editNumberPointsPerOrbits = FindWindowEx(parameterWnd, editNumberOrbits, (L"Edit"), NULL);
	text = to_wstring(this->numberofPointsPerOrbit);
	::SendMessage(editNumberPointsPerOrbits, WM_SETTEXT, 0, (LPARAM)text.data());

	Sleep(UI_DELAY);

	//clear the existing orbit file
	::DeleteFile(orbitFilePath.data());

	//press the ok button
	HWND btnOK = FindWindowEx(parameterWnd, NULL, (L"Button"), (L"DONE"));
	id = ::GetDlgCtrlID(btnOK);
	param = MAKEWPARAM(id, BN_CLICKED);
	::PostMessage(parameterWnd, WM_COMMAND, param, (LPARAM)btnOK);
	//::PostMessage(btnOK, BM_CLICK, 0, 0);
	Sleep(UI_DELAY);

	//if warning windows shows, close it.
	HWND warningWnd = FindWindowEx(NULL, NULL, NULL, (L"Omere"));
	if (warningWnd){
		//pre the no button
		HWND btnYes = FindWindowEx(warningWnd, NULL, (L"Button"), NULL);
		HWND btnNo = FindWindowEx(warningWnd, btnYes, (L"Button"), NULL);
		WPARAM param;
		int id = ::GetDlgCtrlID(btnNo);
		param = MAKEWPARAM(id,BN_CLICKED);
		::SendMessage(warningWnd, WM_COMMAND, param, (LPARAM)btnNo);
	}

	//wait until the progress is done
	int waittime = 0;
	while (!PathFileExists(orbitFilePath.data()) && waittime < TIME_OUT_COUNT*20)
	{
		Sleep(UI_DELAY);
		waittime++;
	}
	if (waittime == TIME_OUT_COUNT)
	{
		cout << "cannot generate the orbit" << endl;
		return false;
	}

	missionsetting = WaitForWindow((L"+"));
	btnOK = FindWindowEx(missionsetting, NULL, (L"Button"), (L"OK"));
	::SendMessage(missionsetting, WM_COMMAND, MAKEWPARAM(::GetDlgCtrlID(btnOK), BN_CLICKED), (LPARAM)btnOK);

	return true;
}

bool OMEREController::CheckParameters(std::wstring &msg){
	bool ret = true;
	msg = (L"");
	if (apogee < 6378)
	{
		msg += (L" Apogee is too small.\n");
		ret = false;
	}

	if (perigee < 6378)
	{
		msg += (L" Perigee is too small.\n");
		ret = false;
	}

	if (perigee > apogee)
	{
		msg += (L" Perigee is larger than Apogee.\n");
		ret = false;
	}

	if (perigee == apogee && perigeArgumant && (perigeArgumant != 0.0 || longtitudeofAscendingNode != 0.0))
	{
		msg += (L" \"Perige Argumant\" and \"Longtitude of Ascending Node\" must be zero when it's a circular orbit.\n");
		ret = false;
	}

	if (inclination == 0.0 && RAAN != 0.0)
	{
		msg += (L" RAAN should be zero when inclination is zero.\n");
		ret = false;
	}

	return ret;
}

bool OMEREController::CalculateTRP(){
	myThreadOMERE = std::thread(&OMEREController::StartupOMERE, this);
	SetOrbitParametersToOMERE();
	HWND OMEREWnd = WaitForWindow((L"mission.cen - OMERE")); //FindWindow(NULL, (L"mission.cen - OMERE"));
	HWND TRPWnd = FindWindowEx(NULL, NULL, NULL, (L"Trapped particles"));
	while (!TRPWnd) {
		::SetForegroundWindow(OMEREWnd);
		::SendMessage(OMEREWnd, WM_ACTIVATE, 0, 0);
		//press alt+F
		::keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(0x45, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(0x45, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		::keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		//press Enter
		::keybd_event(VK_RETURN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(VK_RETURN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		//find
		TRPWnd = WaitForWindow((L"Trapped particles"));
	}//FindWindow(NULL, (L"Trapped particles"));
	if (!TRPWnd){
		cout << "Cannot find Trapped particles setting window" << endl;
		return false;
	}
	Sleep(UI_DELAY);
	ActivateMainWndOfThisApp();

	//set output path for electron flux
	HWND staticNumberOrbits = FindWindowEx(TRPWnd, NULL, (L"Static"), (L"Output file (Electrons)"));
	HWND editPathElectronFlx = FindWindowEx(TRPWnd, staticNumberOrbits, (L"Edit"), NULL);
	::SendMessage(editPathElectronFlx, WM_SETTEXT, 0, (LPARAM)this->TRPElectronFluxPath.data());
	//set output path for proton flux
	HWND editNumberOrbits = FindWindowEx(TRPWnd, editPathElectronFlx, (L"Edit"), NULL);
	::SendMessage(editNumberOrbits, WM_SETTEXT, 0, (LPARAM)this->TRPProtonFluxPath.data());

	Sleep(UI_DELAY);
	//delete existing electronflux and protonflux files
	::DeleteFile(this->TRPElectronFluxPath.data());
	::DeleteFile(this->TRPProtonFluxPath.data());

	//press CALCULATIONS button
	HWND btnCALCULATIONS = FindWindowEx(TRPWnd, NULL, (L"Button"), (L"CALCULATIONS"));
	::PostMessage(btnCALCULATIONS, BM_CLICK, 0, 0);

	//press OK button
	HWND btnOK = FindWindowEx(TRPWnd, NULL, (L"Button"), (L"OK"));
	::PostMessage(btnOK, BM_CLICK, 0, 0);


	if (!WaitForFile(TRPElectronFluxPath) || !WaitForFile(TRPElectronFluxPath))
		return false;

	if (!DismissCalculationFinishedWnd())
		return false;
	CloseOMERE();
	myThreadOMERE.join();
	return true;
}

bool OMEREController::CalculateGCR(){
	HWND OMEREWnd = WaitForWindow((L"mission.cen - OMERE")); //FindWindow(NULL, (L"mission.cen - OMERE"));
	::SetForegroundWindow(OMEREWnd);
	::SendMessage(OMEREWnd, WM_ACTIVATE, 0, 0);
	Sleep(UI_DELAY);


	return true;
}

int OMEREController::yearsSPE[4] = { 1972, 1989, 2000, 2003 };

bool OMEREController::CalculateSPE(){
	HWND OMEREWnd = WaitForWindow((L"mission.cen - OMERE")); //FindWindow(NULL, (L"mission.cen - OMERE"));
	::SetForegroundWindow(OMEREWnd);
	::SendMessage(OMEREWnd, WM_ACTIVATE, 0, 0);
	Sleep(UI_DELAY);
	//press alt+e alt+s alt+s, during which without releasing alt open the SPE dialog
	//alt+e
	::keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
	::keybd_event(0x45, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
	::keybd_event(0x45, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	//press alt+s
	::keybd_event(0x53, 0x47, KEYEVENTF_EXTENDEDKEY | 0, 0);
	::keybd_event(0x53, 0x47, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	//press alt+s
	::keybd_event(0x53, 0x47, KEYEVENTF_EXTENDEDKEY | 0, 0);
	::keybd_event(0x53, 0x47, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	//release alt
	::keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

	Sleep(UI_DELAY);
	//find the SPE window
	HWND SPEWnd = FindWindow(NULL, (L"Solar Particles: flare models"));
	if (!SPEWnd)
	{
		cout << "Cannot open SPE window" << endl;
		return false;
	}
	//activate the year selection widget
	HWND staticNumberOrbits = FindWindowEx(SPEWnd, NULL, (L"Static"), (L"Peak flux (protons):"));
	HWND cbbYear = FindWindowEx(SPEWnd, staticNumberOrbits, (L"ComboBox"), NULL);
	::SetForegroundWindow(cbbYear);
	::SendMessage(cbbYear, WM_ACTIVATE, 0, 0);
	for (int i = 0; i < 10; ++i){
		::keybd_event(VK_UP, 0x47, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(VK_UP, 0x47, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}
	
	for (int i = 0; i < 4; ++i)
	{
		HWND cbbYear = FindWindowEx(SPEWnd, staticNumberOrbits, (L"ComboBox"), NULL);
		::SetForegroundWindow(cbbYear);
		::SendMessage(cbbYear, WM_ACTIVATE, 0, 0);
		::keybd_event(VK_DOWN, 0x47, KEYEVENTF_EXTENDEDKEY | 0, 0);
		::keybd_event(VK_DOWN, 0x47, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		Sleep(UI_DELAY);
		//set path
		HWND staticOutputFile = FindWindowEx(SPEWnd, NULL, (L"Static"), (L"Output file :"));
		HWND editOutputPath = FindWindowEx(SPEWnd, staticOutputFile, (L"Edit"), NULL);
		::SendMessage(editOutputPath, WM_SETTEXT, 0, (LPARAM)this->pathSPE[i].data());

		//delete existing file
		::DeleteFile(this->pathSPE[i].data());

		//press calculate
		HWND btnCalc = FindWindowEx(SPEWnd, NULL, (L"Button"), (L"CALCULATION"));
		::PostMessage(btnCalc, BM_CLICK, 0, 0);


		if (!WaitForFile(this->pathSPE[i]))
			return false;
		Sleep(UI_DELAY);
		if (!DismissCalculationFinishedWnd())
			return false;
	}
	//press OK
	HWND btnOK = FindWindowEx(SPEWnd, NULL, (L"Button"), (L"OK"));
	::PostMessage(btnOK, BM_CLICK, 0, 0);
	Sleep(UI_DELAY);

	return true;
}

bool OMEREController::DismissCalculationFinishedWnd(){
	HWND calcFinishWnd = WaitForWindow((L"Omere"), (L"#32770")); //FindWindowEx(NULL, NULL, (L"#32770"), (L"Omere"));

	if (!calcFinishWnd){
		cout << "Cannot dismiss the calculation finished notice." << endl;
		return false;
	}
		
	//dismiss calcFinishWnd
	int waittime = 1;
	while (calcFinishWnd && waittime < TIME_OUT_COUNT)
	{
		HWND btnOK = FindWindowEx(calcFinishWnd, NULL, (L"Button"), NULL);
		::PostMessage(btnOK, BM_CLICK, 0, 0);
		Sleep(UI_DELAY);
		waittime++;
		calcFinishWnd = FindWindowEx(NULL, NULL, (L"#32770"), (L"Omere"));
	}
	if (waittime == TIME_OUT_COUNT){
		cout << "Cannot dismiss the calculation finished notice." << endl;
		return false;
	}

	return true;
}

bool OMEREController::WaitForFile(std::wstring &filePath){
	int waittime = 1;

	while (!PathFileExists(filePath.data()) && waittime < TIME_OUT_COUNT)
	{
		Sleep(UI_DELAY);
		waittime++;
	}

	if (waittime == TIME_OUT_COUNT)
	{
		std::wcout<< L"Wait for file " << filePath <<L" failed" << endl;
		return false;
	}
	return true;
}

HWND OMEREController::WaitForWindow(TCHAR *wndName, TCHAR *clsName, HWND parentWnd)
{
	int waittime = 1;
	HWND ret = FindWindowEx(parentWnd, NULL, clsName, wndName);
	while (!ret)
	{
		ret = FindWindowEx(parentWnd, NULL, clsName, wndName);
		waittime++;
		Sleep(UI_DELAY);
	}
	return ret;
}
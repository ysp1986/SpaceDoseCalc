#include "stdafx.h"
#include "DoseCalculator.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include "NumberChecker.h"
#include "FluxBasedCalculationCore.h"
#include <cassert>
using namespace std;

set<std::string> DoseCalculator::sAllGenderName({"male","female"});
set<std::string> DoseCalculator::sAllSourceName({"GCR","TRP"});//to be complemented while loading
set<std::string> DoseCalculator::sAllParticleName({"Proton"
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
	,"Fe"});

map<string, string> DoseCalculator::sOrganDict{
	{ u8"marrow", u8"红骨髓" },
	{ u8"brain", u8"大脑" },
	{ u8"skin", u8"皮肤" },
	{ u8"heart", u8"心脏" },
	{ u8"lens", u8"晶状体" },
	{ u8"uterus", u8"子宫" },
	{ u8"thymus", u8"胸腺" },
};
set<string> DoseCalculator::sMaleOrgans{ u8"marrow",u8"brain",u8"skin",u8"heart",u8"lens" };
set<string> DoseCalculator::sFemaleOrgans{ u8"marrow",u8"brain",u8"skin",u8"heart",u8"lens",u8"uterus",u8"thymus" };

DoseCalculator::~DoseCalculator()
{
}


bool DoseCalculator::LoadAllData()
{
	//设置当前路径
	HMODULE mHandler = GetModuleHandle(NULL);
	wchar_t exeFilePath[256];
	GetModuleFileName(mHandler, exeFilePath, 256);
	wstring path = exeFilePath;
	path = path.substr(0, path.rfind(L'\\'));
	_wchdir(path.data());
	//load OMERETRP data
	if (!LoadTRP()) {
		return false;
	}

	if (!LoadSPE()) {
		return false;
	}

	//load other data
	if (!LoadGCR()) {
		return false;
	}
	
	if (!LoadShieldTrans()) {
		return false;
	}
	if (!LoadDoseTrans()) {
		return false;
	}

	return true;
}

bool DoseCalculator::LoadAllButTRP() {
	//设置当前路径
	HMODULE mHandler = GetModuleHandle(NULL);
	wchar_t exeFilePath[256];
	GetModuleFileName(mHandler, exeFilePath, 256);
	wstring path = exeFilePath;
	path = path.substr(0, path.rfind(L'\\'));
	_wchdir(path.data());

	if (!LoadSPE()) {
		return false;
	}

	//load other data
	if (!LoadGCR()) {
		return false;
	}

	if (!LoadShieldTrans()) {
		return false;
	}
	if (!LoadDoseTrans()) {
		return false;
	}

	return true;
}

bool DoseCalculator::LoadTRP()
{
	ifstream input(myTRPPath);
	string line;
	bool dataStart = false;
	myTRPDatas.clear();
	myTRPDatas.resize(1,DualColData("TRP_Proton")); 
	DoseCalculator::sAllSourceName.insert("TRP"); //add source type
	input.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		while (input.good(), getline(input, line))
		{
			if (line.find("#    (MeV)  ") == 0) {
				dataStart = true;
				continue;
			}
			if (dataStart && line.size() > 1)
			{
				vector<string> tokens(SPLIT_SPACE(line));
				if (tokens.size() == 0)
					continue;
				bool valid = true;
				for (auto &a : tokens)
					if (!NumberChecker::isNumber(a))
						valid = false;
				if (!valid)
					throw string(line + " is not all double strings.");
				if (tokens.size() != 3)
					throw string(line + " should have and only have 3 numbers.");
				myTRPDatas[0].AddData(atof(tokens[0].data()), atof(tokens[2].data()));
			}
		}
	}
	catch (ifstream::failure e) {
		cout<<"Error reading TRP: "<< e.what() << endl;
	}
	catch (string &errmsg) {
		cout << "Reading TRP: " << errmsg << endl;
	}
	catch (...) {
		cout << "Something reading TRP" << endl;
	}
	return true;
}


bool DoseCalculator::LoadGCR()
{
	ifstream input(myGCRPath);
	string line;
	DualColData *curData=NULL;
	string curMinMax("");
	string curParticle;
	input.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		while (input.good(),getline(input, line))
		{
			//cout << line << endl;
			if (line.find("MIN") != string::npos) {
				curMinMax = "MIN";
				continue;
			}
			if (line.find("MAX") != string::npos) {
				curMinMax = "MAX";
				continue;
			}
			if (sAllParticleName.find(line) != sAllParticleName.end()) {
				curParticle = line;
				myGCRDatas.resize(myGCRDatas.size()+1);
				string curSourceName = string("GCR-") + curMinMax;
				DoseCalculator::sAllSourceName.insert(curSourceName);  //insert source type
				if(std::find(myGCROpts.begin(), myGCROpts.end(), curSourceName) == myGCROpts.end())
					myGCROpts.push_back(curSourceName);
				string curDataName = curSourceName + string("_") + curParticle;
				new(myGCRDatas.data() + myGCRDatas.size() - 1) DualColData(curDataName.data());
				curData = myGCRDatas.data() + myGCRDatas.size() - 1;
				cout << "Reading: " << curData->myName << endl;
				continue;
			}
			if (line.find("energyMeV") != string::npos)
				continue;
			if (line.size() > 3)
			{
				vector<string> tokens(SPLIT_SPACE(line));
				if (tokens.size() == 0)
					continue;
				bool valid = true;
				for (auto a : tokens)
					if (!NumberChecker::isNumber(a))
						valid = false;
				if (!valid)
					throw string(line + " is not all double strings.");
				if (nullptr == curData)
					throw string(line + " start logging data without Particle Name.");
				if (tokens.size()!=3)
					throw string(line + " should have and only have 3 numbers.");
				curData->AddData(atof(tokens[0].data()),atof(tokens[2].data()));
			}
		}
	}
	catch (ifstream::failure e) {
		cout << "Error reading GCR: " << e.what() << endl;
	}
	catch (string &errmsg) {
		cout << "Error reading GCR: " << errmsg << endl;
	}
	catch (...) {
		cout << "Something reading GCR" << endl;
	}
	return true;
}


bool DoseCalculator::LoadSPE()
{
	mySPEDatas.clear();
	ifstream input(mySPEPath);
	string line;
	DualColData *curData = nullptr;
	input.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		while (getline(input, line).good())
		{
			if (line == "2000	1.44E+02")
				cout << "here" << endl;
			if (line.size() < 1)
				continue;
			vector<string> tokens(SPLIT_SPACE(line));
			if (tokens.size() == 0)
				continue;
			bool all_number = true;
			for (auto a : tokens)
				if (!NumberChecker::isNumber(a))
					all_number = false;
			if (!all_number) {
				mySPEDatas.resize(mySPEDatas.size()+1);
				string curSourceName = string("SPE-") + tokens[0];
				mySPECounts[curSourceName] = 0;
				DoseCalculator::sAllSourceName.insert(curSourceName);//add source type
				string curDataName = curSourceName + "_Proton";
				new(mySPEDatas.data() + mySPEDatas.size() - 1) DualColData(curDataName.data());
				curData = mySPEDatas.data() + mySPEDatas.size() - 1;
			}else{
				if(tokens.size() != 2)
					throw string(line + " does not have and only have 2 numbers.");
				if (nullptr == curData)
					throw string(line + ": data starts without data header");
				curData->AddData(atof(tokens[0].data()), atof(tokens[1].data()));
			}
		}
	}
	catch (ifstream::failure e) {
		cout << "Error reading SPE: " << e.what() << endl;
	}
	catch (string &errmsg) {
		cout << "Error reading SPE: " << errmsg << endl;
	}
	catch (...) {
		cout << "Something reading SPE" << endl;
	}
	return true;
}


bool DoseCalculator::LoadShieldTrans()
{
	myShiendOpts.clear();
	myShiendOpts.push_back("NA");
	myShieldTransParas.clear();
	ifstream input(myShieldParaPath);
	string line;
	string curShieldThickness;
	string curParticleName;
	DualColData *curData = nullptr;
	input.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		while (getline(input, line), input.good())
		{
			if (line.size() < 1)
				continue;
			vector<string> tokens(SPLIT_SPACE(line));
			if (tokens.size() == 0)
				continue;
			if (line.find("g/cm2") != string::npos)
			{
				curShieldThickness = line;
				myShiendOpts.push_back(line);
				continue;
			}
			if (sAllParticleName.find(tokens[0]) != sAllParticleName.end())
			{
				curParticleName = tokens[0];
				myShieldTransParas.resize(myShieldTransParas.size() + 1);
				new(myShieldTransParas.data() + myShieldTransParas.size() - 1) DualColData((curShieldThickness + "_"+ curParticleName).c_str());
				curData = myShieldTransParas.data() + myShieldTransParas.size() - 1;
				curData->AddData(0.0, 0.0);
				continue;
			}
			if (tokens.size() == 2
				&& NumberChecker::isNumber(tokens[0].data())
				&& NumberChecker::isNumber(tokens[1].data())) {
				curData->AddData(atof(tokens[0].data()), atof(tokens[1].data()));
			}
		}
	}
	catch (ifstream::failure e) {
		cout << "Error reading Shield trans: " << e.what() << endl;
	}
	catch (string &errmsg) {
		cout << "Error reading Shield trans: " << errmsg << endl;
	}
	catch (...) {
		cout << "Something reading Shield trans" << endl;
	}
	return true;
}


bool DoseCalculator::LoadDoseTrans()
{
	myDoseTransParas.clear();
	ifstream input(myDoseParaPath);
	string line;
	string curShieldThickness;
	string curParticleName;
	DualColData *curData = nullptr;
	string curGender;
	vector<string> curOrganNames;
	vector<DualColData*> curDatas;
	input.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		while (getline(input, line), input.good())
		{
			if (line.size() < 1)
				continue;
			vector<string> tokens(SPLIT_SPACE(line));
			if (tokens.size() == 0)
				continue;
			if (sAllParticleName.find(tokens[0]) != sAllParticleName.end())
			{
				curParticleName = tokens[0];
				continue;
			}
			if (tokens[0] == "male" || tokens[0] == "female")
			{
				curGender = tokens[0];
				continue;
			}
			if (tokens.size() >= 2 && tokens[0] == "erg")
			{
				curDatas.clear();
				curOrganNames = tokens;
				curOrganNames.erase(curOrganNames.begin());
				for (auto organName : curOrganNames) {
					string curTotalName = curGender + '_' + organName + '_' + curParticleName;
					myDoseTransParas.push_back(curTotalName.data());
					if (curGender == "male")
						sMaleOrgans.insert(organName);
					else if (curGender == "female")
						sFemaleOrgans.insert(organName);
					else {
						if (myMsgReportor) {
							string msg = u8"读取剂量转换因子出错，未知的性别：" + curGender;
							myMsgReportor(msg.data());
						}
					}
				}
				unsigned int numOrgans = tokens.size() - 1;
				for (unsigned int i = 0; i < numOrgans; ++i) {
					curDatas.push_back(myDoseTransParas.data()+ myDoseTransParas.size()- numOrgans +i);
					curDatas.back()->AddData(0.0, 0.0);
				}
				continue;
			}
			//now is the actual data
			assert(curDatas.size() == tokens.size()-1);
			for (auto &str : tokens)
				if (!NumberChecker::isNumber(str))
				throw(line + " error, should all be numbers");
			double curerg = atof(tokens[0].data());
			for (unsigned int i = 0; i < curDatas.size(); ++i) {
				curDatas[i]->AddData(curerg, atof(tokens[i + 1].data()));
			}
		}
	}
	catch (ifstream::failure e) {
		cout << "Error reading Shield trans: " << e.what() << endl;
	}
	catch (string &errmsg) {
		cout << "Error reading Shield trans: " << errmsg << endl;
	}
	catch (...) {
		cout << "Something reading Shield trans" << endl;
	}
	return true;
}

void DoseCalculator::AllCalc() {
	myCalculationSuccess = true;
	if (myMsgReportor)
		myMsgReportor(u8"计算地磁捕获粒子能谱...");
	if (!CalcTRP() ) {
		myCalculationSuccess = false;
	}
	if(!LoadTRP()){
		myCalculationSuccess = false;
	}
	if (!CalcDoses()) {
		myCalculationSuccess = false;
	}
	if (myMsgReportor)
		myMsgReportor(u8"分级统计结果...");
	//accumulate the doses for show
	for (const string &organ : DoseCalculator::sMaleOrgans) {
		string nameL1 = string("male") + u8"_" + organ;
		AccumulateDose("male", organ);
		cout << nameL1 << myDoseResult[nameL1]<< endl;
		for (const string &source : DoseCalculator::sAllSourceName) {
			string nameL2 = nameL1 + u8"_" + source;
			AccumulateDose("male", organ,source);
			cout << nameL2 << myDoseResult[nameL2] << endl;
		}
	}
	for (const string &organ : DoseCalculator::sFemaleOrgans) {
		string nameL1 = string("female") + u8"_" + organ;
		AccumulateDose("female", organ);
		cout << nameL1 << myDoseResult[nameL1] << endl;
		for (const string &source : DoseCalculator::sAllSourceName) {
			string nameL2 = nameL1 + u8"_" + source;
			AccumulateDose("female", organ, source);
			cout << nameL2 << myDoseResult[nameL2] << endl;
		}
	}
	myCalculationFinished = true;
}

bool DoseCalculator::CalcTRP() {
	if (!myOMERE.CalculateTRP())
		return false;
	return true;
}

bool DoseCalculator::CalcDoses() {
	myDoseResult.clear();
	//TRP
	if (myTRPDatas.size() < 1)
		return false;
	//get all dose of every particle to every organ under myShieldThickness
	vector<pair<string, double>> trpDoses;
	if (myMsgReportor)
		myMsgReportor(u8"计算地磁捕获粒子剂量...");
	CalcDoseFromOneSource(myTRPDatas, trpDoses);
	//乘以近地轨道时间
	double trpTime = myDurationNearEarth * 3600 * 24;
	for (auto &res : trpDoses) {
		res.second *= trpTime;
	}
	//SPE
	if (myMsgReportor)
		myMsgReportor(u8"计算太阳粒子事件剂量...");
	vector<pair<string, double>> speDoses;
	CalcDoseFromOneSource(mySPEDatas, speDoses);
	//乘以SPE数量
	for (auto &res : speDoses) {
		string &name = res.first;
		vector<string> keys(SPLIT(name, '_'));
		if (keys.size() < 3)
		{
			continue;
		}
		string &srcname = keys[2];
		if(mySPECounts.find(srcname) != mySPECounts.end())
			res.second *= mySPECounts[srcname];
	}
	//GCR
	if (myMsgReportor)
		myMsgReportor(u8"计算银河宇宙射线剂量...");
	vector<pair<string, double>> gcrDoses;
	CalcDoseFromOneSource(myGCRDatas, gcrDoses);
	//乘以空间飞行时间
	double gcrTime = myDurationInterstellar * 3600 * 24;
	for (auto &res : gcrDoses) {
		res.second *= gcrTime;
	}
	//add to the total dose rates
	for (auto &data : trpDoses) {
		myDoseResult[data.first] = data.second;
	}
	for (auto &data : speDoses) {
		myDoseResult[data.first] = data.second;
	}
	for (auto &data : gcrDoses) {
		myDoseResult[data.first] = data.second;
	}
	return true;
}

bool DoseCalculator::CalcDoseFromOneSource(std::vector<DualColData> srcFlx
	, vector<pair<string, double>> &outDoses) {
	//get the shielding parameter according to thickness
	vector<DualColData> curShieldParas;
	for (auto &shieldPara : myShieldTransParas) {
		if (shieldPara.myName.find(myShiendOpts[myCurShieldTransParaIdx]) != string::npos)
			curShieldParas.push_back(shieldPara);
	}

	for (auto &sourceFlx : srcFlx) {
		string &sourceName = sourceFlx.myName;
		vector<string> srcFlxKeys(SPLIT(sourceName, '_'));
		string &srcOfSrc = srcFlxKeys[0];
		//filtering the GCR year
		if (srcOfSrc.find("GCR") != string::npos
			&& srcOfSrc != myGCROpts[myCurGCRIdx])
			continue;
		//filtering the SPE source
		if (srcOfSrc.find("SPE") != string::npos
			&& mySPECounts[srcOfSrc] < 1)
			continue;

		
		string parType;
		assert(srcFlxKeys.size() == 2);
		parType = srcFlxKeys[1];
		/*if (sourceName.find("TRP") != string::npos && parType.find("Proton") != string::npos)
			cout << "here" << endl;*/
		DualColData shldFlux;
		if (myShiendOpts[myCurShieldTransParaIdx] == "NA") {
			shldFlux = sourceFlx;
			for (auto &dosePara : myDoseTransParas) {
				if (myInterruptCalculation)
					return false;
				//剂量转换因子定义：[male|female]_ORGAN_PARTICLE
				vector<string> doseParaKeys(SPLIT(dosePara.myName, '_'));
				assert(doseParaKeys.size() == 3);
				if (doseParaKeys[2] == parType) {
					//其中剂量名称这样定义：[male|female]_ORGAN<_SOURCE<_PARTICLE>> 分三级
					string doseName = doseParaKeys[0] + "_" + doseParaKeys[1] + "_" + sourceName;

					double dose = FluxBasedCalculationCore::CalculateDose(myDosTranExpandInt, shldFlux, dosePara);
					outDoses.push_back(make_pair(doseName, dose));
					//cout << doseName << " is: " << dose << endl;
				}
			}
		}
		else {
			for (auto &shieldPara : curShieldParas) {
				if (shieldPara.myName.find(myShiendOpts[myCurShieldTransParaIdx]) == string::npos)
					continue;
				//check if it's the shieldPara for this particle
				if (shieldPara.myName.find(parType) != string::npos) {
					FluxBasedCalculationCore::ShieldingTrans(sourceFlx, shieldPara, shldFlux);
					//calculate dose
					for (auto &dosePara : myDoseTransParas) {
						if (myInterruptCalculation)
							return false;
						//剂量转换因子定义：[male|female]_ORGAN_PARTICLE
						vector<string> doseParaKeys(SPLIT(dosePara.myName, '_'));
						assert(doseParaKeys.size() == 3);
						if (doseParaKeys[2] == parType) {
							//其中剂量名称这样定义：[male|female]_ORGAN<_SOURCE<_PARTICLE>> 分三级
							string doseName = doseParaKeys[0] + "_" + doseParaKeys[1] + "_" + sourceName;

							double dose = FluxBasedCalculationCore::CalculateDose(myDosTranExpandInt, shldFlux, dosePara);
							outDoses.push_back(make_pair(doseName, dose));
							//cout << doseName << " is: " << dose << endl;
						}
					}
				}
			}
		}
		
		if (shldFlux.Size() == 0) {
			cout << "Cannot find shield paras" << endl;
			return false;
		}
	}
	return true;
}

double DoseCalculator::AccumulateDose(string gender, string organ
	, string source, string parType) {
	//check if the dose is in the result to display
	string dataName = gender;
	if (organ.size() > 0)
		dataName = dataName + "_" + organ;
	if (source.size() > 0)
		dataName = dataName + "_" + source;
	if (parType.size() > 0)
		dataName = dataName + "_" + parType;
	if (myDoseResult.find(dataName) != myDoseResult.end())
		return myDoseResult[dataName];

	double ret = 0.0;
	//其中剂量名称这样定义：[male|female]_ORGAN<_SOURCE<_PARTICLE>> 分三级
	if (organ.size() == 0)
	{
		if (gender == "male") {
			for (const string &organName : sMaleOrgans) {
				ret += AccumulateDose(gender,organName);
			}
		}
		else {
			for (const string &organName : sFemaleOrgans) {
				ret += AccumulateDose(gender, organName);
			}
		}
	}
	else if (source.size() == 0) {
		for (const string &sourceName : sAllSourceName) {
			ret += AccumulateDose(gender, organ, sourceName);
		}
	}
	else if (parType.size() == 0) {
		for (const string &parTypeName : sAllParticleName) {
			ret += AccumulateDose(gender, organ, source, parTypeName);
		}
	}
	myDoseResult[dataName] = ret;
	return ret;
}
#pragma once
#include "DualColData.h"
#include <string>
#include <map>
#include <set>
#include "OMEREController.h"
#include <mutex>
#include <iterator>

template<char delimer>class splitstring :public string {
public:
};
template<char delimer>istream &  operator>>(istream &is, splitstring<delimer> &out) {
	while (getline(is, out, delimer), is.good()) {
		if (out.size() > 0)
			break;
	}
	return is;
}

#define SPLIT(src,x)  (istream_iterator<splitstring<x>>(stringstream(src)))\
								, istream_iterator<splitstring<x>>()

#define SPLIT_SPACE(src) (istream_iterator<string>(stringstream(src)))\
								, istream_iterator<string>()

typedef void(*MSG_REPORT_CALLBACK)(const char *msg);
//为了便于进行积分谱与微分谱的转换，在读取屏蔽参数、剂量转换参数时，将在参数头部添加X和Y均为0的pair

//能谱来源有：地磁捕获辐射(TRP) | 银河宇宙射线(GCR) | SPE(太阳粒子事件)
//源数据（SOURCE）数据名称为：[TRP|GCR-[MIN|MAX]|SPE-[SPE72|...]]
//屏蔽能量转换因子定义：Xg/cm2_PARTICLE，其中"Xg/cm2"通过在myShieldParaPath中读取获得
//剂量转换因子定义：[male|female]_ORGAN_PARTICLE
//其中剂量名称这样定义：[male|female]_ORGAN<_SOURCE<_PARTICLE>> 分三级
//PARTICLE取"Proton" "P" "He" "Li" "Be" "B" "C" "N" "O" "F" "Ne" "Na" "Mg" "Al" "Si" "Fe"之一
//ORGAN取"marrow" "brain" "skin" "heart" "lens" "uterus" "thymus"之一 
//依次为：红骨髓、大脑、皮肤、心脏、晶状体、子宫、胸腺
//SOURCE取"TRP" "GCR" "SPE" 之一
//尖括号表示在剂量转换阶段才有，其他的是各个阶段均有的
//这里，太阳粒子事件的能谱应能配置，具体在SPE_spectra.txt文件读取时获得，总共有几组，那么就有那么多组的SPE
//其中 TRP 由MOERE计算获得
//TRP来源于"Media\MOERE_OUT\trpProt.flx"
//GCR来源于"Media\dose_data\GCR_spectra.txt"，按最小年、最大年分组
//SPE来源于"Media\dose_data\SPE_spectra.txt"，按SPE72、SPE89、SSPE72、SSPE89分组
//
//屏蔽转换因子来源于"Media\dose_data\shield_tran_paras.txt"，按屏蔽厚度->粒子类型分组
//剂量转换因子来源于"Media\dose_data\dose_tran_paras.txt"，按粒子类型->性别->器官分组（器官时一行排开的）
class DoseCalculator
{
public:
	DoseCalculator::DoseCalculator()
		:myTRPPath("Media\\MOERE_OUT\\trpProt.flx")
		, myGCRPath("Media\\dose_data\\GCR_spectra.txt")
		, mySPEPath("Media\\dose_data\\SPE_spectra.txt")
		, myShieldParaPath("Media\\dose_data\\shield_tran_paras.txt")
		, myDoseParaPath("Media\\dose_data\\dose_tran_paras.txt")
		, myDosTranExpandInt(0.01)
		, myCurShieldTransParaIdx(0)
		, myDurationNearEarth(10)
		, myDurationInterstellar(10)
		, myMsgReportor(nullptr)
		, myCurGCRIdx(0)
	{
	}

	~DoseCalculator();

	static set<std::string> sAllGenderName;
	static set<std::string> sAllSourceName;
	static set<std::string> sAllParticleName;
	static map<string, string> sOrganDict; //from english to chinese, also, the key is the one that used in the data of calculation core
	static set<string> sMaleOrgans;   //key name of organs in man
	static set<string> sFemaleOrgans; //key name of organs in woman

	std::string myTRPPath;
	std::string myGCRPath;
	std::string mySPEPath;
	std::string myShieldParaPath;
	std::string myDoseParaPath;

	std::vector<DualColData> mySPEDatas;
	std::vector<DualColData> myGCRDatas;
	std::vector<DualColData> myTRPDatas;


	std::vector<DualColData> myShieldTransParas;
	std::vector<string> myShiendOpts;
	

	std::vector<DualColData> myDoseTransParas;

	// extra parameters for calculation
	int myCurShieldTransParaIdx;
	map<string, int> mySPECounts;//each item stands for one kind of SPE, the value stands for the number of occurence of the SPE type accident
	map<string,double> myDoseResult;
	vector<string> myGCROpts;
	int myCurGCRIdx;

	bool myInterruptCalculation;
	bool myCalculationFinished;
	bool myCalculationSuccess;
	std::mutex myLock;
	MSG_REPORT_CALLBACK myMsgReportor;

	void SetMsgReportor(MSG_REPORT_CALLBACK callback) {
		myMsgReportor = callback;
	}

	void InitCalculation() {
		myInterruptCalculation = false;
		myCalculationFinished = false;
		myCalculationSuccess = false;
	}

	void InterruptCalculation() {
		myLock.lock();
		myInterruptCalculation = true;
		myLock.unlock();
	}

	bool CalcFinished() {
		return myCalculationFinished;
	}



	//mission parameters
	OMEREController myOMERE;
	double          myDurationNearEarth;
	double          myDurationInterstellar;

	//calculation parameters
	double	        myDosTranExpandInt;

	bool LoadAllData();

	bool LoadAllButTRP();

	bool LoadTRP();
	bool LoadGCR();
	bool LoadSPE();

	bool LoadShieldTrans();
	bool LoadDoseTrans();

	void AllCalc();

	bool CalcTRP();

	bool CalcDoses();

	bool CalcDoseFromOneSource(std::vector<DualColData> srcFlx
		, std::vector<pair<string, double>> &outDoses);

	double AccumulateDose(string gender, string organ = "", string source = "", string parType = "");

	void ClearAllResult() {
		myDoseResult.clear();
	}
};


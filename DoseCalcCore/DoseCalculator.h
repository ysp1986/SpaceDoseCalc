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
//Ϊ�˱��ڽ��л�������΢���׵�ת�����ڶ�ȡ���β���������ת������ʱ�����ڲ���ͷ�����X��Y��Ϊ0��pair

//������Դ�У��شŲ������(TRP) | ������������(GCR) | SPE(̫�������¼�)
//Դ���ݣ�SOURCE����������Ϊ��[TRP|GCR-[MIN|MAX]|SPE-[SPE72|...]]
//��������ת�����Ӷ��壺Xg/cm2_PARTICLE������"Xg/cm2"ͨ����myShieldParaPath�ж�ȡ���
//����ת�����Ӷ��壺[male|female]_ORGAN_PARTICLE
//���м��������������壺[male|female]_ORGAN<_SOURCE<_PARTICLE>> ������
//PARTICLEȡ"Proton" "P" "He" "Li" "Be" "B" "C" "N" "O" "F" "Ne" "Na" "Mg" "Al" "Si" "Fe"֮һ
//ORGANȡ"marrow" "brain" "skin" "heart" "lens" "uterus" "thymus"֮һ 
//����Ϊ������衢���ԡ�Ƥ�������ࡢ��״�塢�ӹ�������
//SOURCEȡ"TRP" "GCR" "SPE" ֮һ
//�����ű�ʾ�ڼ���ת���׶β��У��������Ǹ����׶ξ��е�
//���̫�������¼�������Ӧ�����ã�������SPE_spectra.txt�ļ���ȡʱ��ã��ܹ��м��飬��ô������ô�����SPE
//���� TRP ��MOERE������
//TRP��Դ��"Media\MOERE_OUT\trpProt.flx"
//GCR��Դ��"Media\dose_data\GCR_spectra.txt"������С�ꡢ��������
//SPE��Դ��"Media\dose_data\SPE_spectra.txt"����SPE72��SPE89��SSPE72��SSPE89����
//
//����ת��������Դ��"Media\dose_data\shield_tran_paras.txt"�������κ��->�������ͷ���
//����ת��������Դ��"Media\dose_data\dose_tran_paras.txt"������������->�Ա�->���ٷ��飨����ʱһ���ſ��ģ�
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


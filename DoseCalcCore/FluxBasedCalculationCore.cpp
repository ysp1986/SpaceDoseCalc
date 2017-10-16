#include "stdafx.h"
#include "FluxBasedCalculationCore.h"
#include <cassert>

FluxBasedCalculationCore::FluxBasedCalculationCore()
{
}


FluxBasedCalculationCore::~FluxBasedCalculationCore()
{
}


double FluxBasedCalculationCore::CalculateDose(double interpIntervallog10, DualColData & inFlux, DualColData & transPara)
{
	DualColData expandedDosTrans;
	if (!IntExpandData(interpIntervallog10, transPara, expandedDosTrans))
		throw std::string("Error calculating dose");
	DualColData fittedFlux;
	if (!FitData(inFlux, expandedDosTrans, fittedFlux))
		throw std::string("Error calculating dose");
	assert(fittedFlux.Size() == expandedDosTrans.Size());
	DualColData diffFlux;
	//处理fittedFlux为微分能谱 diffFlux.Y(i) = fittedFlux.Y(i)- fittedFlux.Y(i+1)
	for (int i = 0; i < fittedFlux.Size(); ++i){
		if (i == fittedFlux.Size() - 1) {
			diffFlux.AddData(fittedFlux.X(i),fittedFlux.Y(i));
		}else
			diffFlux.AddData(fittedFlux.X(i), fittedFlux.Y(i)- fittedFlux.Y(i+1));
	}
	//乘
	double retDose = 0.0;
	for (int i = 0; i < diffFlux.Size(); ++i){
		retDose += diffFlux.Y(i) * expandedDosTrans.Y(i);
	}
	
	return retDose;
}


bool FluxBasedCalculationCore::ShieldingTrans(DualColData & inFlux, DualColData & shieldParam, DualColData & outFlux)
{
	DualColData tmpData;
	if (shieldParam.Size() > inFlux.Size()) {
		DualColData fittedFlux;
		if (!FitData(inFlux, shieldParam, fittedFlux))
			throw std::string("Error shielding calculation");

		for (int i = 0; i < fittedFlux.Size(); ++i) {
			tmpData.AddData(shieldParam.Y(i), fittedFlux.Y(i));
		}
	}
	else {
		DualColData newShieldParam;
		if (!FitData(shieldParam, inFlux, newShieldParam))
			throw std::string("Error shielding calculation");
		for (int i = 0; i < inFlux.Size(); ++i) {
			tmpData.AddData(newShieldParam.Y(i), inFlux.Y(i));
		}
	}
	

	//merge values with the same erg
	outFlux.Clear();
	outFlux.myName = inFlux.myName;
	outFlux.AddData(tmpData.X(0), tmpData.Y(0));
	for (int i = 1; i < tmpData.Size(); ++i) {
		if (tmpData.X(i) == outFlux.X(outFlux.Size() - 1)) {
			outFlux.Y(outFlux.Size() - 1) = tmpData.Y(i);
			continue;
		}
		else {
			outFlux.AddData(tmpData.X(i),tmpData.Y(i));
		}
	}

	return false;
}


bool FluxBasedCalculationCore::FitData(DualColData & inFlux, DualColData &ergStruct, DualColData & outFlux)
{
	assert(&outFlux != &ergStruct);
	inFlux.Sort();
	ergStruct.Sort();
	outFlux.Clear();

	if (ergStruct.X(0) > inFlux.X(0))
		std::cout << "warning: the first entry of the output higher than input, data loss" << std::endl;
	
	outFlux.myName = inFlux.myName;
	for (int i = 0; i < ergStruct.myData.size(); ++i)
	{
		if (ergStruct.X(i) <= inFlux.X(0)) {
			outFlux.AddData(ergStruct.X(i),inFlux.Y(0));
			continue;
		}
		if(ergStruct.X(i) >= inFlux.X(inFlux.myData.size()-1)) {
			outFlux.AddData(ergStruct.X(i), inFlux.Y(inFlux.Size() - 1));
			continue;
		}
		int l = 0;
		int r = inFlux.myData.size() - 1;
		while (r > l+1) {
			int mid = (l + r) / 2;
			if (inFlux.X(mid) > ergStruct.X(i))
				r = mid;
			else
				l = mid;
		}
		//interpolate
		double x1 = inFlux.X(l) > 1e-20 ? inFlux.X(l) : 1e-20;
		double x2 = inFlux.X(r) > 1e-20 ? inFlux.X(r) : 1e-20;
		double x = ergStruct.X(i) > 1e-20 ? ergStruct.X(i) : 1e-20;
		double y1 = inFlux.Y(l);
		double y2 = inFlux.Y(r);
		x1 = log(x1);
		x2 = log(x2);
		//y1 = log(y1);
		//y2 = log(y2);
		x = log(x);
		double y = (y1 + (x - x1)*(y2-y1) / (x2 - x1));
		outFlux.AddData(ergStruct.X(i), y);
	}
	return true;
}

bool FluxBasedCalculationCore::IntExpandData(double interpIntervalLog10, DualColData & inData, DualColData & outData) {
	//the logic is, keep the original point, evenly insert points between original 
	//intervals to make the after intervals all less than interpIntervalLog10
	outData.myName = inData.myName;
	assert(&inData != &outData);
	inData.Sort();
	outData.Clear();

	if (interpIntervalLog10 < 1e-10) {
		outData = inData;
		return true;
	}

	//get the structure first
	DualColData tmpData;
	if (inData.Size() < 1)
		return true;
	tmpData.AddData(inData.X(0),0.0);
	for (int i = 0; i < inData.Size()-1; ++i) {
		double X0 = inData.X(i)>0.0? inData.X(i):pow(10,-5* interpIntervalLog10);
		double X1 = inData.X(i+1);
		if (X0 > 0.0) {
			int insertNum = 1;
			while ((log10(X1) - log10(X0)) / insertNum > interpIntervalLog10)
				insertNum++;
			double realInt = pow(10, (log10(X1) - log10(X0)) / insertNum);
			double interv = X0;
			double tmpX = X0;
			for (int i = 0; i < insertNum - 1; ++i) {
				tmpX *= realInt;
				tmpData.AddData(tmpX, 0.0);
			}
		}
		tmpData.AddData(X1, 0.0);
	}
	std::cout << tmpData.Size() << "\t";
	//integer them
	/*DualColData tmpInData = inData;
	for (int i = tmpInData.Size() - 2; i >= 0; --i) {
		tmpInData.Y(i) = tmpInData.Y(i) + tmpInData.Y(i + 1) ;
	}*/
	//fit cur dose to the structure
	if (!FitData(inData, tmpData, outData))
		return false;
	//turn into diff version
	/*for (int i = 0; i < outData.Size()-1; ++i) {
		outData.Y(i) = outData.Y(i) - outData.Y(i+1);
	}*/
	return true;
}


double FluxBasedCalculationCore::CalculateDoseFunc(double interpIntervalLog10, DualColData & inData, DualColData & transPara, std::string & resName)
{
	return 0.0;
}


bool FluxBasedCalculationCore::ShieldingTransFunc(DualColData & inFlux, DualColData & shieldParam, DualColData & outFlux)
{
	return false;
}


bool FluxBasedCalculationCore::FitDataFunc(DualColData & inData, DualColData & ergStruct, DualColData & outFlux)
{
	return false;
}

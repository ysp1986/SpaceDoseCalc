#pragma once
#include "DualColData.h"
#include <string>

class FluxBasedCalculationCore
{
public:
	FluxBasedCalculationCore();
	~FluxBasedCalculationCore();
	static double CalculateDose(double interpIntervallog10, DualColData & inFlux, DualColData & transPara);
	static bool ShieldingTrans(DualColData & inFlux, DualColData & shieldParam, DualColData & outFlux);
	static bool FitData(DualColData & inData, DualColData & ergStruct, DualColData & outFlux);

	static bool IntExpandData(double interpIntervallog10, DualColData & inData, DualColData & outData);

	static double CalculateDoseFunc(double interpIntervallog10, DualColData & inData, DualColData & transPara, std::string & resName);
	static bool ShieldingTransFunc(DualColData & inFlux, DualColData & shieldParam, DualColData & outFlux);
	static bool FitDataFunc(DualColData & inData, DualColData & ergStruct, DualColData & outFlux);
};


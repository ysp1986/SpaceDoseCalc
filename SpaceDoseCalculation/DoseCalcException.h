#pragma once
#include <exception>
class DoseCalcException :
	public std::exception
{
public:
	DoseCalcException(const char* u8Message) :std::exception((std::string(u8"��������������") + u8Message).data()) {

	}
	~DoseCalcException();
};


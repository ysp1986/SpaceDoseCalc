#pragma once
#include <exception>
class DoseCalcException :
	public std::exception
{
public:
	DoseCalcException(const char* u8Message) :std::exception((std::string(u8"º¡¡øº∆À„≥Ã–Ú¥ÌŒÛ£∫") + u8Message).data()) {

	}
	~DoseCalcException();
};


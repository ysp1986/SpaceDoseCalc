#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class DualColData
{
public:
	DualColData(const char *name) :myName(name) {
	}
	DualColData(const DualColData &other):myName(other.myName) {
		myData.clear();
		myData = other.myData;
	}

	DualColData();

	~DualColData();
	std::string myName;
	std::vector<std::pair<double, double>> myData;

	void AddData(double a, double b) {
		try {
			myData.push_back(std::make_pair(a, b));
		}
		catch (...) {
			std::cout << "out of memory" << std::endl;
		}
	}

	static bool Less(std::pair<double, double> a, std::pair<double, double> b) {
		return a.first < b.first;
	}

	void Sort() {
		std::sort(myData.begin(), myData.end(), DualColData::Less);
	}

	double &X(unsigned int i) {
		return myData[i].first;
	}

	double &Y(unsigned int i) {
		return myData[i].second;
	}

	unsigned int Size() {
		return myData.size();
	}

	void Clear() {
		myData.clear();
	}

	void operator *(double mul) {
		for (auto &a : myData) {
			a.second *= mul;
		}
	}
};


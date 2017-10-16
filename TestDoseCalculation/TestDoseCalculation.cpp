// TestDoseCalculation.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <dlib/optimization.h>
#include <iostream>
#include <vector>


using namespace std;
using namespace dlib;

// ----------------------------------------------------------------------------------------
typedef matrix<double, 1, 1> invec;
typedef matrix<double, 4, 1> parvec;



bool ReadXYs(const char *filename, std::vector<pair<invec, double> > &data) {
	ifstream input(filename);
	if (!input.good())
		return false;
	input.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try{
		while (input.good())
		{
			double x, y,dum;
			input >> x >> y;
			data.push_back(make_pair(invec({ x }), y));
		}
	}catch (std::ifstream::failure e) {
		
		std::cerr << "Exception opening/reading/closing file:"<< e.what()<<"\n";
		return false;
	}

	return true;
}

int main()
{
	std::vector<pair<invec, double>> data;
	ReadXYs("GCR",data);
	/*parvec testpar({-2.0,3.0,2.1,3.2});*/
	for (auto &adata : data) {
		adata.first(0) = log(adata.first(0));
		adata.second = log(adata.second+0.1);
	}
	parvec pars;
	pars = parvec({ 1e-6,  1e-6, 1e-6,1e-6 });
	solve_least_squares_lm(objective_delta_stop_strategy(1e-7,10000).be_verbose(),
		residual,
		residual_derivative,//residual_derivative,
		data,
		pars,100);
	cout << pars(0) << ":" << pars(1) << ":" << pars(2) << ":" << pars(3) << endl;
	ofstream outf("output.txt");
	for (auto adata : data) {
		cout<< adata.first(0)<<"	"<< adata.second<<"	" << model(adata.first, pars) << endl;
		outf << adata.first(0) << "	" << adata.second << "	" << model(adata.first, pars) << endl;
	}
    return 0;
}


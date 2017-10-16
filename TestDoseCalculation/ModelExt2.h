#pragma once
#include <dlib/optimization.h>
#include <iostream>
#include <vector>


using namespace std;
using namespace dlib;

class ModelExt2
{
public:
	ModelExt2();
	~ModelExt2();

	typedef matrix<double, 1, 1> invec;
	typedef matrix<double, 4, 1> parvec;

	// We will use this function to generate data.  It represents a function of 2 variables
	// and 3 parameters.   The least squares procedure will be used to infer the values of 
	// the 3 parameters based on a set of input/output pairs.
	static double model(
		const invec& input,
		const parvec& params
	)
	{
		const double a = params(0);
		const double b = params(1);
		const double c = params(2);
		const double d = params(3);

		const double x = input(0);

		const double ret = a*exp(b*x) + c*exp(d*x);

		return ret;
	}

	// ----------------------------------------------------------------------------------------

	// This function is the "residual" for a least squares problem.   It takes an input/output
	// pair and compares it to the output of our model and returns the amount of error.  The idea
	// is to find the set of parameters which makes the residual small on all the data pairs.
	static double residual(
		const pair<invec, double>& data,
		const parvec& params
	)
	{
		return model(data.first, params) - data.second;
	}

	// ----------------------------------------------------------------------------------------

	// This function is the derivative of the residual() function with respect to the parameters.
	static parvec residual_derivative(
		const std::pair<invec, double>& data,
		const parvec& params
	)
	{
		parvec der;

		const double a = params(0);
		const double b = params(1);
		const double c = params(2);
		const double d = params(3);

		const double x = data.first(0);

		double tmpmodel = a*exp(b*x) + c*exp(d*x);

		/*der(0) = exp(b*x) *2 * tmpmodel;
		der(1) = a*x*exp(b*x) * 2 * tmpmodel;
		der(2) = exp(d*x) * 2 * tmpmodel;
		der(3) = c*d*exp(d*x) * 2 * tmpmodel;*/

		/*der(0) = exp(b*x);// *2 * tmpmodel;
		der(1) = a*x*exp(b*x);// * 2 * tmpmodel;
		der(2) = exp(d*x);// * 2 * tmpmodel;
		der(3) = c*d*exp(d*x);// * 2 * tmpmodel;*/
		der(0) = exp(b*x);// *2 * tmpmodel;
		der(1) = a*x*exp(b*x);// * 2 * tmpmodel;
		der(2) = exp(d*x);// * 2 * tmpmodel;
		der(3) = c*d*exp(d*x);// * 2 * tmpmodel;
		return der;
	}

	static bool Fit(std::vector<pair<invec, double>> &data, parvec &pars) {
		pars = parvec({ 1e-6,  1e-6, 1e-6,1e-6 });
		solve_least_squares_lm(objective_delta_stop_strategy(1e-7, 10000).be_verbose(),
			residual,
			residual_derivative,//residual_derivative,
			data,
			pars, 1e-2);
	}
};


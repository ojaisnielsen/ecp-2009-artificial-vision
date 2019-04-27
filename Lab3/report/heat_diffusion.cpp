#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include "CImg.h"
#define _USE_MATH_DEFINES
#include "math.h"
using namespace cimg_library;

CImg<float> GaussianMask(float _sigma, int _radius)
{
	//	creates an empty image that will contains the Gaussian mask
	CImg<float> mask(2*_radius+1, 2*_radius+1);

	//	computes the value of sigma² only once
	float sigm2= 2*_sigma*_sigma;
	float norm(0);	//	normalization term to have a distribution (sum == 1)

	//	initializes the mask
	for(int i=0 ; i<mask.dimx() ; i++)
	{
		//	Compute the value of x², that will remain constant during the entire loop on y
		int x2 = i-_radius;
		x2 = x2*x2;
		for(int j=0 ; j<mask.dimy() ; j++)
		{
			int y = j-_radius;

			//	the normalization term of the Gaussian is not used, because it will
			//	disappear with mask normalization (it is a constant)
			mask(i, j) = exp(-(x2+y*y)/sigm2);

			//	update the normalization term
			norm += mask(i, j);
		}
	}

	//	normalization, by using operator /= from the image class
	mask /= norm;

	return mask;
}

void GaussianFilter(CImg<float> &_out, const CImg<float> &_in, float _sigma, int _radius)
{
	//	creates the Gaussian mask
	CImg<float> mask = GaussianMask(_sigma, _radius);

	//	apply the convolution
	_out = _in.get_convolve(mask);
}

CImg<float> LaplacianMask()
{
	const char *const values = "0,1,0,1,-4,1,0,1,0";
	//laplacian mask
	CImg<float> mask(3, 3, 1, 1, values, 0);

	return mask;
}

void HeatDiffusion(CImg<float> &_out, const CImg<float> &_in, float _step, int _nbIteration)
{
	CImg<float> mask = LaplacianMask();
	_out = _in;
	for (int i = 0; i < _nbIteration; i++)
	{
		_out += _step * _out.get_convolve(mask);
	}

}


int main(int argc, char *argv[])
{

	char *input = argv[1];
	CImg<float> img_raw(input);
	img_raw.resize(img_raw.dimx(), img_raw.dimy(), 1, 1);
	CImg<float> img_gauss(img_raw);
	CImg<float> img_heat(img_gauss);

	CImgList<float> list(2);
	std::stringstream output0;
	std::stringstream output1;

	int nbIteration;
	float step;
	float sigma = 2;
	int radius = 3 * sigma;

	std::cout << "How many iterations for the heat equation : ";
	std::cin >> nbIteration;
	std::cout << "Time step : ";
	std::cin >> step;

	GaussianFilter(img_gauss, img_raw, sigma, radius);
	HeatDiffusion(img_heat, img_raw, step, nbIteration);


	output0 << "Gauss_sigma_" << sigma << "_" << input;
	output1 << "Heat_eq_Iterations_" << nbIteration << "_Step_" << step << "_" << input;
	img_gauss.save(output0.str().c_str());
	img_heat.save(output1.str().c_str());

	list[0] = img_gauss;
	list[1] = img_heat;

	CImgDisplay main_disp(list, "Gaussian vs. Heat equation");
	while(!main_disp.is_closed)
	{
		main_disp.wait();
	}

	return 0;
}
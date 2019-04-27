#include "stdafx.h"
#include <iostream>
#include <string>
#include <sstream>
#include "CImg.h"
#define _USE_MATH_DEFINES
#include "math.h"
using namespace cimg_library;
using namespace std;


float gaussianDist(float r, float deviation)
{
	return (exp(-(r*r)/(2*deviation*deviation)))/(deviation*sqrt(2*M_PI));
}


CImg<float> gaussianKernel(int radius, float deviation)
{
	CImg<float> mask(2*radius+1, 2*radius+1);
	int i, j;
	float r;
	float I;
	for (i = -radius; i <= radius; i++)
	{
		for (j = -radius; j <= radius; j++)
		{
			r = i*i + j*j;
			r = sqrt(r);
			I = gaussianDist(r, deviation);
			mask(i + radius, j + radius) = I;
		}
	}
	mask /= mask.norm(1);
	return mask;
}


int main(int argc, char *argv[])
{

	char *fileName = argv[1];
	stringstream noisyFileName;
	stringstream outputFileName;
	int radius;
	float deviation;
	string addNoise;
	int noisePower;
	string save;
	CImg<float> img(fileName);
	CImg<float> mask;
	CImg<float> output;
	CImgDisplay firstDisp;
	CImgDisplay secondDisp;

	cout << "Gaussian blur on " << fileName << endl << "Add noise ? (y/n) ";
	cin >> addNoise;
	if (addNoise == "y")
	{
		cout << "Noise power : ";
		cin >> noisePower;
		img.noise(noisePower);
	}
	cout << "Kernel radius : ";
	cin >> radius;
	cout << "Standard deviation : ";
	cin >> deviation;
	cout << "Save modified images ? (y/n) ";
	cin >> save;


	mask = gaussianKernel(radius, deviation);
	output = img.get_convolve(mask);
	firstDisp << img;
	firstDisp.set_title("Before gaussian filter");
	secondDisp << output;
	secondDisp.set_title("After gaussian filter");

	if (save == "y")
	{

		if (addNoise == "y")
		{
			noisyFileName << "noisy_" << fileName;
			img.normalize(0, 254);
			img.save(noisyFileName.str().c_str());
		}
		outputFileName << "blured_" << fileName;
		output.normalize(0, 254);
		output.save(outputFileName.str().c_str());
	}

	while (!firstDisp.is_closed || !secondDisp.is_closed)
	{
		if (!firstDisp.is_closed)
			firstDisp.wait();
		if (!secondDisp.is_closed)
			secondDisp.wait();
	}
	return 0;
}


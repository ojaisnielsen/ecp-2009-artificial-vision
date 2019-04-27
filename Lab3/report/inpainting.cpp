#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "CImg.h"
#define _USE_MATH_DEFINES
#include "math.h"
using namespace cimg_library;

CImg<float> HorGradientMask()
{
	const char *const values = "-1,0,1";
	CImg<float> mask(1, 3, 1, 1, values, 0);

	return mask;
}

CImg<float> VerGradientMask()
{
	const char *const values = "-1,0,1";
	CImg<float> mask(3, 1, 1, 1, values, 0);

	return mask;
}

float paramK;
float g(float _s)
{
	if (_s != 0)
	{
		return paramK / _s;
	}
	else
	{
		return 100000000000;
	}
}

void put_vals(CImg<float> &_out, const CImg<float> &_in, std::vector<int> _area)
{
	int x, y;
	for (int i = 0; i < _area.size(); i += 2)
	{
		x = _area[i];
		y = _area[i+1];
		_out(x, y) = _in(x, y);
	}
}

void Inpainting(CImg<float> &_out, const CImg<float> &_in, int _nbIt, float _paramK)
{

	CImg<float> horMask = HorGradientMask();
	CImg<float> verMask = VerGradientMask();
	CImg<float> img_horGrad(_in);
	CImg<float> img_verGrad(_in);
	CImg<float> img_gradMagn(_in);

	paramK = _paramK;

	int w = _in.dimx();
	int h = _in.dimy();
	std::vector<int> area;
	CImg<float> temp(_in);
	_out = _in;
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (_in(x, y) == 255)
			{
				area.push_back(x);
				area.push_back(y);
			}
		}
	}
	for (int i = 0; i < _nbIt; i++)
	{
		img_horGrad = _out.get_convolve(horMask);
		img_verGrad = _out.get_convolve(verMask);
		img_gradMagn = img_horGrad.get_sqr() + img_verGrad.get_sqr();
		img_gradMagn.sqrt();
		img_gradMagn.apply(g);
		img_horGrad.mul(img_gradMagn);
		img_verGrad.mul(img_gradMagn);
		img_horGrad.convolve(horMask);
		img_verGrad.convolve(verMask);
		temp += 0.2 * (img_horGrad + img_verGrad);
		put_vals(_out, temp, area);
		temp = _out;
		std::cout << "Itération : " << i << std::endl;
	}
}




int main(int argc, char *argv[])
{

	char *input = argv[1];
	CImg<float> img_raw(input);
	img_raw.resize(img_raw.dimx(), img_raw.dimy(), 1, 1);
	CImg<float> img_corrected(img_raw);

	CImgList<float> list(2);
	std::stringstream output;

	int nbIteration;
	float K;


	std::cout << "How many iterations for the inpainting : ";
	std::cin >> nbIteration;
	std::cout << "K : ";
	std::cin >> K;


	Inpainting(img_corrected, img_raw, nbIteration, K);


	output << "Inpainting_iter" << nbIteration << "_K_" << K << "_" << input;
	img_corrected.save(output.str().c_str());


	list[0] = img_raw;
	list[1] = img_corrected;

	CImgDisplay main_disp(list, "Inpainting");
	while(!main_disp.is_closed)
	{
		main_disp.wait();
	}

	return 0;
}
#include "..\..\CImg-1.3.0\CImg.h"	//	relative path of the CImg file
//	the namespace permits to use directly CImg<float> instead of having to specify
//	cimg_library::CImg<float> each time an image is used
using namespace cimg_library;

#define _USE_MATH_DEFINES	//	defines the value for pi
#include "math.h"	//	mathematical functions (exponential)
#include <iostream>	//	for input and output on command line


/*!
\brief create a gaussian mask
\param _sigma	sigma for distribution
\param _radius	radius of mask (final size is (2_radius+1)x(2_radius+1))
\return the gaussian mask
*/
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

/*!
\brief apply a gaussian filter to an image
\param _in		input image (adress of parameter is given in order to avoid copy)
\param _out		output image (adress of parameter is given in order to avoid copy)
\param _sigma	sigma for distribution
\param _radius	radius of mask (final size is (2_radius+1)x(2_radius+1))
*/
void GaussianFilter(CImg<float> &_out, const CImg<float> &_in, float _sigma, int _radius)
{
	//	creates the Gaussian mask
	CImg<float> mask = GaussianMask(_sigma, _radius);

	//	apply the convolution
	_out = _in.get_convolve(mask);
}

int main()
{
	//	open the raw image
	CImg<float> img_raw("../images/lena.png");
	//	set the smooth image as a copy of the previous one (easier to have the same size)
	CImg<float> img_smooth(img_raw);

	//	add noise to raw image
//	img_raw.noise(10);

	//	read Gaussian parameters
	float sigma(0);
	int radius(0);

	//	creates one display window by image
	CImgDisplay main_disp(img_raw, "raw image");
	CImgDisplay main_smooth(img_smooth, "filtered image");

	while(!main_disp.is_closed && !main_smooth.is_closed)
	{
		//	read new parameters of filter
		std::cout << "Sigma : ";
		std::cin >> sigma;
		std::cout << "Radius : ";
		std::cin >> radius;
		std::cout << std::endl;

		//	filter the image
		GaussianFilter(img_smooth, img_raw, sigma, radius);

		//	update view
		main_smooth << img_smooth;
	}

	return 0;
}


//	an other way to do that, using CImgList, that allow to keep a set of images
/*int main()
{
	//	open the raw image
	CImg<float> img_raw("../images/lena.png");
	//	set the smooth image as a copy of the previous one (easier to have the same size)
	CImg<float> img_smooth(img_raw);

	//	add both images inside a CImgList
	CImgList<float> list(img_raw, img_smooth);

	//	add noise to raw image
//	img_raw.noise(10);

	//	read Gaussian parameters
	float sigma(0);
	int radius(0);

	//	creates one display only
	CImgDisplay main_disp(list);

	while(!main_disp.is_closed )
	{
		//	read new parameters of filter
		std::cout << "Sigma : ";
		std::cin >> sigma;
		std::cout << "Radius : ";
		std::cin >> radius;
		std::cout << std::endl;

		//	filter the image
		GaussianFilter(img_smooth, img_raw, sigma, radius);

		//	update value inside the list
		list[1] = img_smooth;

		//	update view
		main_disp << list;
	}

	return 0;
}*/
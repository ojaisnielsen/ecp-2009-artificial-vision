#include "CImg.h"	//	relative path of the CImg file
//	the namespace permits to use directly CImg<float> instead of having to specify
//	cimg_library::CImg<float> each time an image is used
using namespace cimg_library;

#define _USE_MATH_DEFINES	//	defines the value for pi
#include "math.h"	//	mathematical functions (exponential)
#include <iostream>	//	for input and output on command line
#include "CIntensityProfile.h"

/*!
\brief create a gaussian mask
\param _sigma	sigma for distribution
\param _radius	radius of mask (final size is (2_radius+1)x(2_radius+1))
\return the gaussian mask
*/
CImg<float> GaussianMask(float _sigma, int _radius)
{
	return NULL;
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

}

/*!
\brief apply anisotropic diffusion filter to an image
\param _in		input image (adress of parameter is given in order to avoid copy)
\param _out		output image (adress of parameter is given in order to avoid copy)
\param _nbIt	number of iterations
\param _paramK	influences the performance of the algorithm with respect to |Du|
*/
void AnisotropicDiffusion(CImg<float> &_out, const CImg<float> &_in, int _nbIt, float _paramK)
{
	
}

/*!
\brief apply total variation filter to an image
\param _in		input image (adress of parameter is given in order to avoid copy)
\param _out		output image (adress of parameter is given in order to avoid copy)
\param _nbIt	number of iterations
\param _paramK	weight of div(Du/|Du|)
*/
void TotalVariationFlow(CImg<float> &_out, const CImg<float> &_in, int _nbIt, float _paramK)
{
	
}



//	Main function
int main()
{
	//	open the raw image
	CImg<float> img_raw("..\\_data\\barbara_noise.bmp");
	//	set the smooth image as a copy of the previous one (easier to have the same size)
	CImg<float> img_smooth(img_raw);

	//	add both images inside a CImgList
	CImgList<float> list(img_raw, img_smooth);

	//	add noise to raw image
	//img_raw.noise(10);

	// Initialisation
	int		algo=0;
	bool	algo_done = false;
	int		dim = 0;
	int		indiceRC = -1;

	//	Algorithm Parameters
	int		nbIteration;
	float	sigma=0;
	int		radius=0;
	float	paramK_aniso;
	float	paramK_TV;
	
	//	Creates display only
	CImgDisplay main_disp(list,"Raw vs. Corrected");

	while(!main_disp.is_closed)
	{
		// read info from command window
		while (algo_done == false)
		{
			std::cout << "Choice of the algorithm (Gaussian Filter=1; Anisotropic Diffusion=2; TV_flow=3) : ";
			std::cin >> algo;
			if ( algo == 1)
			{
				// Read new parameters
				std::cout << "sigma : ";
				std::cin >> sigma;
				std::cout << "radius: ";
				std::cin >> radius;
				std::cout << std::endl;

				// Gaussian Filter
				GaussianFilter(img_smooth, img_raw, sigma, radius);
				
				algo_done = true;
			}
			else if ( algo == 2) 
			{
				// Read new parameters
				std::cout << "K : ";
				std::cin >> paramK_aniso;
				std::cout << "Iterations: ";
				std::cin >> nbIteration;
				std::cout << std::endl;
				
				// Anisotropic Filter
				AnisotropicDiffusion(img_smooth, img_raw, nbIteration, paramK_aniso);

				algo_done = true;
			}
			else if ( algo == 3)
			{
				// Read new parameters
				std::cout << "K : ";
				std::cin >> paramK_TV;
				std::cout << "Iterations: ";
				std::cin >> nbIteration;
				std::cout << std::endl;
				
				// Total Variation Flow
				TotalVariationFlow(img_smooth, img_raw, nbIteration, paramK_TV);

				algo_done = true;
			}
			else
				std::cout << "Incorrect choice\n";
		}
		
		//	update value inside the list
		list[1] = img_smooth;

		//	update view
		main_disp << list;
		
		// Profile on row or column ?
		while (dim != 1 && dim != 2)
		{
			std::cout << "row or col (1/2) : ";
			std::cin >> dim;
		}

		// Choice the indice of the row or column
		if (dim == 1)
			std::cout << "index of row: ";
		else
			std::cout << "index of col: ";		
		std::cin >> indiceRC;

		// Display Intensity Profile for one image (draw in red)
		visualizeIntensityProfile(img_raw, indiceRC,dim);

		// Display Intensity Profile for one image (draw in red and in green)
		visualizeIntensityProfile(img_raw, img_smooth, indiceRC,dim);

		algo_done = false;
		dim = 0;
		img_smooth.save("../_out/imageCorrected.bmp");
		img_smooth = img_raw;
	}

	return 0;
}
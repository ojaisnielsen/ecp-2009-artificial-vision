#include "CImg.h"	//	relative path of the CImg file
//	the namespace permits to use directly CImg<float> instead of having to specify
//	cimg_library::CImg<float> each time an image is used
using namespace cimg_library;

#define _USE_MATH_DEFINES	//	defines the value for pi
#include "math.h"	//	mathematical functions (exponential)
#include <iostream>	//	for input and output on command line
#include <vector>
#include "Blob.h"
#include "DisplayBlob.h"

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
	float fCte = 1.0f/(sigm2*M_PI);

	float fNorm = 0;

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
			mask(i, j) = fCte*exp(-(x2+y*y)/sigm2);
			fNorm += mask(i, j);
		}
	}

	mask /= fNorm;

	return mask;
}

/*!
\brief apply a gaussian filter to an image
\param _out		output image (adress of parameter is given in order to avoid copy)
\param _in		input image (adress of parameter is given in order to avoid copy)
\param _sigma	sigma for distribution
*/
void GaussianFilter(CImg<float> &_out, const CImg<float> &_in, float _sigma)
{
	int radius = int(3*_sigma);

	//	Two methods to compute Gaussian filter, only one should be used

	//	creates the Gaussian mask
//	CImg<float> mask = GaussianMask(_sigma, radius);
//	//	apply the convolution
//	_out = _in.get_convolve(mask);

	_out = _in.get_blur(_sigma, _sigma);
}

/*!
\brief apply a normalized laplacian filter to an image
\param _out		output image (adress of parameter is given in order to avoid copy)
\param _in		input image (adress of parameter is given in order to avoid copy)
\param _sigma	sigma for normalization
*/
void normalizedLaplacianFilter(CImg<float> &_out, const CImg<float> &_in, float _sigma)
{
	const char *const values = "0,1,0,1,-4,1,0,1,0";
	//laplacian mask
	CImg<float> _laplacianMask(3, 3, 1, 1, values, 0);

	//	apply the convolution
	_out = _in.get_convolve(_laplacianMask);
	_out *= _sigma*_sigma;	// normalization of the image after convolution or of the mask is the same
}

/*!
\brief find if a point is a local minimum or a maximum
\param _x,_y,_z		coordinates of point to consider (the scale id is assumed to be correct)
\param _pScale		table with scale images
\return true if given point (_x,_y,_z) is a local minimum or maximum
*/
bool IsMinMax(int _x, int _y, int _z, const CImg<float> *_pScale)
{
	//	compute the part of the image that may be used (for x,y)
	//	z value is assumed to be correct inside the scale table, and not on boundaries
	int xMin(_x-2), xMax(_x+2);
	int yMin(_y-2), yMax(_y+2);

	//	manages boundaries (intersection of neighborhood and image)
	if(xMin < 0)
		xMin = 0;
	if(yMin < 0)
		yMin = 0;
	if(xMax >= _pScale[_z].dimx())
		xMax = _pScale[_z].dimx()-1;
	if(yMax >= _pScale[_z].dimy())
		yMax = _pScale[_z].dimy()-1;

	float fMin, fMax;
	fMin = fMax = _pScale[_z](_x, _y);

	//	compute minimum and maximum for the neighborhood
	for(int z=_z-1 ; z<=_z+1 ; z++)
	{
		for(int y=yMin ; y<=yMax ; y++)
		{
			for(int x=xMin ; x<=xMax ; x++)
			{
				float fVal = _pScale[z](x,y);
				//	update min and max
				if(fVal < fMin)
					fMin = fVal;
				if(fVal > fMax)
					fMax = fVal;
			}
		}
	}

	//	point(_x,_y,_z) is a minimum or a maximum if its intensity equals either
	//	the minimum or maximum inside neighborhood
	return (fMin == _pScale[_z](_x, _y)) || (fMax == _pScale[_z](_x, _y));
}

int main()
{
	//	open the raw image
	CImg<float> img_raw("../images/papillon.bmp");

	//	read parameters
	float sigma(0);
	std::cout << "Initial Sigma : ";
	std::cin >> sigma;
	int num_scales(0);
	std::cout << "Number of scales used : ";
	std::cin >> num_scales;
	int thresh;
	std::cout << "threshold : ";
	std::cin >> thresh;
	std::cout << std::endl;

	//	tables with smoothed images (by Gaussian Filter) and laplacian values
	CImg<float> *smoothed = new CImg<float> [num_scales];
	CImg<float> *nLoGed   = new CImg<float> [num_scales];

	double factor = sqrt(2.0);
	float sigma_scale;

	//smooth images and apply the normalized laplacian
	for(int i=0; i<num_scales; i++)
	{
		sigma_scale = sigma*pow(factor,(double) i);

		GaussianFilter(smoothed[i], img_raw, sigma_scale);
		normalizedLaplacianFilter(nLoGed[i], smoothed[i], sigma_scale);
	}

	std::vector<Blob> vectBlob; // vector that will contain blobs

	//	compute blob at each level
	//	we are not going to look the first and the last image to avoid boundary problems
	for(int z=1; z<num_scales-1; z++)
	{
		sigma_scale = sigma*pow(factor,(double) z);
		for(int x=0; x<img_raw.dimx(); x++)
		{
			for(int y=0; y<img_raw.dimy(); y++)
			{
				//	add current point as a blob if it is a maximum or a minimum
				if((fabs(nLoGed[z](x, y))>thresh) && IsMinMax(x, y, z, nLoGed))
				{
					Blob tmp(x, y, sigma_scale);
					vectBlob.push_back(tmp);
				}
			}
		}
	}

	//	display blobs on initial image
	DisplayBlob main_disp(img_raw, vectBlob);
	main_disp.Launch("./blob.bmp");

	return 0;
}

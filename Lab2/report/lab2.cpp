
#include "stdafx.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "CImg.h"
#include "Blob.h"
#include "DisplayBlob.h"
#define _USE_MATH_DEFINES
#include "math.h"
using namespace cimg_library;
using namespace std;

float gaussianDist(float r, float deviation)
{
	return (exp(-(r*r)/(2*deviation*deviation)))/(deviation*sqrt(2*M_PI));
}


void putGaussianKernel(CImg<float> &mask, int radius, float deviation)
{
	float r;
	float I;
	for (int i = -radius; i <= radius; i++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			r = i*i + j*j;
			r = sqrt(r);
			I = gaussianDist(r, deviation);
			mask(i + radius, j + radius) = I;
		}
	}
	mask /= mask.norm(1);
}


int main(int argc, char *argv[])
{

	char *fileName = argv[1];
	stringstream outputFileName;
	float firstDeviation;
	int scalesNb;
	float threshold;
	string save;
	CImg<float> img(fileName);
	int w, h;
	w = img.dimx();
	h = img.dimy();

	cout << "Blob detection on " << fileName << endl << "First Gaussian filter deviation : ";
	cin >> firstDeviation;
	cout << "Number of scales : ";
	cin >> scalesNb;
	cout << "Significant blobs threshold : ";
	cin >> threshold;
	cout << "Save results ? (y/n) ";
	cin >> save;


	CImg<float> laplacianMask(3, 3);

	laplacianMask.fill(0);
	laplacianMask(0, 1) = 1;
	laplacianMask(1, 0) = 1;
	laplacianMask(1, 1) = -4;
	laplacianMask(1, 2) = 1;
	laplacianMask(2, 1) = 1;

	CImgList<float> gaussianMasks(scalesNb);
	CImgList<float> scaleSpace(scalesNb);
	float deviation = firstDeviation;
	int radius;

	laplacianMask *= deviation*deviation;
	for (int i = 0; i < scalesNb; i++)
	{
		radius = 3 * deviation;
		gaussianMasks[i].assign(2*radius+1, 2*radius+1);
		putGaussianKernel(gaussianMasks[i], radius, deviation);
		scaleSpace[i] = img.get_convolve(gaussianMasks[i]);
		// scaleSpace[i] = img.get_blur(deviation, deviation, 0);
		scaleSpace[i].convolve(laplacianMask);
		laplacianMask *= 2;
		deviation *= sqrt(2.);
	}
	

	float point;
	bool isBigger, isSmaller;
	vector<Blob> blobs;

	deviation = firstDeviation;
	for (int i = 1; i < scalesNb - 1; i++)
	{
		deviation *= sqrt(2.);
		for (int x = 1; x < w - 1; x++)
		{
			for (int y = 1; y < h - 1; y++)
			{
				point = scaleSpace[i](x,y);
				isBigger = true;
				isSmaller = true;
				for (int k = i - 1; k <= i + 1; k++)
				{
					for (int u = x - 1; u <= x + 1; u++)
					{
						for (int v = y - 1; v <= y + 1; v++)
						{
							if (k != i || u != x || v != y)
							{
								isBigger = isBigger && (point > scaleSpace[k](u,v));
								isSmaller = isSmaller && (point < scaleSpace[k](u,v));
							}
						}
					}
				}
				if ((isBigger && (point > threshold)) || (isSmaller && (point < threshold)))
					blobs.push_back(Blob(x, y, deviation));
			}
		}
	}

	int nBlobs = blobs.size();
	DisplayBlob window(img, blobs);

	if (save == "y")
	{
		stringstream outputFileName;
		outputFileName << "Blobs_" << nBlobs << "_Scales_" << scalesNb << "_InitDev_" << firstDeviation << "_Thres_" << threshold << "_" << fileName;
		window.Launch(outputFileName.str().c_str());
	}


	while (!window.is_closed)
	{
			window.wait();
	}

	return 0;
}




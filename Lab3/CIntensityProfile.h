#include "CImg.h"
using namespace cimg_library;

void visualizeIntensityProfile(const CImg<float> &_imgRed,
							   const CImg<float> &_imgGreen,
							   const CImg<float> &_imgBlue,
							   int indice,
							   int dim,
							   int flag = 3);

void visualizeIntensityProfile(const CImg<float> &_imgRed,
							   int indice,
							   int dim);

void visualizeIntensityProfile(const CImg<float> &_imgRed,
							   const CImg<float> &_imgGreen,
							   int indice,
							   int dim);
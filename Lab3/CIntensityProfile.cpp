#include "CIntensityProfile.h"
using namespace cimg_library;

// Function that draw/save a intensity profile at for maximum 3 different images.
void visualizeIntensityProfile(const CImg<float> &_imgRed,		// Image which profile will be draw in red
							   const CImg<float> &_imgGreen,	// Image which profile will be draw in green
							   const CImg<float> &_imgBlue,		// Image which profile will be draw in blue
							   int indice,						// Indice of the row/col
							   int dim,						// Indicate if we extract row or col
							   int flag)
{
	int size;
	int coordX0,coordY0,coordX1,coordY1;
 
	if (dim == 1) //row 
	{
		size = _imgRed.dimx();
		coordX0 = 0;coordY0 = indice;
		coordX1 = size-1;coordY1 = indice;
	}
	else if (dim == 2) //col
	{
		size = _imgRed.dimy();
		coordX0 = indice;coordY0 = 0;
		coordX1 = indice;coordY1 = size;
	}
	else
		return;

	// Create image for intensity profile
	CImg<float> visuProfile(size,256,1,3,0);
	
	// Create display
	CImgDisplay draw_disp(visuProfile,"Intensity profile");

	// Several color declaration
	const unsigned char red[] = { 255,0,0 }; 
	const unsigned char green[] = { 0,255,0 };
	const unsigned char blue[] = { 0,0,255 };
	const unsigned char white[] = { 255,255,255 };
	
	if ( (indice >=0) && indice <size ) // check if indice is correct
	{	
		visuProfile.fill(0);	// fill image to zero
		
		// draw graph in image
		if (flag == 3)
		{
			visuProfile.draw_graph(_imgRed.get_crop(coordX0,coordY0,0,0,coordX1,coordY1,0,0)	,red	,1,1,0,255,0);
			visuProfile.draw_graph(_imgGreen.get_crop(coordX0,coordY0,0,0,coordX1,coordY1,0,0)	,green	,1,1,0,255,0);
			visuProfile.draw_graph(_imgBlue.get_crop(coordX0,coordY0,0,0,coordX1,coordY1,0,0)	,blue	,1,1,0,255,0);
		}
		else if (flag == 2)
		{
			visuProfile.draw_graph(_imgRed.get_crop(coordX0,coordY0,0,0,coordX1,coordY1,0,0)	,red	,1,1,0,255,0);
			visuProfile.draw_graph(_imgGreen.get_crop(coordX0,coordY0,0,0,coordX1,coordY1,0,0)	,green	,1,1,0,255,0);
		}
		else if (flag == 1)
			visuProfile.draw_graph(_imgRed.get_crop(coordX0,coordY0,0,0,coordX1,coordY1,0,0)	,red	,1,1,0,255,0);
		else
			return;
		// draw axis in image
		visuProfile.draw_axis(.0f,size-1.0f,256.0f,.0f,white,0.8f);
		
		// transfer image to display
		visuProfile.display(draw_disp);	
	}
	else 
		return;
	
	// continue while window is not closed
	while (!draw_disp.is_closed)
		continue;

	// save profile in a file
	visuProfile.save("../_out/intensityProfile.bmp");

}

void visualizeIntensityProfile(const CImg<float> &_imgRed,
							   int indice,
							   int dim)
{
	visualizeIntensityProfile(_imgRed,_imgRed,_imgRed, indice,dim,1);
}

void visualizeIntensityProfile(const CImg<float> &_imgRed,
							   const CImg<float> &_imgGreen,
							   int indice,
							   int dim)
{
	visualizeIntensityProfile(_imgRed,_imgGreen,_imgRed, indice,dim,2);
}
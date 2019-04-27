/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIBECP_INCLUDED
#define LIBECP_INCLUDED

#include "EcpException.h"
#define _USE_MATH_DEFINES	//	defines the value for pi
#include <math.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

//A scaling parameter for the visualization of the optical flow.
const float sc = 5;

float	GetInterpolatedValue(const CImg<float> &image,float x, float y, int z );

/**
 * Load a set of images and put them in a CImgList
 * @param images the CImgList<float> in which the new images will be pushed back
 * @param path complete path with trailing / e.g: "/home/user/images/"
 * @param prefix prefix of the file names
 * @param indexMin the files should be indexed between indexMin and indexMax (included)
 * @param indexMax 
 * @param indexStep 
 * @param extension type of the files (without "."). e.g: "png", "jpg", "jpeg"...
 */
void loadImages( CImgList<float>& images, char* path, char* prefix, 
		 int indexMin, int indexMax, int indexStep, 
   		 char* extension )
{
    char fileName[1000];
    for( int index = indexMin; index <= indexMax; index += indexStep )
    {
        sprintf( fileName, "%s%s%d.%s", path, prefix, index, extension );
		
        // Create and load image
        CImg<float> newImg( fileName );
		
        // Put image in list
        images.push_back( newImg );
    }
}

/**
 * Display a list of images sequentially. Closing the window quits the function.
 * @param images 
 * @param delay delay in milliseconds between two images. 
 If delay < 0 then user has to clic to display the next image.
 * @param loop if loop is set to true then the image sequence will loop
 */
void displayImageSequence( CImgList<float>& images, int delay = -1, bool loop = false )
{
    int imageN = images.size;
    if( imageN == 0 )
        return;
	
    CImgDisplay disp( images[0], "Image sequence" );
    char title[100];
	
    for( int i = 0; i < imageN; i++ )
    {
        sprintf( title, "Image %d from sequence", i );
        disp.set_title( title );
		
        images[i].display( disp );
        if( delay > 0 )// wait indicated delay
            disp.wait( delay );
        else// wait for clic
        {
            disp.wait();
            while( !disp.button && !disp.is_closed )
                disp.wait();
        }
		
        // If we want to loop, restart
        if( loop && i == imageN-1 )
            i = 0;
		
        // If windows is closed, quit
        if( disp.is_closed )
        {
            loop = false;
            i = imageN-1;
        }
    }
}

/**
 * Visualize an images described by an optical flow field. 
 Arguments must have the same x and y dimensions. 
 Results are blurred by a 10x10 gaussian window in order to have a smoother field.
 * @param image 
 * @param optFlow 
 * @param disp result is returned in disp
 */
void visualizeOpticalFlow( const CImg<float>& image, const CImg<float>& optFlow, CImg<float>& disp )
{
    // Check arguments
    if( optFlow.dimx() != image.dimx() || optFlow.dimy() != image.dimy() || optFlow.dimx() != disp.dimx() || optFlow.dimy() != disp.dimy() )
        throw EcpException( "visualizeOpticalFlow: incompatible arguments" );
	
    int dimX = image.dimx();
    int dimY = image.dimy();
	
    // Blur the optical flow results
    // with a gaussian window
    int dimGaussianX = 10;
    int dimGaussianY = 10;
    float sigma = 2;
    float colorGaussian = 1;
    CImg<float> gaussian( dimGaussianX, dimGaussianY );
    gaussian.draw_gaussian( (dimGaussianX-1.0)/2.0, (dimGaussianY-1.0)/2.0, 
                            sigma, &colorGaussian );
    gaussian /= gaussian.sum();
    CImg<float> blurredSlice0, blurredSlice1;
    blurredSlice0 = optFlow.get_slice(0);
    blurredSlice0.correlate( gaussian );
    blurredSlice1 = optFlow.get_slice(1);
    blurredSlice1.correlate( gaussian );

    // Define the color of the arrows
    float red[3] = {255, 0, 0};
    float white[3] = {255, 255, 255};

    // Draw the arrows
    for( int x = 1; x < dimX-1; x += 10 )
    {
        for( int y = 1; y < dimY-1; y += 10 )
        {
            int x1 = x + (int)( sc*blurredSlice0( x, y ) );
            int y1 = y + (int)( sc*blurredSlice1( x, y ) );

            disp.draw_arrow( x, y, x1, y1, red, 45, -25 );
        }
    }
}

/**
 * Same as the previous function but for a sequence of images.
 * @param images The images have to be 3-channels images
 * @param optFlow 
 * @param delay same as in displayImageSequence function
 * @param loop same as in displayImageSequence function
 */
void visualizeOpticalFlow( const CImgList<float>& images, 
			   const CImgList<float>& optFlow,
      			   int delay = -1,
     			   bool loop = false )
{
    cout << "----------> Visualize the optical flow" << endl;
    // Check arguments
    if( optFlow.size != images.size-1 )
        throw EcpException( "visualizeOpticalFlow: incompatible arguments" );
    int imageN = images.size;
	int nbOpticalFlow = optFlow.size;
    if( imageN < 2 )
        throw EcpException( "visualizeOpticalFlow: not enough input images" );
    for( int i = 0; i < nbOpticalFlow ; i++ )
    {
        if( optFlow[i].dimx() != images[i].dimx() || optFlow[i].dimy() != images[i].dimy() )
            throw EcpException( "visualizeOpticalFlow: incompatible arguments" );
        if( optFlow[i].dimz() < 2 )
            throw EcpException( "visualizeOpticalFlow: invalid optical flow argument" );
        if( images[i].dimv() != 3 )
            throw EcpException( "visualizeOpticalFlow: only color images are accepted as input" );
    }
	
    // Blur the optical flow results
    // with a gaussian window
    cout << "-----> Smoothing the sequence..." << endl;
    int dimGaussianX = 10;
    int dimGaussianY = 10;
    float sigma = 2;
    float colorGaussian = 1;
    CImg<float> gaussian( dimGaussianX, dimGaussianY );
    gaussian.draw_gaussian( (dimGaussianX-1.0)/2.0, (dimGaussianY-1.0)/2.0, 
                            sigma, &colorGaussian );
    gaussian /= gaussian.sum();
    CImgList<float> optFlowBlurred( optFlow );
    CImg<float> blurredSlice0, blurredSlice1;
    // Actually this for loop can be commented out to accelerate computation
    for( int i = 0; i < nbOpticalFlow; i++ )
    {
        blurredSlice0 = optFlow[i].get_slice(0);
        blurredSlice0.correlate( gaussian );
        blurredSlice1 = optFlow[i].get_slice(1);
        blurredSlice1.correlate( gaussian );
        for( int x = 0; x < optFlow[i].dimx(); x++ )
        {
            for( int y = 0; y < optFlow[i].dimy(); y++ )
            {
                optFlowBlurred[i]( x, y, 0 ) = blurredSlice0(x,y);
                optFlowBlurred[i]( x, y, 1 ) = blurredSlice1(x,y);
            }
        }
    }
    cout << "-----> done." << endl;
	
    // Define the color of the arrows
    float red[3] = {255, 0, 0};
    float white[3] = {255, 255, 255};
	
    // Here we go
    CImgList<float> disp( images );
    for( int i = 0; i < nbOpticalFlow; i++ )
    {
        int dimX = disp[i].dimx();
        int dimY = disp[i].dimy();
        for( int x = 1; x < dimX-1; x += 10 )
        {
            for( int y = 1; y < dimY-1; y += 10 )
            {
                int x1 = x + (int)( sc*optFlowBlurred[i]( x, y, 0 ) );
                int y1 = y + (int)( sc*optFlowBlurred[i]( x, y, 1 ) );
				
                disp[i].draw_arrow( x, y, x1, y1, red, 45, -25 );
            }
        }
    }
	
    displayImageSequence( disp, delay, loop );
}

/**
 * This is the function you will have to complete in 
 order to compute the optical flow in a sequence of images.
 * @param images The images have to be gray-level values images
 * @param optFlow if this does not have the right size then it will be resized
 */
void computeOpticalFlow( const CImgList<float>& images, CImgList<float>& optFlow )
{
    //////////////////////////////
    // Check validity of arguments
    //////////////////////////////
	
    int imageN = images.size;
    // Check there are enough images to compute the optical flow
    if( imageN < 2 )
        throw EcpException( "computeOpticalFlow: not enough input images" );
	
    int dimX = images[0].dimx();
    int dimY = images[0].dimy();
	
    for( int i = 0; i < imageN; i++ )
    {
        // Check every image has the right size
        if( images[i].dimz() != 1 )
            throw EcpException( "computeOpticalFlow: the input images should not have a z dimension" );
        if( images[i].dimv() != 1 )
            throw EcpException( "computeOpticalFlow: the input images should be grey-level values" );
        if( images[i].dimx() != dimX || images[i].dimy() != dimY )
            throw EcpException( "computeOpticalFlow: the input images do not have all the same size" );
    }
    // Check the size of the corresponding optical flow image list
    // and if necessary resize it
    while( (int)optFlow.size < imageN-1 )
    {
        optFlow.push_back( CImg<float>( dimX, dimY, 2 ) );
        optFlow.back().fill(0);
    }
    for( int i = 0; i < imageN-1; i++ )
    {
        if( optFlow[i].dimx() != dimX || optFlow[i].dimy() != dimY || optFlow[i].dimz() < 2 )
            throw EcpException( "computeOpticalFlow: the input optical flow does not have a correct size" );
    }
	
    ///////////////////////
    // Compute optical flow
    ///////////////////////
    cout << "----------> Compute the optical flow" << endl;
	
	// Initialize derivative images
    CImg<float> Ix( dimX, dimY );
    CImg<float> Ix2( dimX, dimY );
    CImg<float> Iy( dimX, dimY );
    CImg<float> Iy2( dimX, dimY );
    CImg<float> IxIy( dimX, dimY );
    CImg<float> It( dimX, dimY );
    CImg<float> IxIt( dimX, dimY );
    CImg<float> IyIt( dimX, dimY );
	
    // Derivation filters
    CImg<float> maskX( 3, 1 );
    // TODO

    CImg<float> maskY( 1, 3 );
    // TODO

    // Gaussian kernel
    int dimGaussianX = 11;
    int dimGaussianY = 11;
    float sigma = 4;
    
    // Initialize M matrix and b, u vectors
    CImg<float> M( 2, 2 );
    CImg<float> b( 1, 2 );
    CImg<float> u( 1, 2 );	
}

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

    //	computes the valuegaussian of sigma² only once
    float sigm2= 2*_sigma*_sigma;
    float fCte = (float)(1.0/((double)sigm2*M_PI));

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
        }
    }

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
    //	creates the Gaussian mask
    CImg<float> mask = GaussianMask(_sigma, radius);

    //	apply the convolution
    _out = _in.get_convolve(mask);
}


/*!
  \brief SubSample optical flow
  \param _in		input flow (adress of parameter is given in order to avoid copy)
  \param _level	step of subsampling
*/
CImg<float> subSambleOptFlow(const CImg<float> &_in, int _level)
{
    CImg<float> _out( (_in.dimx())*_level,(_in.dimy())*_level,_in.dimz());
    for (int x=0; x<_out.dimx(); x++){
        for (int y=0; y<_out.dimy(); y++)
        {
            if ( ( x % _level ) || ( y % _level) )
            {
                _out(x,y,0) = GetInterpolatedValue(_in,(float)x/(float)_level,(float)y/(float)_level,0);
                _out(x,y,1) = GetInterpolatedValue(_in,(float)x/(float)_level,(float)y/(float)_level,1);
            }
            else
            {
                _out(x,y,0) = _in(x/_level,y/_level,0);
                _out(x,y,1) = _in(x/_level,y/_level,1);
            }
        }}
    return _out;
}
/*!
  \brief get interpolated value of an image from floating coordinate.
  \param image	input image (adress of parameter is given in order to avoid copy)
  \param x and y	coordinate
  \param z		slice indice
*/
float	GetInterpolatedValue(
    const CImg<float> &image,		///< Image de taille width * height
    float x, float y,		///< Coordonnées du pixel à interpoler
    int z
    )
{
    int width = image.width;
    int height = image.height;
	
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    //distance par rapport au centre du pixel (comprise entre 0 et 1)
    float dx = x - (float)x0;
    float dy = y - (float)y0;

    //application de la bilinéaire

    float value = 0.0f;

    //pixel principal
    if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
        value += (1.0f - dx) * (1.0f - dy) * image(x0,y0,z);//image[y0*width + x0];

    //pixel de droite
    if (x1 >= 0 && x1 < width && y0 >= 0 && y0 < height)
        value += dx * (1.0f - dy) * image(x1,y0,z);//image[y0*width + x1];

    //pixel en dessous
    if (x0 >= 0 && x0 < width && y1 >= 0 && y1 < height)
        value += (1.0f - dx) * dy * image(x0,y1,z);//image[y1*width + x0];

    //pixel en dessous à droie
    if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height)
        value += dx * dy * image(x1,y1,z);//image[y1*width + x1];

    return value;
}

//Create a warped image given an image and the optical flow.
//As base image we consider the oldest image of the pair.
CImg<float> warpImage(const CImg<float>& base, const CImg<float>& optFlow)
{
    int dimX = base.dimx();
    int dimY = base.dimy();

    float new_x;
    float new_y;
    CImg<float> warp_img(dimX, dimY);

    for(int x = 0; x < dimX; x++)
    {
        for(int y = 0; y < dimY; y++)
        {
            new_x = x - optFlow(x,y,0);
            new_y = y - optFlow(x,y,1);
            warp_img(x,y) = GetInterpolatedValue(base, new_x, new_y, 0);
        }
    }

    //Scale the values of the image into the interval [0,255]
    float mn = warp_img.min();
    float mx = warp_img.max();
    for(int x = 0; x < dimX; x++)
        for(int y = 0; y < dimY; y++)
            warp_img(x,y) = 255*(warp_img(x,y) - mn)/(mx - mn);

    return warp_img;
}


/*!
  \brief	display 2 images in a RGB window
  \param	imageRed	red channel of the final window
  \param	imageGreen	green channel of the final window
*/
void displayAndCompare2Images(const CImg<float>& imageRed, const CImg<float>& imageGreen)
{
    if ( (imageRed.dimx() != imageGreen.dimx()) || (imageRed.dimy() != imageGreen.dimy()) )
        throw EcpException("displayAndCompare2Images: incompatible size" );
	
    CImg<float> visuImage(imageRed.dimx(),imageRed.dimy(),1,3,0);

    for (int x=0; x<imageRed.dimx(); x++){
        for (int y=0; y<imageGreen.dimy(); y++)
        {
            visuImage(x,y,0,0) = imageRed(x,y);
            visuImage(x,y,0,1) = imageGreen(x,y);
        }
    }

    CImgDisplay draw_disp(visuImage,"Images");
	
    // continue while window is not closed
    while (!draw_disp.is_closed)
        continue;
}

#endif

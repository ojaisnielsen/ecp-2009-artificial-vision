/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


// Usual libraries
// #include <iostream>

// CImg library
#include "CImg.h"
using namespace cimg_library;

// Stl
#include <vector>
using namespace std;
 
// Custom libraries
#include "libECP.h"
			 
int main(int argc, char *argv[])
{
    /*if(argc != 6 && argc != 7)
    {
        cout << "Usage: " << argv[0] << " <data directory> <prefix> "
             << "<extension> <start frame> <end frame> [step]"
             << endl << endl
             << "  data directory: the directory where the images are in (including the last '/')"
             << endl
             << "  prefix:         the first part of the image file names (before the number)"
             << endl
             << "  extension:      the part after the dot (including the dot)"
             << endl
             << "  start frame:    the first frame to be loaded"
             << endl
             << "  end frame:      the last frame to be loaded"
             << endl
             << "  step:           how many frames to skip between 2 consecutive frames"
             << endl;

        exit(1);
    }
    int start_frame = atoi(argv[4]);
    int end_frame = atoi(argv[5]);
    int index_step = 1;
    if(argc == 7)
    {
        index_step = atoi(argv[6]);
    }*/
    CImgList<float> imagesColor;
    // if you use argv
	// loadImages( imagesColor, argv[1], argv[2], start_frame, end_frame, index_step, argv[3] );
	// or 
	// complete the path to the directory where you donwloaded the datasets
	// for example
		loadImages( imagesColor, "../taxi/", "taxi", 1, 5, 4, "bmp" );
		//loadImages( imagesColor, "../taxi/", "taxi", 1, 15, 1, "bmp" );

    // Display the images
    displayImageSequence( imagesColor, 200, true );
	
    // Transform the color images to black & white intensity images
    int imageN = imagesColor.size;
    cout << imageN << " images were loaded." << endl;
    CImgList<float> images;
    for( int i = 0; i < imageN; i++ )
        images.push_back( (imagesColor[i].get_channels(0,0) + imagesColor[i].get_channels(1,1) + imagesColor[i].get_channels(2,2))/3 );

    if ( imageN == 2)
        displayAndCompare2Images(images[0],images[1]);

	
	// Compute optical flow
	CImgList<float> optFlow;
	computeOpticalFlow( images, optFlow );

	if (imageN == 2)
	{
		CImg<float> imageWarp( warpImage(images[0],optFlow[0]) );;
		displayAndCompare2Images(imageWarp,images[1]);
	}
		
	// Visualize result
	visualizeOpticalFlow( imagesColor, optFlow, 200, true );
	
}


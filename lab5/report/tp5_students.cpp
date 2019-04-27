/* the namespace permits to use directly CImg<float> instead of having to specify

* cimg_library::CImg<float> each time an image is used

*/
#include "CImg.h" //relative path of the CImg file

using namespace cimg_library;


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define _USE_MATH_DEFINES //defines the value for pi

//#define M_PI 3.1415926536

//#include <cmath> //mathematical functions (exponential)
#include "math.h" //mathematical functions (exponential)
#include <cfloat>

#include <iostream> //for input and output on command line

#include <vector>

#include <sstream>



class EcpException:public std::exception
{
public:
	EcpException(const char* error):m_error(error)
	{
	}

	const char *what() const throw()
	{
		return m_error.c_str();
	}

	~EcpException() throw()
	{
	}

private:
	std::string m_error;
};



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

	//	computes the value of sigma? only once
	float sigm2= 2.0*_sigma*_sigma;
	float fCte = 1.0f/(sigm2*M_PI);

	//	initializes the mask
	for(int i=0 ; i<mask.dimx() ; i++)
	{
		//Compute the value of x?, that will remain
		//constant during the entire loop on y
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




void SinusoidalMasks(float freq, float dir, 
										 CImg<float> &cos_mask, CImg<float> &sin_mask)
{
	float u0 = freq*cos(dir);
	float v0 = freq*sin(dir);

	int dx = cos_mask.dimx();
	int dy = cos_mask.dimy();
	for(int x = 0; x < dx; x++)
	{
		for(int y = 0; y < dy; y++)
		{
			cos_mask(x, y) = cos(2.0*M_PI*(u0*(x - dx/2.0) + v0*(dy/2.0 - y)));
			sin_mask(x, y) = sin(2.0*M_PI*(u0*(x - dx/2.0) + v0*(dy/2.0 - y)));
		}
	}
}

void WriteImage(const CImg <float> & img, const char* fn)
{
	
	float mn = img.min();
	float mx = img.max(); 
	
	CImg<unsigned char> to_disk(img.dimx(), img.dimy());
	//Perform value quantization to the interval [0-256) because when
	//the image is written to the disk each pixel must be one byte,
	//i.e. a number in that interval.
	for(int i = 0; i < img.dimx(); i++)
	{
		for(int j = 0; j < img.dimy(); j++)
		{
			to_disk(i,j) = static_cast<unsigned char>(255*(img(i,j) - mn)/(mx - mn));
		}
	}

	//Write the image to the disk
	std::cout << "Writing file: " << fn << std::endl;
	to_disk.save(fn);
}


CImg<float> GaborFilter(float sigma, float freq, float dir, CImg<float> &img)
{
	
	CImg<float> g_mask = GaussianMask(sigma, 5*sigma);
	CImg<float> c_mask(g_mask.dimx(), g_mask.dimy());
	CImg<float> s_mask(g_mask.dimx(), g_mask.dimy());

	SinusoidalMasks(freq, dir, c_mask, s_mask);

	c_mask.mul(g_mask);
	s_mask.mul(g_mask);
	//CImg<float> gf_cos = g_mask.get_mul(c_mask);
	//CImg<float> gf_sin = g_mask.get_mul(s_mask);
	

	CImg<float> out_c = img.get_convolve(c_mask);//gf_cos);
	CImg<float> out_s = img.get_convolve(s_mask);//gf_sin);
	out_c.sqr();
	out_s.sqr();
	CImg<float> out = out_c + out_s;
	out.sqrt();

	return out;
}




void kMeans( const CImgList<float>& points, CImg<int>& pointsAssignment, CImgList<float>& centers, int centerN )
{
	//////////////////////////////////////////
	// Check arguments all have the right size
	//////////////////////////////////////////
	int pointN = points.size;
	if( pointN == 0 )
		throw EcpException(" kMeans: how can I do k-means on an empty list of points?" );
	int dimX = points[0].dimx();
	int dimY = points[0].dimy();
	int dimZ = points[0].dimz();
	int dimV = points[0].dimv();
	for( int p = 0; p < pointN; p++ )
		if( points[p].dimx() != dimX || points[p].dimy() != dimY || points[p].dimz() != dimZ || points[p].dimv() != dimV )
			throw EcpException(" kMeans: points with incompatible sizes" );
	while( centers.size < centerN )
		centers.push_back( CImg<float>( dimX, dimY, dimZ, dimV, 0 ) );
	for( int c = 0; c < centerN; c++ )
		if( centers[c].dimx() != dimX || centers[c].dimy() != dimY || centers[c].dimz() != dimZ || centers[c].dimv() != dimV )
			centers[c].resize( dimX, dimY, dimZ, dimV );

	if( pointsAssignment.dimy() == 0 && pointsAssignment.dimx() == 0 )
		pointsAssignment.assign( 1, pointN );
	if( pointsAssignment.dimy() != pointN && pointsAssignment.dimx() != 1 )
		pointsAssignment.resize( 1, pointN );

	
	////////////////////////////////////////////////
	//TO DO
	///////////////////////////////////////////////
	
	
	
	////////////////////////////////////////////////
	// Initialize points assignment
	// (for example point p assigned to center #p modulo centerN
	////////////////////////////////////////////////

	for( int p = 0; p < pointN; p++ )
	{
		pointsAssignment(p) = p % centerN;
	}
	


	/////////////////////
	// Start k-means loop
	/////////////////////
	bool converged = false;
	int iter = 0;

	while( !converged )
	{
		iter++;
		std::cout << "iteration : " << iter << std::endl;

		/////////////////////////////
		// Recompute centers position
		/////////////////////////////
		
		CImg<int> groupsSize(centerN, 1, 1, 1, 0);
		for( int p = 0; p < pointN; p++ )
		{
			int c = pointsAssignment(p);
			centers[c] *= groupsSize(c); 
			centers[c] += points[p];
			centers[c] /= groupsSize(c) + 1;
			groupsSize(c) += 1;
		}
	


		//////////////////////////////
		// Recompute points assignment
		//////////////////////////////
 
		float minDist;
		float dist;
		float argMin = 0;
		converged = true;
		for( int p = 0; p < pointN; p++ )
		{
			for( int c = 0; c < centerN; c++ )
			{
				dist = (centers[c] - points[p]).norm(2);
				if( c == 0 )
				{
					minDist = dist;
				}
				else if( minDist > dist )
				{
					minDist = dist;
					argMin = c;
				}
			}
			converged = (pointsAssignment(p) == argMin) && converged;
			pointsAssignment(p) = argMin;


		}

	}
	
}



int main(int argc, char** argv)
{
	//Check the number of arguments given to the program.
	if(argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <input image>" << std::endl;
		exit(1);
	}

	//	open the image
	CImg<float> img_raw(argv[1]);



	//enter the parameters of the algorithm


	// K is the number of clusters to be obtained by k-means
	int K;
	std::cout << "Number of clusters: ";
	std::cin >> K;

	// number of directions considered in the Gabor filter bank
	int num_directions;
	std::cout << "Number of directions: ";
	std::cin >> num_directions;

	// get the dimensions of the image
	int dimX = img_raw.dimx();
	int dimY = img_raw.dimy();




	// number of used frequencies
	int num_freqs;
	std::cout << "Number of Frequencies: ";
	std::cin >> num_freqs;

	//	tables with the responces of the Gabor filters
	CImg<float> **filtered = new CImg<float>*[num_freqs];
	if(filtered == 0)
	{
		std::cerr << "Cannot allocate memory. Exiting" << std::endl;
		exit(2);
	}
	for(int i = 0; i < num_freqs; i++)
	{
		filtered[i] = new CImg<float>[num_directions];
		if(filtered[i] == 0)
		{
			std::cerr << "Cannot allocate memory. Exiting" << std::endl;
			exit(2);
		}
	}

	
	


	float f0,sigma;
	std::cout << "Starting spatial bandwidth sigma: ";
	std::cin >> sigma;
	
	// f0 is the initial frequency to be considered
	f0 = 3.0/(10.0*sigma);

	// fc is the current frequency initialized to f0
	float fc = f0;
	float dir;

	for(int i=0; i < num_freqs; i++)
	{

		std::cout<<"Start computation of frequency "<< i+1<<" over "<< num_freqs <<std::endl;
		std::cout<<"current sigma "<<sigma<<std::endl;
		std::cout<<"current frequency "<<fc<<std::endl;
		// Filtering in the directions for the current frequency fc
		dir = 0;
		for(int j = 0; j < num_directions; j++)
		{
			////////////////////////////////////////////////
			//TO DO
			///////////////////////////////////////////////

			std::cout<<"	current direction "<<dir<<std::endl;

			filtered[i][j] = GaborFilter(sigma, fc, dir, img_raw);
			dir += M_PI / num_directions;

		}

		// update the spatial bandwidth sigma and the frequency fc
		sigma *= sqrt(2.0);
		fc = 3.0/(10.0*sigma);

		std::cout<<"End computation of frequency "<< i+1<<" over "<< num_freqs <<std::endl;
	}

	// create the  CImgList<float> features for k-means and store the features from the array filtered
	
	////////////////////////////////////////////////
		//TO DO
	///////////////////////////////////////////////

	int p = 0;
	CImgList<float> features(dimX * dimY, num_freqs, num_directions);
	for( int x = 0; x < dimX; x++ )
	{
		for( int y = 0; y < dimY; y++ )
		{
			for( int i = 0; i < num_freqs; i++ )
			{
				for( int j = 0; j < num_directions; j++ )
				{
					features[p](i,j) = filtered[i][j](x,y);
				}
			}
			p++;
		}
	}


	

	// Perform k-means on the Gabor features
	CImgList<float> centers;
	CImg<int> pointsAssignment(dimX*dimY);
	kMeans( features, pointsAssignment, centers , K );
	

	
	
	
	
	// display utility
	float **colors = new float*[K];
	for(int k=0;k<K;k++)
	{
		colors[k]= new float[3];
		float u = k/float(K);
		float& r = colors[k][0];
		float& g = colors[k][1];
		float& b = colors[k][2];

		if(u<0)
		{
		}

		else if(u < 1/6.f)
		{
			r=255;
			g=static_cast<int>(6*255*u);
			b=0;
		}

		else if(u<2/6.f)
		{
			r=static_cast<int>(255*(2-6*u));
			g=255;
			b=0;
		}

		else if(u < 3/6.f)
		{
			r=0;
			g=255;
			b=static_cast<int>(255*(6*u-2));
		}

		else if(u<4/6.f)
		{
			r=0;
			g=static_cast<int>(255*(4-6*u));
			b=255;
		}

		else if(u<5/6.f)
		{
			r=static_cast<int>(255*(6*u-4));
			g=0;
			b=255;
		}

		else if(u<1.01)
		{
			r=255;
			g=0;
			b=static_cast<int>(255*(6-6*u));
		}

		else
		{
			r=255;
			g=255;
			b=255;
		}
	}

	// assignment is the color segmentation image that is saved to the disk
	CImg<int> assignment(dimX,dimY,1,3);
	
	
	p=0;
	for(int x=0; x<dimX; x++)
	{
		for(int y=0; y<dimY; y++)
		{
			for(int v=0; v<3; v++)
			{
				int k=pointsAssignment(p);
				assignment(x,y,0,v)=colors[k][v];
			}
			p++;
		}
	}
	assignment.save("kmeansGaborSegmentation.png");

	// release allocated memory
	for(int i = 0; i < num_freqs; i++)
	{
		delete[] filtered[i];
	}
	delete[]filtered;

	for(int k=0;k<K;k++)
	{
		delete[] colors[k];
	}
	delete[] colors;
	return 0;
}

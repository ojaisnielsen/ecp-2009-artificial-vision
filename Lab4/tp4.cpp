#include "CImg.h" //relative path of the CImg file

/* the namespace permits to use directly CImg<float> instead of having to specify

* cimg_library::CImg<float> each time an image is used

*/

using namespace cimg_library;

#define _USE_MATH_DEFINES //defines the value for pi

#include "math.h" //mathematical functions (exponential)

#include <iostream> //for input and output on command line

#include <vector>

#include <sstream>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif


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

    //	computes the value of sigma� only once
    float sigm2= 2*_sigma*_sigma;
    float fCte = 1.0f/(sigm2*M_PI);

    //	initializes the mask
    for(int i=0 ; i<mask.dimx() ; i++)
    {
        //Compute the value of x�, that will remain
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


/*!
 * \brief Create
 */

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

void WriteImage(const CImg<float>& img, const char* fn)
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


CImg<float> GaborFilter(float sigma, float freq, float dir, CImg<float> img)
{
    CImg<float> g_mask = GaussianMask(sigma, 5*sigma);
    CImg<float> c_mask(g_mask.dimx(), g_mask.dimy());
    CImg<float> s_mask(g_mask.dimx(), g_mask.dimy());

    SinusoidalMasks(freq, dir, c_mask, s_mask);
       
    CImg<float> gf_cos = g_mask.get_mul(c_mask);
    CImg<float> gf_sin = g_mask.get_mul(s_mask);

    CImg<float> out_c = img.get_convolve(gf_cos);
    CImg<float> out_s = img.get_convolve(gf_sin);
    CImg<float> out = out_c.sqr() + out_s.sqr();

    return out.get_sqrt();
}

/* Bonus */
CImg<int> GaborSegmentation(CImg<float>* scale_filts, int n_dirs)
{

    int dx = scale_filts[0].dimx();
    int dy = scale_filts[0].dimy();

    CImg<int> ret(dx,dy);
    for(int x = 0; x < dx; x++)
    {
        for(int y = 0; y < dy; y++)
        {
            int max = scale_filts[0].at(x,y);
            int argm = 0;
            for(int i = 1; i < n_dirs; i++)
            {
                if(scale_filts[i](x,y) > max)
                {
                    max = scale_filts[i].at(x,y);
                    argm = i;
                }
            }
            ret(x,y) = argm;
        }
    }

    return ret;

}


int main(int argc, char** argv)
{
    //Check the number of arguments given to the program.
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input image>" << std::endl;
        exit(1);
    }

    //	open the raw image
    CImg<float> img_raw(argv[1]);

    //	read parameters
    float sigma(0);
    std::cout << "Initial Sigma: ";
    std::cin >> sigma;
    int num_scales(0);
    std::cout << "Number of scales used: ";
    std::cin >> num_scales;
    int num_directions = 1;
    std::cout << "Number of directions: ";
    std::cin >> num_directions;
    std::cout << std::endl;

    //	tables with the responces of the Gabor filters
    CImg<float> **filtered = new CImg<float>*[num_scales];
    if(filtered == 0)
    {
        std::cerr << "Cannot allocate memory. Exiting" << std::endl;
        exit(2);
    }
    for(int i = 0; i < num_scales; i++)
    {
        filtered[i] = new CImg<float>[num_directions];
        if(filtered[i] == 0)
        {
            std::cerr << "Cannot allocate memory. Exiting" << std::endl;
            exit(2);
        }
    }
    

    double factor = sqrt(2.0);
    float sigma_scale;
    
    sigma_scale = sigma;
    for(int i=0; i < num_scales; i++)
    {
        for(int j = 0; j < num_directions; j++)
        {
            float dir = j*M_PI/num_directions;

            int n_pix = 2*(5*sigma_scale) + 1;
            float freq = 3.0/n_pix;

            filtered[i][j] = GaborFilter(sigma_scale, freq,  dir, img_raw);

            
            //Save the filtered images
            std::stringstream fn;
            int dir_deg = static_cast<int>(dir*180/M_PI);
            fn << "img_" << i << "_" << dir_deg << ".pgm";
            WriteImage(filtered[i][j], fn.str().c_str());
        }

        sigma_scale = sigma_scale*factor;
    }

    int sc = 0;
        
    if(num_scales != 1)
    {
        std::cout << "Which scale should be segmented? (0-" << num_scales - 1 << ") ";
        
        std::cin >> sc;
        std::cout << std::endl;
    }
    CImg<int> segm = GaborSegmentation(filtered[sc], num_directions);
    segm.save("output.pgm");


    return 0;
}

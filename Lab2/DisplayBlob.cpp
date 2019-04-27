#include "DisplayBlob.h"
#include <iostream>	//	for input and output on command line


DisplayBlob::DisplayBlob(const cimg_library::CImg<float> &_img, const std::vector<Blob> &_vBlob)
: cimg_library::CImgDisplay(_img, "Blob")
, m_imgRaw(_img), m_bRawDisp(true)
{
	//	set initial blob image
	m_imgBlob = _img;

	//	compute the image blob for current blob set
	_ComputeImgBlob(_vBlob);
}

/* ------------------------------------------------------ */
DisplayBlob::DisplayBlob(const DisplayBlob &_displ)
: cimg_library::CImgDisplay( (const cimg_library::CImgDisplay &)_displ)
, m_imgRaw(_displ.m_imgRaw)
{
	m_imgBlob = _displ.m_imgBlob;
	m_bRawDisp= _displ.m_bRawDisp;
}

/* ------------------------------------------------------ */
DisplayBlob::~DisplayBlob()
{
}

/* ------------------------------------------------------ */
void DisplayBlob::Launch(const char* const _sPath)
{
	//	manages saving
	if(NULL != _sPath)
		m_imgBlob.save(_sPath);

	//	user interaction
	while(!is_closed)
	{
		wait();
		//	each time someone press a key, display switches
		if(is_key(true))
		{
			if(m_bRawDisp)
				_DisplayBlob();
			else
				_DisplayRaw();
		}
	}
}

/* ------------------------------------------------------ */
void DisplayBlob::SetBlob(const std::vector<Blob> &_vBlob)
{
	_ComputeImgBlob(_vBlob);
}

/* ------------------------------------------------------ */
void DisplayBlob::_ComputeImgBlob(const std::vector<Blob> &_vBlob)
{
	//	check that images sizes match
	if( !m_imgRaw.is_sameXYZ(m_imgBlob) )
	{
		std::cout << "Image sizes do not match" << std::endl;
		return;
	}

	//	reset blob image
	m_imgBlob = m_imgRaw;

	float* color = new float[3];
	color[0] = 100.0f;
	color[1] = 200.0f;
	color[2] = 100.0f;

	//	display each blob as a circle
	//	iterators allows to loop faster on a vector, an other way to do loop on a
	//	variable i, and to use access operator [.]
	for(std::vector<Blob>::const_iterator it=_vBlob.begin() ; it!=_vBlob.end() ; it++)
		m_imgBlob.draw_circle(it->x, it->y, it->t+1, color, 1, 1);

	_DisplayBlob();
}

/* ------------------------------------------------------ */
void DisplayBlob::_DisplayRaw()
{
	*this << m_imgRaw;
	m_bRawDisp = true;
}

/* ------------------------------------------------------ */
void DisplayBlob::_DisplayBlob()
{
	*this << m_imgBlob;
	m_bRawDisp = false;
}
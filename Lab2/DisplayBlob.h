#ifndef DISPLAY_BLOB_H	//	This prevents including the same file twice, which may lead to
#define DISPLAY_BLOB_H	//	some problems

//	relative path of the CImg file
#include "CImg.h"
#include "Blob.h"
#include <vector>

/*!
\class DisplayBlob "DisplayBlob.h"
\brief Define a display window for blobs
*/
class DisplayBlob : public cimg_library::CImgDisplay{
public:

	/*!
	\brief constructor
	\param _img		image to display
	\param _vBlob	vector of blobs
	*/
	DisplayBlob(const cimg_library::CImg<float> &_img, const std::vector<Blob> &_vBlob);

	/* ------------------------------------------------------ */

	/*!
	\brief copy constructor
	*/
	DisplayBlob(const DisplayBlob &_displ);

	/* ------------------------------------------------------ */

	/*!
	\brief destructor
	*/
	~DisplayBlob();

	/* ------------------------------------------------------ */

	/*!
	\brief manages display with user interaction
	\param _sPath save blob image to given path (if not void)
	*/
	void Launch(const char* const _sPath=NULL);

	/* ------------------------------------------------------ */

	/*!
	\brief set new blob values
	\param _vBlob	vector of blobs
	*/
	void SetBlob(const std::vector<Blob> &_vBlob);

	/* ------------------------------------------------------ */
protected:

	//! display raw image
	void _DisplayRaw();

	/* ------------------------------------------------------ */

	//! display blobed image
	void _DisplayBlob();

private:

	//! class members
	const cimg_library::CImg<float> &m_imgRaw;	///< raw image (not owned) (cannot be modified)
	cimg_library::CImg<float> m_imgBlob;		///< image with blobs
	bool m_bRawDisp;							///< whether or not raw image is displayed

	/* ------------------------------------------------------ */

	/*!
	\brief defines the image with the blobs
	\param _vBlob	vector of blobs
	*/
	void _ComputeImgBlob(const std::vector<Blob> &_vBlob);

	/* ------------------------------------------------------ */
};
#endif // DISPLAY_BLOB_H

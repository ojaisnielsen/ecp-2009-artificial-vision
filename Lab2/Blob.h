#ifndef BLOB_H	//	This prevents including the same file twice, which may lead to
#define BLOB_H	//	some problems

/*!
\class Blob "Blob.h"
\brief Define a Blob as a 2D coordinate (x,y) and a scale t
*/
class Blob{
public:
	int x,y;	///< coordinates
	int t;		///< scale

	/* ------------------------------------------------------ */
	//! constructor
	Blob(int _x=0, int _y=0, int _t=0);

	/* ------------------------------------------------------ */
	//! destructor
	~Blob();

	/* ------------------------------------------------------ */
	//! copy constructor
	Blob(const Blob &_blob);

	/* ------------------------------------------------------ */
	//! comparison operator
	bool operator < (const Blob &_blob) const;
};

#endif // BLOB_H

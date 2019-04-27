#include "Blob.h"

/* ------------------------------------------------------ */
Blob::Blob(int _x, int _y, int _t)
{
	x = _x;
	y = _y;
	t = _t;
}

/* ------------------------------------------------------ */
Blob::~Blob()
{
}

/* ------------------------------------------------------ */
Blob::Blob(const Blob &_blob)
{
	x = _blob.x;
	y = _blob.y;
	t = _blob.t;
}

/* ------------------------------------------------------ */
bool Blob::operator < (const Blob &_blob) const
{
	//	order is defined as order for t, then strength, then y, then x
	if(t != _blob.t)
		return (t < _blob.t);

	if(y != _blob.y)
		return (y < _blob.y);
	else
		return (x < _blob.x);
}

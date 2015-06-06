#ifndef __SQ_MATRIX_STRUCT_H
#define __SQ_MATRIX_STRUCT_H

#include "udxinc.h"

struct SqMatrix
{
	SqMatrix() :
		matrix(NULL),
		dimension(0)
	{
	}

	double* matrix;
	int32 dimension;

	inline void setMatrixVal(int32 x, int32 y, double val)
	{
		checkBounds(x, y);
		this->matrix[y * this->dimension + x] = val;
	}

	inline double getMatrixVal(int32 x, int32 y)
	{
		checkBounds(x, y);
		return this->matrix[y * this->dimension + x];
	}

	inline void checkBounds(int32 x, int32 y)
	{
		if (x < 0 || x >= this->dimension)
			throwUdxException("Coordinate X exceeds matrix dimension");

		if (y < 0 || y >= this->dimension)
			throwUdxException("Coordinate Y exceeds matrix dimension");
	}
};

#endif

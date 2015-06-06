#include "udxinc.h"
#include "SqMatrixStruct.h"

using namespace nz::udx_ver2;

// Calculates linear regression B-coefficients vector
// Usage CalcLRBVec(<x1>, <x2>, ..., <xn>, <y>)
class CalcLRBVecUdtf : public Udtf
{
	private:
		SqMatrix* sqMatrix;
		double* outputVector;
		int32 eoiRowNum;

	public:
		CalcLRBVecUdtf(UdxInit *pInit) : 
			Udtf(pInit),
			sqMatrix(NULL),
			outputVector(NULL),
			eoiRowNum(0)
		{
		}

		~CalcLRBVecUdtf()
		{
			if (outputVector != NULL)
				delete[] outputVector;
		}

		inline void init()
		{
			CPad* pad = getPad("SqMatrixPad");
			sqMatrix = (SqMatrix*) pad->getRootObject(sizeof(SqMatrix));

			if (sqMatrix == NULL)
				throwUdxException("Pad does not exist.");

			if (sqMatrix->dimension + 1 != numArgs())
				throwUdxException("Incorrect number of arguments.");

			outputVector = new double[sqMatrix->dimension];

			for (int32 i = 0; i < sqMatrix->dimension; ++i)
				outputVector[i] = 0.0;
		}

		static Udtf* instantiate(UdxInit*);

		virtual void newInputRow()
		{
			if (sqMatrix == NULL)
				init();

			double y = doubleArg(sqMatrix->dimension);

			for (int32 ai = 0; ai < sqMatrix->dimension; ++ai)
			{
				double x = doubleArg(ai);

				for (int32 si = 0; si < sqMatrix->dimension; ++si)
				{
					double val = sqMatrix->getMatrixVal(ai, si);
					outputVector[si] += x * val * y;
				}
			}
		}

		virtual DataAvailable nextOutputRow()
		{
			return Done;
		}

		virtual DataAvailable nextEoiOutputRow()
		{
			if (sqMatrix == NULL || eoiRowNum >= sqMatrix->dimension)
				return Done;

			int32* bCol = int32ReturnColumn(0);
			*bCol = eoiRowNum;

			double* valCol = doubleReturnColumn(1);
			*valCol = outputVector[eoiRowNum];

			eoiRowNum++;
			return MoreData;
		}
};

Udtf* CalcLRBVecUdtf::instantiate (UdxInit* pInit) 
{
	return new CalcLRBVecUdtf(pInit);
}

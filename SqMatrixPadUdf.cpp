#include "udxinc.h"
#include "SqMatrixStruct.h"

using namespace nz::udx_ver2;

// Creates SpuPad 'SqMatrixPad' and stores square matrix.
// Usage: SqMatrixPad(<matrix dimension>, <col>, <row>, <value>)
class SqMatrixPadUdf : public Udf
{
	private:

	public:
		SqMatrixPadUdf(UdxInit *pInit) : 
			Udf(pInit)
		{
		}

		static Udf* instantiate(UdxInit*);

		virtual ReturnValue evaluate()
		{
			bool padCreated = false;
			int32 dimension = int32Arg(0);
			CPad* pad = getPad("SqMatrixPad");
			SqMatrix* sqMatrix = (SqMatrix*) pad->getRootObject(sizeof(SqMatrix));

			if (sqMatrix == NULL)
			{
				if (dimension <= 0)
					throwUdxException("Expected positive matrix dimension.");

				sqMatrix = PAD_NEW(pad,SqMatrix);
				sqMatrix->dimension = dimension;
				sqMatrix->matrix = PAD_NEW(pad,double)[dimension * dimension];
				pad->setRootObject(sqMatrix, sizeof(SqMatrix));
				padCreated = true;
			}

			int32 x = int32Arg(1);
			int32 y = int32Arg(2);
			double val = doubleArg(3);

			if (isUserQuery())
				sqMatrix->setMatrixVal(x, y, val);

			NZ_UDX_RETURN_BOOL(padCreated);
		}
};

Udf* SqMatrixPadUdf::instantiate (UdxInit* pInit) 
{
	return new SqMatrixPadUdf(pInit);
}

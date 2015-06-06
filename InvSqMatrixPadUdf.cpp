#include "udxinc.h"
//#include "mkl.h"
#include <Eigen/LU>
#include "SqMatrixStruct.h"

using namespace nz::udx_ver2;

// Inverts square matrix stored in SpuPad 'SqMatrixPad'.
// Usage: InvSqMatrixPad()
class InvSqMatrixPadUdf : public Udf
{
	private:
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RMatrixXd;

	public:
		InvSqMatrixPadUdf(UdxInit *pInit) : 
			Udf(pInit)
		{
		}

		static Udf* instantiate(UdxInit*);

		// Eigen version
		virtual ReturnValue evaluate()
		{
			CPad* pad = getPad("SqMatrixPad");
			SqMatrix* sqMatrix = (SqMatrix*) pad->getRootObject(sizeof(SqMatrix));

			if (sqMatrix == NULL)
				throwUdxException("Pad does not exist.");

			Eigen::MatrixXd m = Eigen::Map<RMatrixXd>(
				sqMatrix->matrix,
				sqMatrix->dimension,
				sqMatrix->dimension);

			RMatrixXd i = m.inverse();

			Eigen::Map<RMatrixXd>(
				sqMatrix->matrix,
				sqMatrix->dimension,
				sqMatrix->dimension) = i;

			NZ_UDX_RETURN_BOOL(true);
		}

		// MKL version
		/*virtual ReturnValue evaluate()
		{
			CPad* pad = getPad("SqMatrixPad");
			SqMatrix* sqMatrix = (SqMatrix*) pad->getRootObject(sizeof(SqMatrix));

			if (sqMatrix == NULL)
				throwUdxException("Pad does not exist.");

			MKL_INT n = sqMatrix->dimension;
			MKL_INT lda = sqMatrix->dimension;
			MKL_INT ipiv;
			MKL_INT info;
			MKL_INT workSize = n*n;
			double* workArray = new double[workSize];

			dgetrf(&n, &n, sqMatrix->matrix, &lda, &ipiv, &info);

			if (info != 0)
				throwUdxException("Matrix LU factorization failed.");

			dgetri(&n, sqMatrix->matrix, &lda, &ipiv, workArray, &workSize, &info);

			if (info != 0)
				throwUdxException("Matrix inversion failed.");

			NZ_UDX_RETURN_BOOL(true);
		}*/
};

Udf* InvSqMatrixPadUdf::instantiate (UdxInit* pInit) 
{
	return new InvSqMatrixPadUdf(pInit);
}

#include "udxinc.h"

using namespace nz::udx_ver2;

class XTXMatrixUdtf : public Udtf
{
	private:
		int32 args;
		int32 outputMatrixSize;
		double* outputMatrix;
		double* inputArguments;
		int32 eoiRowNum;

	public:
		XTXMatrixUdtf(UdxInit *pInit) : 
			Udtf(pInit),
			args(0),
			outputMatrixSize(0),
			outputMatrix(NULL),
			inputArguments(NULL),
			eoiRowNum(0)
		{
		}

		~XTXMatrixUdtf()
		{
			if (outputMatrix != NULL)
			{
				delete[] outputMatrix;
				outputMatrix = NULL;
			}

			if (inputArguments != NULL)
			{
				delete[] inputArguments;
				inputArguments = NULL;
			}
		}

		static Udtf* instantiate(UdxInit*);

		inline void init()
		{
			args = numArgs();

			if (args == 0)
				throwUdxException("Expected one or more arguments");

			outputMatrixSize = args * args;
			outputMatrix = new double[outputMatrixSize];
			inputArguments = new double[args];

			for (int32 i = 0; i < outputMatrixSize; ++i)
				outputMatrix[i] = 0.0;
		}

		inline double getOutputMatrixVal(int32 x, int32 y)
		{
			return outputMatrix[y * args + x];
		}

		inline void setOutputMatrixVal(int32 x, int32 y, double val)
		{
			outputMatrix[y * args + x] = val;
		}

		virtual void newInputRow()
		{
			if (outputMatrix == NULL)
				init();

			for (int32 x = 0; x < args; ++x)
			{
				inputArguments[x] = doubleArg(x);

				for (int32 y = 0; y <= x; ++y)
				{
					double val = getOutputMatrixVal(x, y);
					val += inputArguments[x] * inputArguments[y];
					setOutputMatrixVal(x, y, val);
					setOutputMatrixVal(y, x, val);
				}
			}
		}

		virtual DataAvailable nextOutputRow()
		{
			return Done;
		}

		virtual DataAvailable nextEoiOutputRow()
		{
			if (eoiRowNum >= outputMatrixSize)
				return Done;

			int32 x = eoiRowNum % args;
			int32 y = eoiRowNum / args;
			double val = getOutputMatrixVal(x, y);

			int32* xCol = int32ReturnColumn(0);
			*xCol = x;

			int32* yCol = int32ReturnColumn(1);
			*yCol = y;

			double* valCol = doubleReturnColumn(2);
			*valCol = val;

			eoiRowNum++;
			return MoreData;
		}
};

Udtf* XTXMatrixUdtf::instantiate (UdxInit* pInit) 
{
	return new XTXMatrixUdtf(pInit);
}

#include "udxinc.h"

using namespace nz::udx_ver2;

class TestUdtf : public Udtf
{
	private:
		char value[1000];
		int valuelen;
		int i;

	public:
		TestUdtf(UdxInit *pInit) : Udtf(pInit) {}
		static Udtf* instantiate(UdxInit*);

		virtual void newInputRow()
		{
			StringArg *valuesa = stringArg(0);
			bool valuesaNull = isArgNull(0);

			if (valuesaNull)
				valuelen = 0;
			else 
			{
				if (valuesa->length >= 1000)
				throwUdxException("Input value must be less than 1000 characters.");
				memcpy(value, valuesa->data, valuesa->length);
				value[valuesa->length] = 0;
				valuelen = valuesa->length;
			}

			i = 0;
		}

		virtual DataAvailable nextOutputRow()
		{
			if (i >= valuelen)
				return Done;

			// save starting position of name
			int start = i;

			// scan string for next comma
			while ((i < valuelen) && value[i] != ',')
				i++;

			// return word
			StringReturn *rk = stringReturnColumn(0);
			if (rk->size < i-start)
				throwUdxException("Value exceeds return size");

			memcpy(rk->data, value+start, i-start);
			rk->size = i-start;
			i++;
			return MoreData;
		}
};

Udtf* TestUdtf::instantiate (UdxInit* pInit) 
{
	return new TestUdtf(pInit);
}

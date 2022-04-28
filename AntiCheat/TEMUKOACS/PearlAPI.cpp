#include "PearlAPI.h"

FUNC_BYTES PearlAPI::GetFunctionBytes(PBYTE func) {
	FUNC_BYTES tmp;
	for (uint32 i = 0; i < sizeof(func) / sizeof(func[0]); i++)
		tmp.push_back(func[i]);
	return tmp;
}

bool PearlAPI::ValidateFunction(PBYTE func, FUNC_BYTES bytes)
{
	FUNC_BYTES tmp;
	for (uint32 i = 0; i < sizeof(func) / sizeof(func[0]); i++)
		tmp.push_back(func[i]);
	return tmp == bytes;
}
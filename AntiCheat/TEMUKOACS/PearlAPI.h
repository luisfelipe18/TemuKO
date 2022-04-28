#pragma once
#include "stdafx.h"

typedef std::vector<uint8>(FUNC_BYTES);

class PearlAPI
{
public:
	static FUNC_BYTES GetFunctionBytes(PBYTE func);
	static bool ValidateFunction(PBYTE func, FUNC_BYTES bytes);
};
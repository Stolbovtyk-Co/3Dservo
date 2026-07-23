#pragma once
#include <Windows.h>

static class HRErrorHandler
{
public:
	inline static void Throw(HRESULT hr);
};


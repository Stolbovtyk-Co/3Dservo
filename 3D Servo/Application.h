#pragma once
#include <Windows.h>

class Application
{
private:
	bool m_isRunning;
	HWND hWnd;
public:
	Application();

	void Run();

	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
private:
	void Initialize();

	void Process();

	void Risovat();
};


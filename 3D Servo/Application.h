#pragma once
#include <Windows.h>
#include "RenderEngine.h"
#include "Logger.h"
#include <string>

class Application
{
private:
	const int WIN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	const int WIN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	bool m_isRunning;
	static std::wstring m_windowClassName;

	HWND m_hWnd = nullptr;
	std::shared_ptr<Logger> m_logger = std::make_shared<Logger>();
	std::unique_ptr<RenderEngine> m_renderEngine = nullptr;
public:
	Application();

	void Run();

	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
private:
	void Initialize();

	void Process();

	void Risovat();
};


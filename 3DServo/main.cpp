#include "Engine/Core/Application.h"
#include <Windows.h>

int WINAPI main(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    Application app;
    app.Run();
    return 0;
}
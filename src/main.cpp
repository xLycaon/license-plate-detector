#include "Interface.hpp"
#include <iostream>
#include <Windows.h>

int main() {

    // Detach from console window
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);

    Interface app;
    app.run();

    return 0;
}

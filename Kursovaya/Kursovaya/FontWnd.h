#pragma once
#include <windows.h>
HWND CreateFontWindow(HINSTANCE hInstance, HWND hwndParent);
bool CreateFontWndClass(HINSTANCE hInstance);
LRESULT CALLBACK FontWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
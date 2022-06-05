#pragma once

#include <windows.h>
void SetDefFont(HWND hwndParent, const wchar_t* fontName, int fontSize);
bool CALLBACK SetFont(HWND child, LPARAM font);
void PrintToFile(wchar_t* filePath, wchar_t* buff);
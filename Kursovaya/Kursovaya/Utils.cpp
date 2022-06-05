#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>

bool CALLBACK SetFont(HWND child, LPARAM font) {
    SendMessage(child, WM_SETFONT, font, true);
    return true;
}

void SetDefFont(HWND hwndParent, const wchar_t* fontName, int fontSize) {
    HFONT hfont = CreateFont(fontSize, NULL, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, MAKELPARAM(VARIABLE_PITCH, FF_MODERN), fontName);
    EnumChildWindows(hwndParent, (WNDENUMPROC)SetFont, (LPARAM)MAKELPARAM(hfont, 0));
}

void PrintToFile(wchar_t* filePath, wchar_t* buff) {
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());

    std::wofstream wofs(filePath, std::ios_base::out);
    if (wofs.is_open()) {
        wofs.imbue(utf8_locale);
        wofs << buff;
    }
    wofs.close();
}
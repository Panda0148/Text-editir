#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "resource.h"
#include "CommCtrl.h"
#include "Utils.h"
#include <vector>
#include <algorithm>
#include <string>

LRESULT CALLBACK FontWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK EnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType, LPARAM lParam);
bool CALLBACK SetFont(HWND child, LPARAM font);

LOGFONTW logfont = { 0 };
std::wstring selFont;
LONG selHeight = 0;

std::vector<std::wstring> FontHeightVec = {
                L"11",
                L"12",
                L"14",
                L"16",
                L"18",
                L"20",
                L"22",
                L"24",
                L"26",
                L"28",
                L"36",
                L"48",
                L"72"
};

 bool CreateFontWndClass(HINSTANCE hInstance) {
    WNDCLASS wc = { 0 };
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpfnWndProc = FontWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"FontWndClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
        return false;

    else
        return true;
}

HWND CreateFontWindow(HINSTANCE hInstance, HWND hwndParent) {

	HWND hwndFont = CreateWindow(
        L"FontWndClass",                     // Класс окно
        L"Шрифт",    // Название окна
        WS_OVERLAPPEDWINDOW,            // Стиль окна

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,

        hwndParent,       // Родительское окно    
        NULL,       // Меню
        hInstance,  // Instance экземпляра
        NULL        // доп данные
    );

    if (hwndFont == 0) {
        return 0;
    }
    else {
        return hwndFont;
    }
}

LRESULT CALLBACK FontWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    std::vector<std::wstring> fnvec;
    logfont.lfHeight = 16;
    logfont.lfWeight = FW_DONTCARE;
    logfont.lfCharSet = DEFAULT_CHARSET;
    logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logfont.lfQuality = ANTIALIASED_QUALITY;
    logfont.lfPitchAndFamily = MAKELPARAM(VARIABLE_PITCH, FF_MODERN);
    wcscpy_s(logfont.lfFaceName, L"");
    HWND hwndChildEdit = FindWindowEx(FindWindowEx(NULL, NULL, L"MainWindowClass", NULL), NULL, L"EDIT", NULL);
    


    switch (uMsg) {
    case WM_CREATE:
    {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        const COLORREF clr = RGB(255, 255, 255);

        HBRUSH brush = CreateSolidBrush(clr);
        FillRect(hdc, &ps.rcPaint, brush);
        EndPaint(hwnd, &ps);

        HWND hwndFontComboBoxLabel = CreateWindow(L"STATIC", L"Шрифт:", WS_VISIBLE | WS_CHILD | SS_LEFT, 70, 25, 70, 25, hwnd, NULL, hInstance, NULL);
        ShowWindow(hwndFontComboBoxLabel, SW_NORMAL);

        HWND hwndSizeComboBoxLabel = CreateWindow(L"STATIC", L"Размер:", WS_VISIBLE | WS_CHILD | SS_LEFT, 330, 25, 70, 25, hwnd, NULL, hInstance, NULL);
        ShowWindow(hwndSizeComboBoxLabel, SW_NORMAL);

        HWND hwndFontComboBox = CreateWindow(WC_COMBOBOX, TEXT("Шрифт"),
            CBS_SIMPLE | CBS_HASSTRINGS | CBS_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
            70, 50, 200, 250, hwnd, (HMENU)ID_FONTCOMBOBOX, hInstance,
            NULL);

        HWND hwndSizeComboBox = CreateWindow(WC_COMBOBOX, TEXT("Размер"),
            CBS_SIMPLE | CBS_HASSTRINGS | CBS_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
            330, 50, 100, 250, hwnd, (HMENU)ID_SIZECOMBOBOX, hInstance,
            NULL);

        HWND hwndExText = CreateWindow(L"STATIC", L"АаБбВвГгДдЕе", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 70, 400, 200, 25, hwnd, (HMENU)ID_EXTEXT, hInstance, NULL);
        ShowWindow(hwndExText, SW_NORMAL);

        HWND hwndExTextLabel = CreateWindow(L"STATIC", L"Пример:", WS_VISIBLE | WS_CHILD | SS_LEFT, 70, 375, 70, 25, hwnd, NULL, hInstance, NULL);
        ShowWindow(hwndSizeComboBoxLabel, SW_NORMAL);

        HWND hwndBtnOK = CreateWindow(WC_BUTTON, TEXT("Принять"), WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 70, 500, 100, 25, hwnd, (HMENU)ID_BTN_OK, hInstance, NULL);
        ShowWindow(hwndBtnOK, SW_NORMAL);

        HWND hwndBtnCancel = CreateWindow(WC_BUTTON, TEXT("Отмена"), WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 450, 500, 100, 25, hwnd, (HMENU)ID_BTN_CANCEL, hInstance, NULL);
        ShowWindow(hwndBtnCancel, SW_NORMAL);


        EnumFontFamiliesEx(GetDC(NULL), &logfont, EnumFontFamExProc, (LPARAM) &fnvec, NULL);

        for (size_t i = 0; i < fnvec.size(); i++)
        {
            SendMessage(hwndFontComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)fnvec[i].c_str());
        }
        SendMessage(hwndFontComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

        SetDefFont(hwnd, L"Helvetica", 16);

        return 0;


    }

    case WM_COMMAND: {

        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_FONTCOMBOBOX)
        {
            wchar_t  ListItem[256];
            int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                (WPARAM)0, (LPARAM)0);
            (TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
                (WPARAM)ItemIndex, (LPARAM)ListItem);
            selFont = ListItem;

            

            HWND hwndSizeComboBox = FindWindowEx(hwnd, (HWND)lParam, WC_COMBOBOX, NULL);
            SendMessage(hwndSizeComboBox, CB_RESETCONTENT, NULL, NULL);
            for (size_t i = 0; i < FontHeightVec.size(); i++)
            {
                SendMessage(hwndSizeComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)FontHeightVec[i].c_str());
            }
            SendMessage(hwndSizeComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
            HWND hwndExText = FindWindowEx(hwnd, NULL, L"STATIC", NULL);
            hwndExText = FindWindowEx(hwnd, FindWindowEx(hwnd, hwndExText, L"STATIC", NULL), L"STATIC", NULL);

            wcscpy_s(logfont.lfFaceName, selFont.c_str());
            logfont.lfHeight = selHeight;
            HFONT font = CreateFontIndirect(&logfont);
            SendMessage(hwndExText, WM_SETFONT, MAKEWPARAM(font, 0), MAKELPARAM(TRUE, 0));
            
        }


        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_SIZECOMBOBOX)
        {
            wchar_t  ListItem[256];
            int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                (WPARAM)0, (LPARAM)0);
            (TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
                (WPARAM)ItemIndex, (LPARAM)ListItem);
            selHeight = _wtol(ListItem);

            wcscpy_s(logfont.lfFaceName, selFont.c_str());
            logfont.lfHeight = selHeight;
            HWND hwndExText = FindWindowEx(hwnd, NULL, L"STATIC", NULL);
            hwndExText = FindWindowEx(hwnd, FindWindowEx(hwnd, hwndExText, L"STATIC", NULL), L"STATIC", NULL);
            HFONT font = CreateFontIndirect(&logfont);
            SendMessage(hwndExText, WM_SETFONT, MAKEWPARAM(font, 0), MAKELPARAM(TRUE, 0));

        }
         
        if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == ID_BTN_OK) {
            wcscpy_s(logfont.lfFaceName, selFont.c_str());
            logfont.lfHeight = selHeight;
            HFONT font = CreateFontIndirect(&logfont);
            SendMessage(hwndChildEdit, WM_SETFONT, MAKEWPARAM(font, 0), MAKELPARAM(TRUE, 0));
            SendMessage(hwnd, WM_DESTROY, NULL, NULL);

        }

        if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == ID_BTN_CANCEL) {
            SendMessage(hwnd, WM_DESTROY, NULL, NULL);

        }

        return 0;
    }

    case WM_DESTROY:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

int CALLBACK EnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType, LPARAM fvec) {
    std::vector<std::wstring>* fnvec = (std::vector<std::wstring>*)fvec;
    if (!lpelfe)
        return 0;
    else {
        if (std::find(fnvec->begin(), fnvec->end(), std::wstring(lpelfe->lfFaceName)) != fnvec->end()) {
            return 1;
        }
        else {
            fnvec->push_back(std::wstring(lpelfe->lfFaceName));
            return 1;
        }

    }
}

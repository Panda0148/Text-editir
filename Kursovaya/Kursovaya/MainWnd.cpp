#ifndef UNICODE
#define UNICODE
#endif

//Modern window looks
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


#include <windows.h>
#include "resource.h"
#include "FontWnd.h"
#include "Utils.h"
#include "commctrl.h"




#include <thread>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void PrintToFile(wchar_t* filePath, wchar_t* buff);



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, INT nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"MainWindowClass";

    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    if (!RegisterClass(&wc)) 
        return -1;

    if (!CreateFontWndClass(hInstance))
        return -1;

    HMENU hMainMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1));


    HWND hwndMain = CreateWindow(
        CLASS_NAME,                     // Класс окна
        L"Текстовый редактор",    // Название окна
        WS_OVERLAPPEDWINDOW,            // Стиль окна

        // размер и позиция
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 600,

        NULL,       // родительское окно   
        hMainMenu,       // меню
        hInstance,  // Instance экземпляра
        NULL        // дополнительные данные приложения
    );

    if (hwndMain == 0) {
        return 0;
    }

   

    RECT MainRect = {};
    GetWindowRect(hwndMain, &MainRect);

    int MainWitdh = MainRect.right - MainRect.left;
    int MainHeight = MainRect.bottom - MainRect.top;

    HWND hwndEdit = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_LEFT | WS_HSCROLL | WS_VSCROLL | ES_AUTOVSCROLL, 0, 0, MainWitdh - 20, MainHeight - 60, hwndMain, (HMENU)ID_EDIT, hInstance, NULL);
    
    RegisterHotKey(hwndMain, ID_CTRLS, MOD_CONTROL, 0x53);
    RegisterHotKey(hwndMain, ID_CTRLO, MOD_CONTROL, 0x4F);


    ShowWindow(hwndMain, nCmdShow);
    ShowWindow(hwndEdit, nCmdShow);

    TCITEM tci = {};
    wchar_t tab[] = L"Tab 1";
    tci.pszText = tab;


    //цикл сообщений
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }




    return 0;

}

//hwnd - окно экземпляра; uMsg - код сообщения; wParam & lParam - доп данные
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
    HWND hwndChildEdit = FindWindowEx(hwnd, NULL, L"EDIT", NULL);
    HWND hwndFontWnd = FindWindowEx(hwnd, NULL, L"FontWndClass", NULL);
    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    
    
    switch (uMsg) {

    case WM_PAINT:
    {
        HFONT hfont = CreateFont(18, NULL, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, MAKELPARAM(VARIABLE_PITCH, FF_MODERN), TEXT("Helvetica"));
        SendMessage(hwndChildEdit, WM_SETFONT, MAKEWPARAM(hfont, 0), MAKELPARAM(TRUE, 0));
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        const COLORREF clr = RGB(255, 255, 255);

        HBRUSH brush = CreateSolidBrush(clr);
        FillRect(hdc, &ps.rcPaint, brush);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
    {
       
        int nwidth = LOWORD(lParam);
        int nheight = HIWORD(lParam);
        SetWindowPos(hwndChildEdit, NULL, 0, 0, nwidth, nheight, NULL);


    }

    case WM_COMMAND:
    {
        if (HIWORD(wParam) == EN_CHANGE) {
            wchar_t buff[128];
            GetWindowText(hwnd, buff, 128);
            if (wcschr(buff, L'*') == NULL) {
                int len = wcslen(buff);
                buff[len] = L'*';
                buff[len + 1] = L'\0';
                SetWindowText(hwnd, buff);
            }
        }
        if (LOWORD(wParam) == ID_FILE_OPEN) {
            wchar_t filePath[128];
            filePath[0] = NULL;
            OPENFILENAME FileStruct = {NULL};
            FileStruct.lStructSize = sizeof(OPENFILENAME);
            FileStruct.lpstrFile = filePath;
            FileStruct.nMaxFile = 128;
            FileStruct.lpstrFilter = L"Text Files\0*.txt\0\0";
            GetOpenFileName(&FileStruct);

            std::wifstream wifs(filePath);
            if (wifs.is_open()) {
                SetWindowText(hwnd, filePath);
                wifs.imbue(utf8_locale);
                std::wstringstream buff;
                buff << wifs.rdbuf();
                std::wstring ws = buff.str();
                SetWindowText(hwndChildEdit, ws.c_str());
            }
            wifs.close();
            

        }

        if (LOWORD(wParam) == ID_FILE_SAVE) {
            wchar_t filePath[128];
            wchar_t curFile[128];
            GetWindowText(hwnd, curFile, 128);

            if ((wcscmp(curFile, L"Текстовый редактор") == 0) || (wcscmp(curFile, L"Текстовый редактор*") == 0)) {

                filePath[0] = NULL;
                OPENFILENAME FileStruct = { NULL };
                FileStruct.lStructSize = sizeof(OPENFILENAME);
                FileStruct.lpstrFile = filePath;
                FileStruct.nMaxFile = 128;
                GetSaveFileName(&FileStruct);
            }
            else {
                GetWindowText(hwnd, filePath, 128);
            }
            wchar_t buff[1024];
            GetWindowText(hwndChildEdit, buff, 1024);

            if (wcscmp(filePath, L"") != 0)
                if (wcschr(filePath, L'*') != NULL) {
                    filePath[wcslen(filePath) - 1] = L'\0';
                    SetWindowText(hwnd, filePath);
                }

            std::thread to(PrintToFile, filePath, buff);
            to.join();


        }

        if (LOWORD(wParam) == ID_FILE_SAVEAS) {
            wchar_t filePath[128];
            filePath[0] = NULL;
            OPENFILENAME FileStruct = { NULL };
            FileStruct.lStructSize = sizeof(OPENFILENAME);
            FileStruct.lpstrFile = filePath;
            FileStruct.nMaxFile = 128;
            GetSaveFileName(&FileStruct);
            SetWindowText(hwnd, filePath);
            wchar_t buff[1024];
            GetWindowText(hwndChildEdit, buff, 1024);

            std::thread to(PrintToFile, filePath, buff);
            to.join();
        }

        if (LOWORD(wParam) == ID_FORMAT_CHANGEFONT) {
            HINSTANCE hInstance = GetModuleHandle(NULL);
            HWND hwndFont = CreateFontWindow(hInstance, NULL);
            if (hwndFont == 0) {
                return 0;
            }
            ShowWindow(hwndFont, SW_NORMAL);
        }

        if (LOWORD(wParam) == ID_FILE_EXIT)
            PostQuitMessage(0);

        return 0;
    }

    case WM_HOTKEY:
    {
        if (wParam == ID_CTRLS) {
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_FILE_SAVE, NULL), NULL);
        }

        if (wParam == ID_CTRLO) {
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, NULL), NULL);
        }
    }
    return 0;


    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

}









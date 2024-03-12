#include <windows.h>
#include <shellapi.h>
#include <string>
#include <iostream>
#include "framework.h"
#include "DiskOInferno.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
// Function to format bytes into KB, MB, GB, etc.
std::string FormatBytes(ULARGE_INTEGER bytes) {
    // ... implementation here ... 
        const double GB_DIVISOR = 1024.0 * 1024.0 * 1024.0;  // Bytes in 1 GB

    double sizeInGB = bytes.QuadPart / GB_DIVISOR;

    // For more precise display:
    return std::to_string(sizeInGB).substr(0, 5) + " GB";  // Show up to 2 decimals

    // If you prefer integer display:
    // return std::to_string((int)sizeInGB) + " GB"; 
}

// Callback for taskbar icon messages
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        // Handle right-click on the icon for actions like Exit
    case WM_RBUTTONUP:
        // ... 
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // 1. Create a hidden window for icon messages
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_OWNDC;  // Add this line
    wcex.lpfnWndProc = WindowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DISKOINFERNO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DISKOINFERNO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    auto status = RegisterClassExW(&wcex);

    if (!status) {
        MessageBox(nullptr, L"Failed to register window class!", L"Error", MB_OK);
        return 1;
    }
    HWND hwnd = CreateWindowEx(0, /* Default Extended Styles */
        wcex.lpszClassName, /* Class Name */
        L"My Hidden Taskbar App", /* Window Title (Irrelevant here) */
        0,   /* No WS_VISIBLE style */
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,  /* Position, Size */
        nullptr, nullptr, hInstance, nullptr);

    if (hwnd == nullptr) {
        MessageBox(nullptr, L"Failed to create handle!", L"Error", MB_OK);
        return 1;
    }


    // 2. Create taskbar icon
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DISKOINFERNO));
    nid.uCallbackMessage = WM_USER + 100; // Custom message for icon updates

    Shell_NotifyIcon(NIM_ADD, &nid);

    // 3. Get initial free space
    ULARGE_INTEGER freeBytes, totalBytes;
    GetDiskFreeSpaceEx(L"C:\\", &freeBytes, &totalBytes, nullptr);
    std::string freeSpaceStr = "Free Space: " + FormatBytes(freeBytes);
    // Convert std::string to std::wstring
    std::wstring wideFreeSpaceStr(freeSpaceStr.begin(), freeSpaceStr.end());

    wcscpy_s(nid.szTip, wideFreeSpaceStr.length() + 1, wideFreeSpaceStr.c_str());
    Shell_NotifyIcon(NIM_MODIFY, &nid);

    // 4. Update loop (with a timer or background thread)
    while (true) {
        // ... Get Free Space Again ...
        // ... Update nid.szTip ...
        Shell_NotifyIcon(NIM_MODIFY, &nid);
        Sleep(5000); // Update every 5 seconds
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);
    return 0;
}

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <iostream>
#include <Commctrl.h>  // For progress bar
#include "framework.h"
#include "DiskOInferno.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWnd;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
NOTIFYICONDATA nid = {};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

const UINT_PTR TIMER_ID = 1;
const UINT_PTR INITIAL_TIMER_ID = 2;
const UINT UPDATE_INTERVAL = 1000 * 60 * 60;  // 1 hour
const UINT INITIAL_UPDATE_INTERVAL = 5000;  // 5 seconds

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DISKOINFERNO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DISKOINFERNO));

	MSG msg;

	// TODO: Place code here.
	// 2. Create taskbar icon

	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	nid.uCallbackMessage = WM_USER + 100; // Custom message for icon updates

	Shell_NotifyIcon(NIM_ADD, &nid);

	// 3. Get initial free space
	ULARGE_INTEGER freeBytes, totalBytes;
	GetDiskFreeSpaceEx(L"C:\\", &freeBytes, &totalBytes, nullptr);
	std::string freeSpaceStr = "Free Space: " + FormatBytes(freeBytes) + " of " + FormatBytes(totalBytes);
	// Convert std::string to std::wstring
	std::wstring wideFreeSpaceStr(freeSpaceStr.begin(), freeSpaceStr.end());

	wcscpy_s(nid.szTip, wideFreeSpaceStr.length() + 1, wideFreeSpaceStr.c_str());
	Shell_NotifyIcon(NIM_ADD, &nid);

	// Start the update timer
	SetTimer(hWnd, TIMER_ID, UPDATE_INTERVAL, nullptr);
	SetTimer(hWnd, INITIAL_TIMER_ID, INITIAL_UPDATE_INTERVAL, nullptr);

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_OWNDC;  // Add this line
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DISKOINFERNO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowW(szWindowClass, szTitle, 0,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_TIMER:
		if (wParam == TIMER_ID || wParam == INITIAL_TIMER_ID) {

			ULARGE_INTEGER freeBytes, totalBytes;
			GetDiskFreeSpaceEx(L"C:\\", &freeBytes, &totalBytes, nullptr);
			std::string freeSpaceStr = "Free Space: C:\\" + FormatBytes(freeBytes) + " of " + FormatBytes(totalBytes);
			// Convert std::string to std::wstring
			std::wstring wideFreeSpaceStr(freeSpaceStr.begin(), freeSpaceStr.end());

			wcscpy_s(nid.szTip, wideFreeSpaceStr.length() + 1, wideFreeSpaceStr.c_str());
			Shell_NotifyIcon(NIM_MODIFY, &nid);

			// kill timer if wParam == INITIAL_TIMER_ID
			if (wParam == INITIAL_TIMER_ID) {
				KillTimer(hWnd, INITIAL_TIMER_ID);
			}
		}
		break;
	case WM_USER + 100: // Handle taskbar icon messages
		switch (lParam) {
		case WM_RBUTTONUP:
			// Handle right-click on the icon (show menu, etc.)
								// Show your About dialog
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		// Get the progress bar handle
		HWND hProgress = GetDlgItem(hDlg, IDC_PROGRESS1);

		// Get disk space information
		ULARGE_INTEGER freeBytes, totalBytes;
		GetDiskFreeSpaceEx(L"C:\\", &freeBytes, &totalBytes, nullptr);

		// Calculate and set the progress
		double percentFree = (double)freeBytes.QuadPart / totalBytes.QuadPart * 100.0;

		SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		SendMessage(hProgress, PBM_SETPOS, (int)percentFree, 0);

		// Get handle to the static control
		HWND hStaticText = GetDlgItem(hDlg, IDC_FREESPACE); // Replace with your ID

		// Format the free space string 
		std::string freeSpaceStr = "Free Space: C:\\" + FormatBytes(freeBytes) + " of " + FormatBytes(totalBytes);
		std::wstring wideFreeSpaceStr(freeSpaceStr.begin(), freeSpaceStr.end());

		// Set the text of the static control
		SetWindowText(hStaticText, wideFreeSpaceStr.c_str());

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


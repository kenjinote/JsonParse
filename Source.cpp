#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <iostream>
#include "json11.hpp"

TCHAR szClassName[] = TEXT("Window");

LPWSTR a2w(LPCSTR lpszText)
{
	const DWORD dwSize = MultiByteToWideChar(CP_UTF8, 0, lpszText, -1, 0, 0);
	LPWSTR lpszReturnText = (LPWSTR)GlobalAlloc(0, dwSize * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, lpszText, -1, lpszReturnText, dwSize);
	return lpszReturnText;
}

LPSTR w2a(LPCWSTR lpszText)
{
	const DWORD dwSize = WideCharToMultiByte(CP_UTF8, 0, lpszText, -1, 0, 0, 0, 0);
	LPSTR lpszReturnText = (char*)GlobalAlloc(GPTR, dwSize * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, lpszText, -1, lpszReturnText, dwSize, 0, 0);
	return lpszReturnText;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;
	static HWND hEdit2;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindow(TEXT("EDIT"), TEXT("{\r\n  \"item\": [\r\n    {\r\n      \"name\": \"けんじ\",\r\n      \"age\" : 20\r\n    }\r\n  ]\r\n}"), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("Parse"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindow(TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		MoveWindow(hEdit1, 10, 50, 256, 160, TRUE);
		MoveWindow(hEdit2, 10, 218, 256, 160, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			DWORD dwSize = GetWindowTextLength(hEdit1);
			LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, sizeof(WCHAR) * (dwSize + 1));
			GetWindowTextW(hEdit1, lpszText, dwSize + 1);
			LPSTR lpszTextA = w2a(lpszText);
			GlobalFree(lpszText);
			std::string src(lpszTextA);
			GlobalFree(lpszTextA);
			std::string err;
			json11::Json v = json11::Json::parse(src, err);
			SetWindowText(hEdit2, 0);
			for (auto &item : v["item"].array_items()) {
				LPWSTR lpszText;
				lpszText = a2w(item["name"].string_value().c_str());
				SendMessageW(hEdit2, EM_REPLACESEL, 0, (LPARAM)lpszText);
				SendMessageW(hEdit2, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
				GlobalFree(lpszText);
				WCHAR szText[16];
				wsprintfW(szText, L"%d", item["age"].int_value());
				SendMessageW(hEdit2, EM_REPLACESEL, 0, (LPARAM)szText);
				SendMessageW(hEdit2, EM_REPLACESEL, 0, (LPARAM)L"\r\n");
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("JsonParse"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

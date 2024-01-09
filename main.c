#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <wchar.h>
#include "Application.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
WCHAR buffer[256 * 16] = { 0 };

Application app;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Vulkan Trial");
	HWND hwnd;
	MSG msg = {
		.wParam = 0,
	};
	WNDCLASS wndclass = {
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIcon(NULL, IDI_APPLICATION),
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH),
		.lpszMenuName = NULL,
		.lpszClassName = szAppName,
	};
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName, // window class name
		TEXT("The Hello Program"), // window caption
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, // initial x position
		CW_USEDEFAULT, // initial y position
		CW_USEDEFAULT, // initial x size
		CW_USEDEFAULT, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters

	init(&app, "Vulkan Trial", "No Engine", hInstance, hwnd);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	destroy(&app);
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		//PlaySound(TEXT("hellowin.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return 0;
	case WM_PAINT:
//		hdc = BeginPaint(hwnd, &ps);
//		GetClientRect(hwnd, &rect);
//		//DrawText(hdc, buffer, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//		EndPaint(hwnd, &ps);
		drawFrame(&app);
		//UpdateWindow(hwnd);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

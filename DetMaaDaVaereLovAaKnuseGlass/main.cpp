#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <strsafe.h>
#include <thread>
#include <TlHelp32.h>
#include <algorithm>
#include "geterror.h"
#include "resource.h"

// Globale variabler.
COLORREF TrueWhite = RGB(255, 255, 255);
HBRUSH BgBrush;
HDC hdc, BufferDC;
HBITMAP BufferBitmap;
RECT GameClientRect;
HWND hWnd;
bool GlassBroken = false;

void Avslutt()
{
	PlaySound(
		MAKEINTRESOURCE(DML),
		GetModuleHandle(NULL),
		SND_RESOURCE | SND_NODEFAULT
	);
	exit(0);
}

//
//	Oppdaterer projisering.
//
const wchar_t* bilder[] = {
	L"KNUSBILDE_1",
	L"KNUSBILDE_2"
};
void UpdateProjection()
{
	//
	//	Opprett mellomlager for tegning.
	//	Vi lar front-HDC være i fred mens vi tegner på et mellomlager
	//	og flipper over dette når vi er ferdig - kun én oppdatering. 
	//
	BufferDC = CreateCompatibleDC(hdc);
	//BufferBitmap = CreateCompatibleBitmap(hdc, cx, cy);
	//BufferBitmap = LoadBitmap(GetModuleHandle(NULL), bilder[rand() % 1]);
	BufferBitmap = (HBITMAP)LoadImage(
		GetModuleHandle(NULL),
		bilder[rand() % 2],
		IMAGE_BITMAP,
		0,
		0,
		LR_DEFAULTCOLOR
	);
	SelectObject(BufferDC, BufferBitmap);

	//
	//	Skriv mellomlager til prosjektoren og rydd opp.
	//
	int cx = GameClientRect.right;
	int cy = GameClientRect.bottom;
	BitBlt(hdc, (rand() % cx)-250, (rand() % cy)-250, cx, cy, BufferDC, 0, 0, SRCCOPY);
	DeleteObject(BufferBitmap);
	DeleteDC(BufferDC);

	//
	//	Spill knuselyd.
	//
	PlaySound(
		MAKEINTRESOURCE(KNUSLYD_1),
		GetModuleHandle(NULL),
		SND_RESOURCE | SND_ASYNC | SND_NODEFAULT
	);
}

LRESULT __stdcall WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	return 0;
}

int __stdcall wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR lpCmdLine,
	int nCmdShow
) {
	WNDCLASSEX wc;
	MSG Msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0;
	wc.hIconSm = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"MainClass";

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Error registering window.", L"Oisann!", MB_OK | MB_ICONERROR);
		exit(EXIT_FAILURE);
	}

	hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, L"MainClass", L"Knusenarr", WS_POPUP, 0, 0, 1920, 1080, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, L"Error during creating window.", L"Oisann!", MB_OK | MB_ICONERROR);
		exit(EXIT_FAILURE);
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), NULL, LWA_COLORKEY);
	hdc = GetDC(hWnd);
	GetClientRect(hWnd, &GameClientRect);

	// Hovedloop.
	do {
		Sleep(10); // CPU. 

		// Håndter systemmeldinger til vinduet, diskré.
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}

		// Knusekombo.
		if (
			(GetAsyncKeyState(0x41) & 0x8000) && // A
			(GetAsyncKeyState(0x44) & 0x8000) && // D
			(GetAsyncKeyState(0x57) & 0x8000) && // W
			!GlassBroken
		) {
			srand((unsigned int)time(0));
			try { UpdateProjection(); }
			catch (...) {
				GetError(L"Feil under oppdatering av projisering.");
				exit(EXIT_FAILURE);
			}
			GlassBroken = true;
		}

		if (
			(GetAsyncKeyState(0x41) & 0x8000) == 0 && // A
			(GetAsyncKeyState(0x44) & 0x8000) == 0 && // D
			(GetAsyncKeyState(0x57) & 0x8000) == 0 // W
		) {
			GlassBroken = false;
		}

		// Pass på at vindu alltid er øverst.
		SetWindowPos(
			hWnd,
			HWND_TOPMOST,
			GameClientRect.left,
			GameClientRect.top,
			GameClientRect.right - GameClientRect.left,
			GameClientRect.bottom - GameClientRect.top,
			SWP_SHOWWINDOW
		);

		// FIX combo for DML lyd og avslutning.
		if (
			(GetAsyncKeyState(0x4C) & 0x8000) && // L
			(GetAsyncKeyState(0x4F) & 0x8000) && // O
			(GetAsyncKeyState(0x56) & 0x8000)    // V
		) {
			Avslutt();
			break;
		}

	} while (Msg.message != WM_QUIT);

	return (int)Msg.wParam;
}

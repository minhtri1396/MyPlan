#include "stdafx.h"
#include "WndAddWork.h"
#include "CText.h"

#include <windowsx.h>

#include <winuser.h>
#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

extern CListView* lvWorks;
extern HINSTANCE hInst;
HWND hParent;
HWND hwndWorkName;
HWND hwndPriority;
HWND hwndTimePercent;
HWND hwndAddWorkButton;

void RegisterWndAddWork(HINSTANCE hInstance);
LRESULT CALLBACK WndAddWorkProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnCommandWndAddWork(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL OnCreateWndAddWork(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroyWndAddWork(HWND hwnd);
void(*m_InformChanged)(); //auto call this func after changing lvWork;

void InitWndAddWork(HINSTANCE hInstance, CListView *_lvWorks)
{
	lvWorks = _lvWorks;
	hInst = hInstance;
	RegisterWndAddWork(hInstance);
}

void StartWndAddWork(HWND _hParent, void(*_InformChanged)())
{
	hParent = _hParent;
	m_InformChanged = _InformChanged;
	HWND hWndMain = CreateWindowW(L"addwork", L"Add Work", WS_SYSMENU,
		CW_USEDEFAULT, 0, 680, 200, nullptr, nullptr, hInst, nullptr);
	if (hWndMain)
	{
		ShowWindow(hWndMain, SW_SHOW);
		//UpdateWindow(hWndMain);
	}
	EnableWindow(hParent, FALSE);
}

void RegisterWndAddWork(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndAddWorkProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPLAN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = L"Add Work";
	wcex.lpszClassName = L"addwork";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndAddWorkProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommandWndAddWork);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreateWndAddWork);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroyWndAddWork);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void OnCommandWndAddWork(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_ADDWORKBUTTON:
		PWORK newWork;
		__int8 iPriority, iTimePercent;
		__int16 selectedCount;
		__int16* arrIndex;
		WCHAR *lpwWorkName, *lpwPriority, *lpwTimePercent;

		lpwWorkName = CText::GetText(hwndWorkName);
		if (wcslen(lpwWorkName) == 0)
		{
			MessageBox(0, L"Bạn không thể bỏ trống tên công việc.", L"Lỗi empty", 0);
			free(lpwWorkName);
			return;
		}

		lpwPriority = CText::GetText(hwndPriority);
		lpwTimePercent = CText::GetText(hwndTimePercent);
		if (!CText::IsNumber(lpwTimePercent))
		{
			free(lpwPriority);
			free(lpwTimePercent);
			MessageBox(0, L"Phần trăm thời gian chỉ có thể chứa ký tự là số.", L"Lỗi ký tự", 0);
			return;
		}

		iPriority = CText::WcToInt8(lpwPriority);
		iTimePercent = CText::WcToInt8(lpwTimePercent);

		newWork = InitWork(lpwWorkName, iPriority, iTimePercent);

		if (!newWork) //Nếu thêm iTimePercent này thì tổng các iTimePercen đã vượt quá 100%
		{
			MessageBox(0, L"Tổng phần trăm các thời gian của từng công việc không thể vượt quá 100%.", L"Lỗi vượt quá 100%", 0);
		}
		else
		{
			if (!lvWorks->AddWork(newWork))
			{
				CWork::Terminate(newWork->TIMEPERCENT());
				delete[]newWork->NAME();
				free(newWork);
				MessageBox(0, L"Tên công việc đã tồn tại.", L"Trùng tên", 0);
			}
			else
			{
				m_InformChanged();
			}
		}

		delete[]lpwPriority;
		delete[]lpwTimePercent;
		delete[]lpwWorkName;

		break;
	}
}

BOOL OnCreateWndAddWork(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// Get system font
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(20, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));

	//Name of work
	HWND wnd = CreateWindowEx(0, WC_STATIC, L"Tên công việc:", WS_CHILD | WS_VISIBLE | SS_CENTER,
		10, 20, 150, 25, hwnd, NULL, hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	hwndWorkName = CreateWindowEx(0, WC_EDIT, L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
		160, 20, 460, 25, hwnd, NULL, hInst, NULL);
	SendMessage(hwndWorkName, WM_SETFONT, (WPARAM)hFont, TRUE);

	//Priority
	wnd = CreateWindowEx(0, WC_STATIC, L"Độ ưu tiên:", WS_CHILD | WS_VISIBLE | SS_CENTER,
		10, 60, 150, 25, hwnd, NULL, hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	hwndPriority = CreateWindowEx(0, WC_COMBOBOX, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL,
		160, 60, 50, 25, hwnd, NULL, hInst, NULL);
	SendMessage(hwndPriority, WM_SETFONT, (WPARAM)hFont, TRUE);
	WCHAR wstrPriority[3];
	for (__int8 i = 1; i < 11; ++i)
	{
		_itow(i, wstrPriority, 10);
		SendMessage(hwndPriority, CB_ADDSTRING, NULL, (LPARAM)wstrPriority);
	}
	SendMessage(hwndPriority, CB_SETCURSEL, 0, (LPARAM)L"1");

	//Percent of time
	wnd = CreateWindowEx(0, WC_STATIC, L"Time percent:", WS_CHILD | WS_VISIBLE | SS_CENTER,
		320, 60, 150, 25, hwnd, NULL, hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	hwndTimePercent = CreateWindowEx(0, WC_EDIT, L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
		490, 60, 32, 25, hwnd, NULL, hInst, NULL);
	SendMessage(hwndTimePercent, WM_SETFONT, (WPARAM)hFont, TRUE);

	//Adding Work Button
	hwndAddWorkButton = CreateWindowEx(0, WC_BUTTON, L"Thêm", WS_CHILD | WS_VISIBLE | SS_CENTER | BS_PUSHBUTTON,
		26, 115, 602, 28, hwnd, (HMENU)IDC_ADDWORKBUTTON, hInst, NULL);
	SendMessage(hwndAddWorkButton, WM_SETFONT, (WPARAM)hFont, TRUE);

	return true;
}

void OnDestroyWndAddWork(HWND hwnd)
{
	EnableWindow(hParent, TRUE);
	SetFocus(hParent);
}

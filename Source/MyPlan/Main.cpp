// MyPlan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Main.h"
#include "WndAddWork.h"
#include "RibbonIDs.h"
#include "RibbonFramework.h"
#include <windowsx.h>

#include <commdlg.h>
#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
UINT uiHeightRibbon = 147;
HWND hThisWnd;									// this window
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szCurDirectoryPath[MAX_PATH];
BOOL bListWorkChanged;							//if(bListWorkChanged) then call Save() before Exit or Load
BOOL bStarted = FALSE;
CListView *lvWorks = NULL;
wchar_t lpwCurPathFile[MAX_PATH] = { 0 };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//Command functions
void				OnClose(HWND hwnd);
BOOL				OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
LRESULT				OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);
void				OnSize(HWND hwnd, UINT state, int cx, int cy);
void				OnDestroy(HWND hwnd);

//File and ListView functions
void				AddNewWork();
VOID				New();
BOOL				SaveAs();
BOOL				Save();
VOID				Load();

//Automatically called when  ribbon changed it's size (user click hidden ribbon)
void				Ribbon_ChangedSize(UINT _newHeight);
//Automatically called when ribbon excuted
void				RibbonElement_Excute(UINT nCmdID);
//Inform list of works has been changed
VOID				ListWork_HasChanged();
//Print file name on title of main window
VOID				PrintFileName(wchar_t *fileName);

//INI functions
VOID SetWidthToINI();
VOID SetHeightToINI();
UINT GetWidthFromINI();
UINT GetHeightFromINI();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	GetCurrentDirectory(MAX_PATH, szCurDirectoryPath);

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MYPLAN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYPLAN));

	MSG msg;

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

	wcex.style = 0; //CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPLAN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MYPLAN);
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

	UINT cx = GetWidthFromINI();
	UINT cy = GetHeightFromINI();

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX,
		CW_USEDEFAULT, 0, cx, cy, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
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
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
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
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

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

void OnClose(HWND hwnd)
{
	BOOL bCanExit = TRUE;
	if (bListWorkChanged)
	{
		HRESULT hrMsg = MessageBox(0, L"Bạn có muốn lưu danh sách công việc hiện tại?", L"Lưu danh sách", MB_YESNOCANCEL);
		if (hrMsg == IDYES)
		{
			bCanExit = Save();
		}
		else if (hrMsg == IDCANCEL)
		{
			bCanExit = FALSE;
		}
	}
	if (bCanExit)
	{
		DestroyWindow(hwnd);
	}
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

	//Ribbon
	if (!InitializeFramework(hwnd, Ribbon_ChangedSize, RibbonElement_Excute))
	{
		return -1;
	}

	// Get system font
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(20, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));

	//ListView of Works
	lvWorks = new CListView(hwnd, hInst, hFont);
	lvWorks->Init(1000, 440, 0, 147);
	lvWorks->InitBasicColumns();

	//Add work window
	InitWndAddWork(hInst, lvWorks);
	hThisWnd = hwnd;

	return true;
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (lvWorks)
	{
		HWND hListView = lvWorks->GetHandle();
		RECT lvRect;
		GetWindowRect(hListView, &lvRect);

		RECT mainRect;
		GetWindowRect(hThisWnd, &mainRect);

		int height = mainRect.bottom - mainRect.top;

		if (height < 251)
		{
			uiHeightRibbon = 32;
		}
		else
		{
			uiHeightRibbon = 147;
		}

		SetWindowPos(hListView, NULL, 0, uiHeightRibbon, mainRect.right - mainRect.left - 15, height - uiHeightRibbon - 5, 0);
	}
}

void OnDestroy(HWND hwnd)
{
	SetWidthToINI();
	SetHeightToINI();
	delete lvWorks;
	PostQuitMessage(0);
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	LPNMTOOLBAR lpnmToolBar = (LPNMTOOLBAR)pnm;

	if (true) //Để tránh vòng lặp lẩn quẩn T_T
	{
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;

		switch (pnm->code)
		{
		case NM_CLICK:
			if (!bStarted && pnm->hwndFrom == lvWorks->GetHandle())
			{
				if (ListView_GetSelectedCount(pnm->hwndFrom) == 1)
				{
					HRESULT hr = MessageBox(0, L"Bạn chắc chắc muốn xóa ghi chú này?", L"Xóa ghi chú", MB_YESNO);
					if (hr == IDYES)
					{
						lvWorks->RemoveSelectedWork();
						ListWork_HasChanged();
					}
				}
			}
			break; //case NM_CLICK:
		}//switch
	}//if

	return 0;
}

VOID AddNewWork()
{
	StartWndAddWork(hThisWnd, ListWork_HasChanged);
}

VOID New()
{
	BOOL bCanCreateNew = TRUE;
	if (bListWorkChanged)
	{
		HRESULT hrMsg = MessageBox(0, L"Bạn có muốn lưu danh sách công việc hiện tại?", L"Lưu danh sách", MB_YESNOCANCEL);
		if (hrMsg == IDYES)
		{
			Save();
		}
		else if (hrMsg == IDCANCEL)
		{
			bCanCreateNew = FALSE;
		}
	}
	if (bCanCreateNew)
	{
		lpwCurPathFile[0] = '\0';
		PrintFileName(lpwCurPathFile);
		bListWorkChanged = FALSE;
		lvWorks->RemoveAll();
	}
}

BOOL SaveAs()
{
	HRESULT hrMsg;
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hThisWnd;
	ofn.lpstrFile = lpwCurPathFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"My Plan (*.mp)\0*.mp\0"; //double zero terminated
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		PrintFileName(lpwCurPathFile);
		bListWorkChanged = FALSE;
		lvWorks->Save(ofn.lpstrFile);
		return TRUE;
	}

	return FALSE;
}

BOOL Save()
{
	BOOL bSuccess = TRUE;
	if (wcslen(lpwCurPathFile)) //have path (from loading file or saving as before)
	{
		bListWorkChanged = FALSE;
		lvWorks->Save(lpwCurPathFile);
	}
	else
	{
		bSuccess = SaveAs();
	}
	return bSuccess;
}

VOID Load()
{
	bool bCanLoad = true;
	HRESULT hrMsg;
	OPENFILENAME ofn;
	wchar_t fileName[MAX_PATH];
	fileName[0] = '\0';

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hThisWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"My Plan (*.mp)\0*.mp\0"; //double zero terminated
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		if (bListWorkChanged) // != 0
		{
			hrMsg = MessageBox(0, L"Bạn có muốn lưu danh sách công việc hiện tại?", L"Lưu danh sách", MB_YESNOCANCEL);
			if (hrMsg == IDYES)
			{
				Save();
			}
			else if (hrMsg == IDCANCEL)
			{
				bCanLoad = false;
			}
		} //if (ListView_GetItemCount(lvWorks->GetHandle()))
		if (bCanLoad)
		{
			bListWorkChanged = false;
			wcscpy(lpwCurPathFile, fileName);
			PrintFileName(lpwCurPathFile);
			lvWorks->Load(lpwCurPathFile);
		}
	} // if (GetOpenFileName(&ofn) == TRUE)
}

//Automatically called when  ribbon changed it's size (user click hidden ribbon)
void Ribbon_ChangedSize(UINT _newHeight)
{
	if (lvWorks)
	{
		HWND hListView = lvWorks->GetHandle();
		RECT mainRect;
		GetWindowRect(hThisWnd, &mainRect);
		SetWindowPos(hListView, NULL, 0, _newHeight, mainRect.right - mainRect.left - 15,
			mainRect.bottom - mainRect.top - _newHeight - 5, 0);
		uiHeightRibbon = _newHeight;
	}
}

//Automatically called when ribbon excuted
void RibbonElement_Excute(UINT nCmdID)
{
	switch (nCmdID)
	{
	case ID_BTN_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), 0, About);
		break;
	case ID_BTN_EXIT:
		OnClose(hThisWnd);
		break;
	case ID_BTN_NEW:
		New();
		break;
	case ID_BTN_ADD:
		AddNewWork();
		break;
	case ID_BTN_SAVEAS:
		SaveAs();
		break;
	case ID_BTN_SAVE:
		Save();
		break;
	case ID_BTN_OPEN:
		Load();
		break;
	case ID_BTN_SORTASCNAME:
		lvWorks->Sort(L"TÊN CÔNG VIỆC", TRUE);
		break;
	case ID_BTN_SORTASCPRIORITY:
		lvWorks->Sort(L"ĐỘ ƯU TIÊN", TRUE);
		break;
	case ID_BTN_SORTASCTIMEPERCENT:
		lvWorks->Sort(L"PHẦN TRĂM THỜI GIAN", TRUE);
		break;
	case ID_BTN_SORTDESCNAME:
		lvWorks->Sort(L"TÊN CÔNG VIỆC", FALSE);
		break;
	case ID_BTN_SORTDESCPRIORITY:
		lvWorks->Sort(L"ĐỘ ƯU TIÊN", FALSE);
		break;
	case ID_BTN_SORTDESCTIMEPERCENT:
		lvWorks->Sort(L"PHẦN TRĂM THỜI GIAN", FALSE);
		break;
	}
}

//Inform list of works has been changed
VOID ListWork_HasChanged()
{
	bListWorkChanged = TRUE;
}

//Print file name on title of main window
VOID PrintFileName(wchar_t *fileName)
{
	wchar_t buffer[1001];
	wcscpy(buffer, szTitle);
	if (wcslen(fileName))
	{
		wcscat(buffer, L" (");
		wcscat(buffer, fileName);
		wcscat(buffer, L")");
	}
	SetWindowText(hThisWnd, buffer);
}

//INI functions
VOID SetWidthToINI()
{
	WCHAR lpwWidth[7];
	RECT mainRect;
	GetWindowRect(hThisWnd, &mainRect);
	_itow(mainRect.right - mainRect.left, lpwWidth, 10);

	WCHAR configPath[MAX_PATH];
	wsprintf(configPath, L"%s\\config.ini", szCurDirectoryPath);

	WritePrivateProfileString(szTitle, L"Width", lpwWidth, configPath);
}

VOID SetHeightToINI()
{
	WCHAR lpwHeight[7];
	RECT mainRect;
	GetWindowRect(hThisWnd, &mainRect);
	_itow(mainRect.bottom - mainRect.top, lpwHeight, 10);

	WCHAR configPath[MAX_PATH];
	wsprintf(configPath, L"%s\\config.ini", szCurDirectoryPath);

	WritePrivateProfileString(szTitle, L"Height", lpwHeight, configPath);
}

BOOL IsFileExisted(wchar_t *szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

UINT GetWidthFromINI()
{
	WCHAR configPath[MAX_PATH];
	wsprintf(configPath, L"%s\\config.ini", szCurDirectoryPath);

	UINT cx = 1000;
	if (IsFileExisted(configPath))
	{
		cx = GetPrivateProfileInt(szTitle, L"Width", MAX_PATH, configPath);
	}
	return cx;
}

UINT GetHeightFromINI()
{
	WCHAR configPath[MAX_PATH];
	wsprintf(configPath, L"%s\\config.ini", szCurDirectoryPath);

	UINT cy = 700;
	if (IsFileExisted(configPath))
	{
		cy = GetPrivateProfileInt(szTitle, L"Height", MAX_PATH, configPath);
	}

	return cy;
}

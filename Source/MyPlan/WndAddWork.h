#include "resource.h"
#include "CListView.h"
#include <Windows.h>

#ifndef WNDADDWORK
#define WNDADDWORK

void InitWndAddWork(HINSTANCE hInstance, CListView *_lvWorks);
void StartWndAddWork(HWND _hParent, void(*_InformChanged)());

#endif //WNDADDWORK
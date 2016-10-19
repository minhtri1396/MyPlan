#include "stdafx.h"
#include "CListView.h"
#include "CFile.h"
#include "Resource.h"
#include <thread>
#include <commctrl.h>

HWND CListView::GetHandle()
{
	return m_hListView;
}

CListView::CListView(HWND _hParentWnd, HINSTANCE _hInst, HFONT _hFont)
{
	m_bAddFromOut = true;
	m_nColumn = 0;
	m_nRow = 0;
	m_hParentWnd = _hParentWnd;
	m_hInst = _hInst;
	m_hFont = _hFont;
	m_lstWork = new WORKS();
}

CListView::~CListView()
{
	delete m_lstWork;
}

void CListView::Init(_In_ __int32 _width, _In_ __int32 _height, _In_ __int16 _x, _In_ __int16 _y)
{
	m_hListView = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT | LVS_SINGLESEL, \
		_x, _y, _width, _height, m_hParentWnd, NULL, m_hInst, NULL);

	ListView_SetExtendedListViewStyle(m_hListView, LVS_EX_GRIDLINES);

	//ListView_SetExtendedListViewStyle(m_hListView, LVS_EX_FULLROWSELECT);

	SendMessage(m_hListView, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SetImage();
}

void CListView::AddColumn(_In_ WCHAR* _colName, _In_ __int32 _width)
{
	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_CENTER;
	lvCol.cx = _width;
	lvCol.pszText = _colName;
	ListView_InsertColumn(m_hListView, m_nColumn++, &lvCol);
}

void CListView::InitBasicColumns()
{
	AddColumn(L"", 50);
	AddColumn(L"TÊN CÔNG VIỆC", 610);
	AddColumn(L"ĐỘ ƯU TIÊN", 133);
	AddColumn(L"PHẦN TRĂM", 190);
}

void CListView::SetImage()
{
	HICON hiconItem;
	HIMAGELIST hSmall;

	hSmall = ImageList_Create(20, 20, ILC_COLOR32 | ILC_MASK, 1, 1);

	// Add an icon to each image list.  
	hiconItem = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_RECYCLE_BIN));

	//ImageList_AddIcon(hLarge, hiconItem);
	ImageList_AddIcon(hSmall, hiconItem);

	DestroyIcon(hiconItem);

	// Assign the image lists to the list-view control.
	ListView_SetImageList(m_hListView, hSmall, LVSIL_SMALL);
}

void  CListView::AddRow(_In_ int _nColumn, ...)
{
	va_list ap;
	va_start(ap, _nColumn);

	//Thêm Item mới (row mới) và cột đầu tiên
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = m_nRow++;
	lvItem.iImage = IDI_RECYCLE_BIN;
	lvItem.iSubItem = 0;
	lvItem.pszText = va_arg(ap, WCHAR*);
	ListView_InsertItem(m_hListView, &lvItem);

	//Thêm các subitem (các cột còn lại)
	for (int i = 1; i < _nColumn; i++)
	{
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = i;
		lvItem.pszText = va_arg(ap, WCHAR*);
		ListView_SetItem(m_hListView, &lvItem);
	}

	va_end(ap);
}

BOOL CListView::AddWork(_In_ const PWORK &_pWork)
{
	WCHAR priority[3], timePercent[4];
	_itow(_pWork->PRIORITY(), priority, 10);
	_itow(_pWork->TIMEPERCENT(), timePercent, 10);
	if (m_bAddFromOut) //User called this function
	{
		if (m_lstWork->Add(_pWork))
		{
			AddRow(4, L"", _pWork->NAME(), priority, timePercent);
			return TRUE;
		}
	}
	else //System called this function
	{
		AddRow(4, L"", _pWork->NAME(), priority, timePercent);
		return TRUE;
	}

	return FALSE;
}

void CListView::RemoveSelectedWork()
{
	__int16 iRemove = GetSelectedIndex();
	m_lstWork->Remove(iRemove);
	ListView_DeleteItem(m_hListView, iRemove);
	--m_nRow;
}

__int16 CListView::Size()
{
	return m_lstWork->Size();
}

__int16 CListView::GetSelectedIndex()
{
	__int16 index = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
	return index;
}

void CListView::Sort(_In_ WCHAR* _type, _In_ BOOL _sortAsc)
{
	if (_type[0] == L'P') //PHẦN TRĂM THỜI GIAN
	{
		m_lstWork->SortTimePer(_sortAsc);
	}
	else if (_type[0] == L'Đ') //ĐỘ ƯU TIÊN
	{
		m_lstWork->SortPriority(_sortAsc);
	}
	else //TÊN CÔNG VIỆC
	{
		m_lstWork->SortName(_sortAsc);
	}

	m_nRow = 0;
	ListView_DeleteAllItems(m_hListView);

	WCHAR priority[3], timePercent[4];
	__int16 size = Size();
	for (__int16 i = 0; i < size; ++i)
	{
		WORK* w = m_lstWork->operator[](i);
		_itow(w->PRIORITY(), priority, 10);
		_itow(w->TIMEPERCENT(), timePercent, 10);

		AddRow(4, L"", w->NAME(), priority, timePercent);
	}
}

void CListView::Save(_In_ wchar_t *_path)
{
	CFile::Save(m_lstWork, _path);
}

void CListView::Load(_In_ wchar_t *_path)
{
	RemoveAll();
	WORKS *lsWork = CFile::Load(_path);
	m_lstWork = lsWork;

	m_bAddFromOut = false; //đánh dấu việc thêm công việc là từ hệ thống
	__int16 size = lsWork->Size();
	for (__int16 i = 0; i < size; ++i)
	{
		AddWork(lsWork->operator[](i));
	}
	m_bAddFromOut = true; //đánh dấu việc thêm công việc là từ user
}

void CListView::RemoveAll()
{
	delete m_lstWork;
	m_lstWork = new WORKS();
	ListView_DeleteAllItems(m_hListView);
	m_nRow = 0;
}

__int16 minutes;
void Counter(_In_ WORKS &_lstWork)
{
	__int16 passedMinutes = 0;
	__int16 count = 0;
	__int16 size = _lstWork.Size() - 1;
	for (__int16 i = 0; i < size; ++i)
	{
		PWORK w = _lstWork[i];
		count = w->TIMEPERCENT() * ::minutes / 100.0;
		passedMinutes += count;
		while (count)
		{
			--count;
			Sleep(1000);
		}
	}

	count = ::minutes - passedMinutes;
	while (count)
	{
		--count;
		Sleep(1000);
	}
}

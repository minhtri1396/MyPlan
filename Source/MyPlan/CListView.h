#include "CWorks.h"
#include <Windows.h>

#ifndef CLISTVIEW
#define CLISTVIEW

typedef class CListView
{
private:
	bool m_bAddFromOut;
	int m_nColumn, m_nRow;
	HWND m_hParentWnd, m_hListView;
	HINSTANCE m_hInst;
	HFONT m_hFont;
	WORKS *m_lstWork;

	void AddColumn(_In_ WCHAR* _colName, _In_ __int32 _width);
	void AddRow(_In_ int _nColumn, ...);
	void SetImage();
public:
	HWND GetHandle();
	CListView(_In_ HWND _hParentWnd, _In_ HINSTANCE _hInst, _In_ HFONT _hFont);
	~CListView();
	void Init(_In_ __int32 _width, _In_ __int32 _height, _In_ __int16 _x, _In_ __int16 _y);
	void InitBasicColumns();
	BOOL AddWork(_In_ const PWORK &_pWork);
	void RemoveSelectedWork();
	__int16 Size();
	__int16 GetSelectedIndex();
	void Sort(_In_ WCHAR* _type, _In_ BOOL _sortAsc);
	void Save(_In_ wchar_t *_path);
	void Load(_In_ wchar_t *_path);
	void RemoveAll();
} LISTVIEW;

#endif //CLISTVIEW

#include "stdafx.h"
#include "CWorks.h"
#include <algorithm>

CWorks::~CWorks()
{
	RemoveAll();
}

BOOL CWorks::Add(_In_ const PWORK& _pWork)
{
	WCHAR *lpwNewWorkName = _wcsupr(_pWork->NAME());
	int sizeWorkList = m_vecWorkList.size();

	for (int i = 0; i < sizeWorkList; ++i)
	{
		if (wcscmp(_wcsupr(m_vecWorkList[i]->NAME()), _wcsupr(lpwNewWorkName)) == 0)
		{
			return FALSE;
		}
	}

	m_vecWorkList.push_back(_pWork);

	return TRUE;
}

__int16 CWorks::Remove(_In_ const PWORK& _pWork)
{
	WCHAR *lpwNewWorkName = _wcsupr(_pWork->NAME());
	int sizeWorkList = m_vecWorkList.size();

	for (int i = 0; i < sizeWorkList; ++i)
	{
		if (wcscmp(_wcsupr(m_vecWorkList[i]->NAME()), _wcsupr(lpwNewWorkName)) == 0)
		{
			delete m_vecWorkList[i];
			m_vecWorkList.erase(m_vecWorkList.begin() + i);
			return i;
		}
	}
	return -1;
}

__int16 CWorks::Remove(_In_ __int16 iRemove)
{
	if (iRemove < 0 || iRemove >= m_vecWorkList.size())
	{
		return FALSE;
	}

	delete m_vecWorkList[iRemove];
	m_vecWorkList.erase(m_vecWorkList.begin() + iRemove);
	return TRUE;
}

void CWorks::RemoveAll()
{
	int sizeWorkList = m_vecWorkList.size();
	while (sizeWorkList > 0)
	{
		delete m_vecWorkList[--sizeWorkList];
	}

	m_vecWorkList.clear();
}

__int16 CWorks::Size()
{
	return (__int16)m_vecWorkList.size(); //số lượng công việc có trong list không quá lớn
										  //nên chỉ cần dùng số nguyên 16 bit là đủ
}

WORK* CWorks::operator[](_In_ __int16 _index)
{
	return m_vecWorkList[_index];
}

//Compare to sort ascending
BOOL AscName(PWORK w1, PWORK w2)
{
	WCHAR* name1 = w1->NAME();
	WCHAR* name2 = w2->NAME();
	__int8 k1 = 0, k2 = 0;

	while (name1[k1] && name2[k2])
	{
		if (name1[k1] < name2[k2])
		{
			return TRUE;
		}

		if (name1[k1] > name2[k2])
		{
			return FALSE;
		}

		++k1; ++k2;
	}

	if (wcslen(name1) < wcslen(name2))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL AscPriority(PWORK w1, PWORK w2)
{
	return w1->PRIORITY() < w2->PRIORITY();
}

BOOL AscTimePer(PWORK w1, PWORK w2)
{
	return w1->TIMEPERCENT() < w2->TIMEPERCENT();
}

//Compare to sort descending
BOOL DescName(PWORK w1, PWORK w2)
{
	WCHAR* name1 = w1->NAME();
	WCHAR* name2 = w2->NAME();
	__int8 k1 = 0, k2 = 0;

	while (name1[k1] && name2[k2])
	{
		if (name1[k1] > name2[k2])
		{
			return TRUE;
		}

		if (name1[k1] < name2[k2])
		{
			return FALSE;
		}

		++k1; ++k2;
	}

	if (wcslen(name1) > wcslen(name2))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL DescPriority(PWORK w1, PWORK w2)
{
	return w1->PRIORITY() > w2->PRIORITY();
}

BOOL DescTimePer(PWORK w1, PWORK w2)
{
	return w1->TIMEPERCENT() > w2->TIMEPERCENT();
}


void CWorks::SortName(_In_ BOOL sortAsc)
{
	if (sortAsc)
	{
		std::sort(m_vecWorkList.begin(), m_vecWorkList.end(), AscName);
	}
	else
	{
		std::sort(m_vecWorkList.begin(), m_vecWorkList.end(), DescName);
	}
}

void CWorks::SortPriority(_In_ BOOL sortAsc)
{
	if (sortAsc)
	{
		std::sort(m_vecWorkList.begin(), m_vecWorkList.end(), AscPriority);
	}
	else
	{
		std::sort(m_vecWorkList.begin(), m_vecWorkList.end(), DescPriority);
	}
}

void CWorks::SortTimePer(_In_ BOOL sortAsc)
{
	if (sortAsc)
	{
		std::sort(m_vecWorkList.begin(), m_vecWorkList.end(), AscTimePer);
	}
	else
	{
		std::sort(m_vecWorkList.begin(), m_vecWorkList.end(), DescTimePer);
	}
}
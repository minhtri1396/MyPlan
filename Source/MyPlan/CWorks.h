#include <vector>
#include "CWork.h"
#include <Windows.h>

#ifndef CWORKS
#define CWORKS

typedef class CWorks
{
private:
	std::vector<PWORK> m_vecWorkList;
public:
	CWorks() {}
	~CWorks();
	BOOL Add(_In_ const PWORK& _pWork);
	__int16 Remove(_In_ const PWORK& _pWork);
	__int16 Remove(_In_ __int16 iRemove);
	void RemoveAll();
	__int16 Size();
	WORK* operator[](_In_ __int16 _index);
	void SortName(_In_ BOOL sortAsc);
	void SortPriority(_In_ BOOL sortAsc);
	void SortTimePer(_In_ BOOL sortAsc);
} WORKS;

#endif //CWORKS

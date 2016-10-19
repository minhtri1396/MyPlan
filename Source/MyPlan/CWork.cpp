#include "stdafx.h"
#include "CWork.h"

static __int8 m_remainPer = 100;

WCHAR* CWork::NAME()
{
	return m_lpwName;
}

__int8 CWork::PRIORITY()
{
	return m_iPriority;
}

__int8 CWork::TIMEPERCENT()
{
	return m_iTimePer;
}

CWork* CWork::Init(_In_ WCHAR* _lpwName, _In_ __int8 _iPriority, _In_ __int8 _iTimePer)
{
	CWork* newWork = NULL;

	if (Register(_iTimePer))
	{
		newWork = new CWork();
		newWork->m_lpwName = _wcsdup(_lpwName);
		newWork->m_iPriority = _iPriority;
		newWork->m_iTimePer = _iTimePer;
	}

	return newWork;
}

CWork::CWork()
{
}

CWork::~CWork()
{
	Terminate(m_iTimePer);
	delete []m_lpwName;
}

/*
Tên Function: Register
Chức năng: đăng ký số phần trăm sử dụng
Input: số phần trăm muốn đăng ký dùng
Output:
+ TRUE: nếu đăng ký thành công (số phần trăm đăng ký <= số phần trăm còn lại, lấy từ NumClearPercent)
+ FALSE: nếu đăng ký thất bại (khi không thỏa TRUE hoặc phần trăm muốn đăng ký < 0)
*/
BOOL CWork::Register(_In_ __int8 _per)
{
	if (_per < 0 || _per > m_remainPer)
	{
		return FALSE;
	}

	m_remainPer -= _per;

	return TRUE;
}

/*
Tên Function: Terminate
Chức năng: hủy số phần trăm đã đăng ký
Input: số phần trăm muốn hủy
Output: none
*/
void CWork::Terminate(_In_ __int8 _per)
{
	if (_per < 0 || _per >(100 - m_remainPer))
	{
		return;
	}

	m_remainPer += _per;
}

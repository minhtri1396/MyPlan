#include "stdafx.h"
#include "CText.h"
#include <math.h>

//Chuyển chuỗi dãy số thành wchar
//Ex: _strCode = 65|66|67 => output: ABC
WCHAR* CText::DencyptToWchar(CHAR *_strCode)
{
	WCHAR *lpwStrResult = NULL;
	CHAR lpStrNum[11];

	__int32 lenStr = strlen(_strCode);
	__int32 lenWStr = lenStr;

	if (lenWStr > 0)
	{
		lpwStrResult = new WCHAR[lenWStr + 1];
		lenWStr = 0;
		__int32 ncStrNum = 0;
		for (__int32 i = 0; i < lenStr; ++i)
		{
			if (_strCode[i] == '|')
			{
				lpStrNum[ncStrNum] = 0;
				ncStrNum = 0;
				lpwStrResult[lenWStr++] = (wchar_t)atoi(lpStrNum);
			}
			else
			{
				lpStrNum[ncStrNum++] = _strCode[i];
			}
		} //for
		lpwStrResult[lenWStr++] = (wchar_t)atoi(lpStrNum);
		lpwStrResult[lenWStr] = 0;
	} //if
	return lpwStrResult;
}

//Chuyển chuỗi wchar thành dãy số
//Ex: _lpwStr = ABC => output: 65|66|67
CHAR* CText::EncyptFromWchar(WCHAR *_lpwStr)
{
	__int32 lenWStr = wcslen(_lpwStr);
	CHAR lpStrNum[11];
	CHAR buffer[1001];
	buffer[0] = 0;

	if (lenWStr)
	{
		__int16 iNum;
		for (__int32 i = 0; i < lenWStr; ++i)
		{
			iNum = (__int16)_lpwStr[i];
			itoa(iNum, lpStrNum, 10);
			if (strlen(buffer))
			{
				strcat(buffer, "|");
				strcat(buffer, lpStrNum);
			}
			else
			{
				strcpy(buffer, lpStrNum);
			}
		} //for
	} //if

	return strdup(buffer);
}

WCHAR* CText::GetText(_In_ HWND _hwnd)
{
	__int16 len = GetWindowTextLength(_hwnd);
	WCHAR *text = new WCHAR[len + 1];
	if (len == 0)
	{
		text[0] = 0;
	}
	else
	{
		GetWindowText(_hwnd, text, len + 1);
	}
	return text;
}

__int8 CText::WcToInt8(_In_opt_ WCHAR* _lpwStr)
{
	if (!_lpwStr)
	{
		return 0;
	}
	__int8 num = (__int8)_wtoi(_lpwStr);
	return num;
}

BOOL CText::IsNumber(_In_opt_ WCHAR* _lpwStr)
{
	__int8 k = 0;
	while (_lpwStr[k])
	{
		if (_lpwStr[k] < L'0' || _lpwStr[0] > L'9')
		{
			return FALSE;
		}
		++k;
	}

	return TRUE;
}

WCHAR* CText::CleanWhiteSpaces(_In_ WCHAR* _lpwStr)
{
	WCHAR* lpwStr = new WCHAR[wcslen(_lpwStr) + 1];
	__int8 kOld = 0, kNew = 0;
	while (_lpwStr[kOld])
	{
		if (_lpwStr[kOld] != L' ')
		{
			lpwStr[kNew++] = _lpwStr[kOld];
		}
		++kOld;
	}
	lpwStr[kNew] = 0;

	WCHAR* lpwTmp = _lpwStr;
	_lpwStr = lpwStr;
	lpwStr = _wcsdup(lpwStr); //giữ đúng số lượng ký tự (nếu đã có xóa đi khoảng trắng)
	delete[]_lpwStr;
	_lpwStr = lpwTmp;

	return lpwStr;
}

PWCHAR* CText::Split(_In_ WCHAR *_lpwStr, _In_ WCHAR wch, _Out_ __int8 &_count)
{
	_count = 0;
	__int8 k = 0;
	while (_lpwStr[k])
	{
		if (_lpwStr[k] == wch)
		{
			++_count;
		}
		++k;
	}

	++_count;
	PWCHAR* lpwRes = new PWCHAR[_count];
	k = 0;
	__int8 iRow = 0, iCol = 0;
	lpwRes[iRow] = new WCHAR[wcslen(_lpwStr) + 1];
	while (_lpwStr[k])
	{
		if (_lpwStr[k] == wch)
		{
			lpwRes[iRow][iCol] = 0;
			++iRow;
			iCol = 0;
			lpwRes[iRow] = new WCHAR[wcslen(_lpwStr) + 1];
		}
		else
		{
			lpwRes[iRow][iCol++] = _lpwStr[k];
		}
		++k;
	}
	lpwRes[iRow][iCol] = 0;

	return lpwRes;
}

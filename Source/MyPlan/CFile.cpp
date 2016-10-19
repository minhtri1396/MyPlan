#include "stdafx.h"
#include "CFile.h"
#include "CText.h"
#include <Windows.h>

//Ngăn cách các chuỗi sau khi gộp name, priority và timepercent của work thành 1 chuỗi
//VD: Name = "D|MT" Priority = 3 TimePercent = 0 => gộp thành chuỗi "D||MT|3|0"
wchar_t *CFile::Encoding(_In_ PWORK _work)
{
	wchar_t buffer[1001];
	__int16 nBuffer = 0;
	__int16 lenName = wcslen(_work->NAME());

	for (__int16 i = 0; i < lenName; ++i)
	{
		if (_work->NAME()[i] == '|')
		{
			buffer[nBuffer++] = '|';
		}
		buffer[nBuffer++] = _work->NAME()[i];
	}

	buffer[nBuffer] = '\0'; //NULL character
	wcscat(buffer, L"|");

	wchar_t wcStrNum[11];
	_itow(_work->PRIORITY(), wcStrNum, 10);
	wcscat(buffer, wcStrNum);
	wcscat(buffer, L"|");

	_itow(_work->TIMEPERCENT(), wcStrNum, 10);
	wcscat(buffer, wcStrNum);

	return wcsdup(buffer);
}

//Xóa dấu | do encoding tạo ra
PWORK CFile::Decoding(_In_ wchar_t *_lpwStr)
{
	__int32 len = wcslen(_lpwStr);
	__int32 nStr = 0;
	__int32 run;
	wchar_t lpwName[1001], lpwPriority[11], lpwTimePercent[11];
	//Get name
	for (run = 0; nStr < len; ++nStr)
	{
		if (_lpwStr[nStr] == '|')
		{
			++nStr;
			if (_lpwStr[nStr] != '|')
			{
				break;
			}
		}
		lpwName[run++] = _lpwStr[nStr];
	}
	lpwName[run] = 0;
	// Get priority
	for (run = 0; nStr < len; ++nStr)
	{
		if (_lpwStr[nStr] == '|')
		{
			++nStr;
			break;
		}
		lpwPriority[run++] = _lpwStr[nStr];
	}
	lpwPriority[run] = 0;
	// Get timepercent
	for (run = 0; nStr < len; ++nStr)
	{
		lpwTimePercent[run++] = _lpwStr[nStr];
	}
	lpwTimePercent[run] = 0;
	return WORK::Init(lpwName, _wtoi(lpwPriority), _wtoi(lpwTimePercent));
}

void CFile::SetByteToRead(_In_ HANDLE &_hFile, _In_ __int32 _nbytes)
{
	char pwNumBytes[10]; //the number of bytes
	_itoa(_nbytes, pwNumBytes, 10);

	//Write the number of digit //Ex: _nbytes = 15 => iNumDigit = 2
	__int8 iNumDigit = 0;
	while (_nbytes > 0)
	{
		_nbytes /= 10;
		++iNumDigit;
	}
	char pwNumDigit[2];
	_itoa(iNumDigit, pwNumDigit, 10);
	DWORD dwBytesWritten;
	WriteFile(
		_hFile,           // open file handle
		pwNumDigit,      // start of data to write
		1,				// number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure

	//Write _nbytes
	WriteFile(
		_hFile,           // open file handle
		pwNumBytes,      // start of data to write
		iNumDigit,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure
}

__int32 CFile::GetByteToRead(_In_ HANDLE &_hFile)
{
	DWORD dwBytesRead;
	char numOfDigit[2] = { 0 };
	char *lpwLen;

	ReadFile(_hFile, numOfDigit, 1, &dwBytesRead, NULL); //number of digit of lpwLen

	lpwLen = new char[atoi(numOfDigit) + 1]; //length of buffer need to create
	ReadFile(_hFile, lpwLen, atoi(numOfDigit), &dwBytesRead, NULL);
	lpwLen[atoi(numOfDigit)] = 0;

	__int32 dwBytesToRead = atoi(lpwLen);
	delete[]lpwLen;

	return dwBytesToRead;
}

void CFile::Save(_In_ WORKS* _lsWork, _In_ wchar_t *_path)
{
	HANDLE hFile = CreateFile(_path,                // name of the write
		FILE_GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile != INVALID_HANDLE_VALUE)
	{
		wchar_t *lpwEncoding;
		char *lpwStrSave;
		DWORD dwBytesToWrite;
		DWORD dwBytesWritten;
		BOOL bErrorFlag;
		__int16 sizeOfListWork = _lsWork->Size();
		for (__int16 i = 0; i < sizeOfListWork; ++i)
		{
			lpwEncoding = Encoding(_lsWork->operator[](i));
			lpwStrSave = CText::EncyptFromWchar(lpwEncoding);
			SetByteToRead(hFile, strlen(lpwStrSave));

			dwBytesToWrite = (DWORD)strlen(lpwStrSave);
			dwBytesWritten = 0;

			bErrorFlag = WriteFile(
				hFile,           // open file handle
				lpwStrSave,      // start of data to write
				dwBytesToWrite,  // number of bytes to write
				&dwBytesWritten, // number of bytes that were written
				NULL);            // no overlapped structure

			delete[]lpwEncoding;
			delete[]lpwStrSave;

			if (FALSE == bErrorFlag)
			{
				return;
			}
		}
		CloseHandle(hFile);
	}
}

WORKS* CFile::Load(_In_ wchar_t *_path)
{
	WORKS* lsWork = NULL;
	HANDLE hFile;

	hFile = CreateFile(_path,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile != INVALID_HANDLE_VALUE)
	{
		lsWork = new WORKS();

		DWORD  dwBytesToRead = 0;
		DWORD  dwBytesRead = 0;
		wchar_t *lpwWork;
		char *buffer;

		while (TRUE)
		{
			dwBytesToRead = GetByteToRead(hFile);
			if (dwBytesToRead == 0)
			{
				break;
			}

			buffer = new char[dwBytesToRead + 1];
			if (FALSE == ReadFile(hFile, buffer, dwBytesToRead, &dwBytesRead, NULL))
			{
				break;
			}

			buffer[dwBytesRead] = '\0'; // NULL character

			lpwWork = CText::DencyptToWchar(buffer);

			lsWork->Add(Decoding(lpwWork));

			delete[]buffer;
			delete[]lpwWork;
		} //while

		CloseHandle(hFile);
	} //if

	return lsWork;
}

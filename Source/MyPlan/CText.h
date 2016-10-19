#include <Windows.h>

#ifndef CTEXT
#define CTEXT

class CText
{
private:
	__int32 lenStr;
	__int32 lenWStr;
public:
	//Chuyển chuỗi dãy số thành wchar
	//Ex: _strCode = 65|66|67 => output: ABC
	static WCHAR* DencyptToWchar(CHAR *_strCode);
	//Chuyển chuỗi wchar thành dãy số
	//Ex: _lpwStr = ABC => output: 65|66|67
	static CHAR* EncyptFromWchar(WCHAR *_lpwStr);
	static WCHAR* GetText(_In_ HWND _hwnd);
	static __int8 WcToInt8(_In_opt_ WCHAR* _lpwStr);
	static BOOL IsNumber(_In_opt_ WCHAR* _lpwStr);
	static WCHAR* CleanWhiteSpaces(_In_ WCHAR* _lpwStr);
	static PWCHAR* Split(_In_ WCHAR *_lpwStr, _In_ WCHAR wch, _Out_ __int8 &_count);
};

#endif

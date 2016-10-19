#include "CWorks.h"

#ifndef CFILE
#define CFILE

class CFile
{
private:
	//Ngăn cách các chuỗi sau khi gộp name, priority và timepercent của work thành 1 chuỗi
	//VD: Name = "D|MT" Priority = 3 TimePercent = 0 => gộp thành chuỗi "D||MT|3|0"
	static wchar_t *Encoding(_In_ PWORK _work);
	//Xóa dấu | do encoding tạo ra
	static PWORK Decoding(_In_ wchar_t *_lpwStr);
	static void SetByteToRead(_In_ HANDLE &_hFile, _In_ __int32 _nbytes);
	static __int32 GetByteToRead(_In_ HANDLE &_hFile);
public:
	static void Save(_In_ WORKS* _lsWork, _In_ wchar_t *_path);
	static WORKS* Load(_In_ wchar_t *_path);
};

#endif
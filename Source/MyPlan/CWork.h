#include <Windows.h>

#ifndef CWORK
#define CWORK

typedef class CWork
{
private:
	WCHAR *m_lpwName;
	__int8 m_iPriority;
	__int8 m_iTimePer;

	CWork();

	/*
	Tên Function: Register
	Chức năng: đăng ký số phần trăm sử dụng
	Input: số phần trăm muốn đăng ký dùng
	Output:
	+ TRUE: nếu đăng ký thành công (số phần trăm đăng ký <= số phần trăm còn lại, lấy từ NumClearPercent)
	+ FALSE: nếu đăng ký thất bại (khi không thỏa TRUE hoặc phần trăm muốn đăng ký < 0)
	*/
	static BOOL Register(_In_ __int8 _per);
public:
	WCHAR* NAME();
	__int8 PRIORITY();
	__int8 TIMEPERCENT();

	/*
	Tên Function: TerminatePercent
	Chức năng: hủy số phần trăm đã đăng ký
	Input: số phần trăm muốn hủy
	Output: none
	*/
	static void Terminate(_In_ __int8 _per);

	/*
	Tên Function: Init
	Chức năng: khởi tạo đối tượng Work với các thông tin truyền vào
	Input:
	+ lpwName: tên công việc
	+ iPriority: độ ưu tiên thực hiện công việc
	+ iTimePercent: phần trăm thời lượng công việc sẽ chiếm trong tổng thời gian
	Output: đối tượng Work vừa được tạo.
	*/
	static CWork* Init(_In_ WCHAR* _lpwName, _In_ __int8 _iPriority, _In_ __int8 _iTimePer);
	~CWork();
} WORK;

typedef CWork* PWORK;
#define InitWork(lpwName, iPriority, iTimePer) CWork::Init(lpwName, iPriority, iTimePer)

#endif //CWORK

#include <windows.h>
#include <conio.h>
#define WMTRAY_QUERY_VIEW           (WM_USER + 236) 

// Yes I know there is a com interface for this
// But CoCreateInstance is failing and I do not have
// the time or will to figgure out why, this will do for now

class TaskbarList
{
public:
	void Init() { _wm_shellhook = RegisterWindowMessage(TEXT("SHELLHOOK")); }
	
	void AddTab(HWND hwnd);
	void DeleteTab(HWND hwnd);
private:
	HWND _HwndGetTaskbarList();
	HWND _hwndTaskbarList;
	int _wm_shellhook;
};

HWND TaskbarList::_HwndGetTaskbarList()
{
	if (_hwndTaskbarList && IsWindow(_hwndTaskbarList))
		return _hwndTaskbarList;
	_hwndTaskbarList = FindWindowA("Shell_TrayWnd", NULL);
	if (_hwndTaskbarList)
		_hwndTaskbarList = (HWND)SendMessage(_hwndTaskbarList, WMTRAY_QUERY_VIEW, 0, 0);
	return _hwndTaskbarList;	
}

void TaskbarList::AddTab(HWND hwnd)
{
	HWND hwndTL = _HwndGetTaskbarList();
	if (hwndTL) SendMessage(hwndTL, _wm_shellhook,
		HSHELL_WINDOWCREATED, (LPARAM) hwnd);
}

void TaskbarList::DeleteTab(HWND hwnd)
{
	HWND hwndTL = _HwndGetTaskbarList();
	if (hwndTL) SendMessage(hwndTL, _wm_shellhook,
		HSHELL_WINDOWDESTROYED, (LPARAM) hwnd);
}

static TaskbarList taskList;

void TaskBar_Init(void)
{
	taskList.Init();
}

void TaskBar_Swap( HWND hPrevWnd, HWND hNextWnd)
{
	taskList.DeleteTab(hPrevWnd);
	taskList.AddTab(hNextWnd);
}

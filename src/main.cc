#include <stdshit.h>
#include <conio.h>
#include "apihook.h"
#include "tabbar.h"

static void* RegisterClassWrapW;
__stdcall int RegisterClassWrapW_hook(WNDCLASSW* pClss)
{
	if(!tabbar_prevProc) { tabbar_prevProc = pClss->lpfnWndProc; }
	pClss->lpfnWndProc = tabbar_hookProc;
	return HOTCALL(RegisterClassWrapW_hook, RegisterClassWrapW)(pClss);
};

void hook_init(void)
{	
	AllocConsole();
	
	importDiscr id;
	id.fromName("browseui.dll");
	id.setFunc("shlwapi.dll", (cch*)131, 
		&RegisterClassWrapW_hook, &RegisterClassWrapW);
}


extern "C"
BOOL WINAPI DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
		hook_init(); tabbar_init();
	}
	
	
	
	
	
	
	
	//SHOpenFolderWindow

	return TRUE;
}

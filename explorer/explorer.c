#include <windows.h>

void RawEntryPoint(void);
void HookEntryPoint(void)
{
	LoadLibraryA("libtest.dll");
	RawEntryPoint();
}

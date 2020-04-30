setlocal
call exmod.bat
gcc %CFLAGS% explorer.c -c
exe_mod explorer.exe ..\bin\explorer.exe explorer.o
endlocal

setlocal
call exmod.bat x64
gcc %CFLAGS% explorer.c -c
exe_mod explorer_x64.exe ..\bin\explorer_x64.exe explorer.o
endlocal

SET CFLAGS=-std=c11 -fPIC -D_POSIX_C_SOURCE=199309L -pthread -O3 -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -DTHREADS=16 -DPORTABLE

:: Build the core library and the shell.
:: Requires WinLibs - https://winlibs.com/

gcc %CFLAGS% -c src/nissy.c
gcc %CFLAGS% nissy.o shell/shell.c -o run

:: Currently, building the Python module is not supported.
:: There are probably some issues related to using GCC + MINGW instead
:: of a more native Windows compiler.

:: Python libraries - change to mathc your local installation
::SET PYPATH=%userprofile%\AppData\Local\Programs\Python\Python313
::SET PYINCLUDE=%PYPATH%\include
::SET PYDLL=%PYPATH%\python313.dll

::gcc %CFLAGS% -I%PYINCLUDE% -shared %PYDLL% python/nissy_module.c nissy.o -o nissy_python_module.pyd

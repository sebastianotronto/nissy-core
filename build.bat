SET COMPILER=clang

SET ARCH=
SET WARNINGS=-Wno-deprecated-declarations
SET VARIABLES=-DTHREADS=1 -DPORTABLE
SET OFLAGS=-O3
SET DFLAGS=-g -fsanitize=address
SET CFLAGS=-std=c11 %OFLAGS% %ARCH% %WARNINGS% %VARIABLES%

SET STACKSIZE=-Wl,-stack:16777216
SET LFLAGS=%STACKSIZE%

%COMPILER% %CFLAGS% -c src\nissy.c || exit /b
%COMPILER% %CFLAGS% %LFLAGS% nissy.o shell\shell.c -o run.exe || exit /b

:: Python libraries - change to mathc your local installation
SET PYPATH=%userprofile%\AppData\Local\Programs\Python\Python313
SET PYINCLUDE=%PYPATH%\include
SET PYLIBS=%PYPATH%\libs

%COMPILER% %CFLAGS% -I%PYINCLUDE% -L%PYLIBS% -shared -lpython3 python/nissy_module.c nissy.o -o nissy.pyd
@echo off

if [%1]==[/d] (
    SET DEBUG=1
    shift
) else (
    SET DEBUG=0
)

SET CC=clang
SET CXX=clang++
SET ARCH=PORTABLE
SET THREADS=1
SET SANITIZE=

:: TODO depends on ARCH variable
SET ARCHOPTS=
:: TODO depends on SANITIZE variable
SET DFLAGS=-g3 -DDEBUG

SET WARNINGS=-Wno-deprecated-declarations
SET VARIABLES=-DTHREADS=%THREADS% -D%ARCH%
SET OFLAGS=-O3

SET CFLAGS=-std=c11 %ARCHOPTS% %WARNINGS% %VARIABLES%

SET STACKSIZE=-Wl,-stack:16777216
SET LFLAGS=%STACKSIZE%

:: Python libraries - change to match your local installation
SET PYPATH=%userprofile%\AppData\Local\Programs\Python\Python313
SET PYINCLUDE=%PYPATH%\include
SET PYLIBS=%PYPATH%\libs

SET TARGET=%1
if not defined TARGET SET TARGET=nissy
SET EXPR=%2
for %%a in (nissy python shell test config help clean tool cpp solvetest) do (
    if %TARGET%==%%a (
        call:build_%TARGET% %*
        exit /b
    )
)

echo Target '%TARGET%' unavailable, run 'build help' for info
exit /b 1

:build_help
    echo *** Warning: support for building nissy on Windows is incomplete ***
    echo.
    echo Build system for nissy. Usage:
    echo.
    echo build [/d] [TARGET]
    echo.
    echo Possible values for TARGET (defaults to 'nissy' if unspecified):
    echo.
    echo nissy       Build the nissy.o object file.
    echo python      Build the Python module for nissy.
    echo             NOTE: Python development headers must be installed in
    echo                   order to build the Python module. The path of these
    echo                   headers is hardcoded in the PYPATH varible in
    echo                   build.bat. You may need to edit this file.
    echo shell       Build the basic nissy shell (./run).
    echo cpp FILE    Build and run the given C++ FILE.
    echo test [EXPR] Build and run unit tests. If EXPR is provided, only the
    echo             tests whose name contains EXPR are run. The /d option is
    echo             is always implied.
    echo tool EXPR   Run the 'tool' matching the given EXPR.
    echo solvetest   Build nissy and run a collection of tools for testing"
    echo             various solvers.
    echo help        Show this help message.
    echo config      Show build configuration and exit.
    echo clean       Remove all build files.
    echo.
    echo The /d option activates debug mode (slower, used for testing).
    echo Tests are automatically built in debug mode even without /d.
    echo For more on build configurations, see the comments in build.bat.
exit /b

:build_config
    echo Compiler: %CC%
    echo Architecture: %ARCH%
    echo Max threads: %THREADS%
    echo Optimization flags: %OFLAGS%
    echo Debug flags: %DFLAGS%
exit /b

:build_nissy
    call:odflags
    @echo on
    %CC% %CFLAGS% %ODFLAGS% -c src\nissy.c || exit /b 1
    @echo off
exit /b

:build_shell
    call:build_nissy || exit /b 1
    call:odflags
    @echo on
    %CC% %CFLAGS% %ODFLAGS% %LFLAGS% nissy.o shell\shell.c -o run.exe ^
        || exit /b 1
    @echo off
exit /b

:build_python
    call:build_nissy || exit /b 1
    call:odflags
    @echo on
    %CC% %CFLAGS% %LFLAGS% -I%PYINCLUDE% -L%PYLIBS% -shared -lpython3 ^
        python\nissy_module.c nissy.o -o python\nissy.pyd || exit /b 1
    @echo off
exit /b

:build_test
    SET DEBUG=1
    call:build_nissy || exit /b 1
    if not defined EXPR (
        SET WILDCARD=*
    ) else (
        SET WILDCARD=*%EXPR%*
    )
    for /d %%d in ( test\%WILDCARD% ) do (
        if exist %%d\* (
            call:build_single_test %%d || exit /b 1
        )
    )
exit /b

:build_clean
    @echo on
    del *.o *.so *.a *.ilk *.pdb *.exe
    @echo off
exit /b

:build_tool
    if not defined EXPR (
        @echo Please provide a valid EXPR to select a tool
        exit /b 1
    )
    
    set toolname=
    for /d %%d in ( tools\*%EXPR%* ) do (
        set toolname=%%~nd
    )

    if [%toolname%]==[] (
        @echo Expression '%EXPR%' does not match any tool
        exit /b 1
    )
    
    call:build_nissy || exit /b 1
    call:build_single_tool %*
exit /b

:build_cpp
    if not defined EXPR (
        @echo Please provide a valid C++ source file.
        exit /b 1
    )
    if not exist %EXPR% (
        @echo File %EXPR% does not exist.
        exit /b 1
    )
    call:build_nissy || exit /b 1
    @echo on
    %CXX% %ODFLAGS% -std=c++20 -o runcpp.exe cpp\nissy.cpp nissy.o %EXPR% ^
        || exit /b 1
    runcpp
    @echo off
exit /b

:build_solvetest
    call:build_nissy || exit /b 1
    for /d %%d in ( tools\*solvetest* ) do (
        set toolname=%%~nd
        call:build_single_tool
    )
exit /b

:build_single_test
    call:odflags
    %CC% %CFLAGS% %ODFLAGS% %LFLAGS% nissy.o %1\*.c -o runtest.exe || exit /b 1
    set error=0
    for %%c in ( %1\*.in ) do (
        if %error%==1 exit /b 1
        @echo | set /p v="%1\%%~nc: "
        runtest < %%c > test\last.out 2> test\last.err
        FC /a %1\%%~nc.out test\last.out > test\last.fc.out
        if ERRORLEVEL 1 (
            @echo Test failed! Different output:
            type test\last.fc.out
            @echo stderr:
            type test\last.err
            goto :error
        ) else (
            @echo ok
        )
    )
    if %error%==1 exit /b 1
    del runtest.*
exit /b

:build_single_tool
    @echo on
    %CC% %CFLAGS% %ODFLAGS% %LFLAGS% nissy.o tools\%toolname%\*.c ^
        -o runtool.exe || exit /b 1
    @echo off
    runtool %3 %4 %5 %6 %7 %8 %9 || exit /b 1
    @echo.
    @echo (On Windows, the output of a tool is not saved to any file.)
exit /b

:odflags
    if %DEBUG%==1 (
        SET ODFLAGS=%DFLAGS%
    ) else (
        SET ODFLAGS=%OFLAGS%
    )
exit /b

:error
    exit /b 1
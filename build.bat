@echo off

if [%1]==[/d] (
    SET DEBUG=1
    shift
) else (
    SET DEBUG=0
)

SET CC=clang
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
for %%a in (nissy python shell test config help) do (
    if %TARGET%==%%a (
        call:build_%TARGET%
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
    echo shell       Build the basic nissy shell (./run).
    echo help        Show this help message.
    echo config      Show build configuration and exit.
    echo test [EXPR] Build and run unit tests. If EXPR is provided, only the
    echo             tests whose name contains EXPR are run. The /d option is
    echo             is always implied.
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

:odflags
    if %DEBUG%==1 (
        SET ODFLAGS=%DFLAGS%
    ) else (
        SET ODFLAGS=%OFLAGS%
    )
exit /b

:error
    exit /b 1
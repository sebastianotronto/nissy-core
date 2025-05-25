# The engine of Nissy, including the H48 optimal solver

**Warning**: this library is work in progress, breaking changes can
happen without notice.

This repository contains the core functionality for future versions of
[nissy](https://nissy.tronto.net).

H48 is an experimental Rubik's cube solver. It is heavily inspired by
[nxopt](https://github.com/rokicki/cube20src/blob/master/nxopt.md) and
[vcube](https://github.com/Voltara/vcube), but it uses a different set
of pruning tables that were developed independently.

## Building

This project currently supports only POSIX systems (Linux, MacOS, BSD...).
If you want to build on Windows you can do so via
[WSL](https://learn.microsoft.com/en-us/windows/wsl/install). Note that
the resulting executable will not be a native Windows executable.
Windows as a build target will be available in the future.

To build nissy simply run

```
$ ./build
```

For a list of options and targets for the build system run `./build help`.
Some compiler settings can be overridden using environment variables,
as explained in the comments at the beginning of the build script. For
example, the command:

```
$ export NISSY_BUILD_THREADS=3
$ CC=gcc ./build
```

is going to configure `nissy` to use at most 3 threads, and build it with
`gcc`. Se the comments in `./build` for more details.

## Running tests

This project includes a suite of "unit" test. You can run them with:

```
$ ./build test
```

For running the tests for the WebAssembly build with nodejs you can use:

```
$ ./build webtest
```

To run only a subset of the tests, you can pass as argument a regular
expression that matches only the name of the tests you want to run:

```
$ ./buld test coord
```

Each subfolder of the test folder contains a test. A test can consist
of multiple test cases (.in files). Running a test means compiling and
running the corresponding test against each test case. When a test case
is run, the .in file is read a the output of the program is compared
to the corresponding .out filei using diff(1). If the two differ, the
difference is printed out and no other test is run.

The results of the last test case run is saved in test/last.out (standard
output, the results compared with the .out files) and test/last.err
(standard error).

Tests are always run in debug mode: this means that optimizations are
disabled and some extra logging is enabled.

## Running "tools"

In the tools folder there are some small programs that test various
functionality of the H48 library. They work similarly to test, but they
are not run in debug mode by default.

To run a tool you can use:

```
$ ./build tool TOOLNAME PARAMETERS...
```

Where `TOOLNAME` is the name of one of the tools, or a regular expression
that matches one, and `PARAMETERS...` is a tool-specific list of
parameters.

For example:

```
$ :./build tool gendata h48h2k2
```

Will run a tool that generates the data table for the H48 solver with
parameters `h=2` and `k=2.

Each tool run is automatically timed, so these tools can be used as
benchmark.  The output as well as the time of the run are saved to a
file in the tools/results folder.

To build and run a tool in debug mode, use `./build -d tool`.

## Command-line interface

The `shell` folder contains the code for a rudimentary shell that can
be used to run commands manually. The user experience in not amazing,
as the commands require quite verbose options.

To build the shell run:

```
$ ./build shell
```

This will create an executable called `run`.  Then you can for example
get a cube from a sequence of moves:

```
$ ./run frommoves -moves "R' U' F"
JLQWSVUH=ZLCUABGIVTKH=A
```

The cube format is meant to be easy to copy-paste and read for the
software, but not necessarily intuitive for the user. See below for a
detaileed description.

You can also get a random cube

```
$ ./run randomcube
WDSQREVX=VBKYDUCJXWAb=A
```

To solve a cube you can use:

```
$ ./run solve -solver h48h0k4 -n 1 -M 4 -cube "JLQWSVUH=ZLCUABGIVTKH=A"
F' U' R
```

Or alternatively:

```
$ ./run solve_scramble -solver h48h0k4 -n 1 -M 4 -moves "R' U' F"
F' U' R
```

For a full list of available command, use `./run help`.

## Using this software as a library

This tool has been developed as a library, so it can be easily included
in other programs. For this reason, some bindings for languages other
than C are available.

The API is documented in the public header file `src/nissy.h`.

### C

To use this in a C project, simly include `src/nissy.h`. The tools
in the `tools/` folder are a good example for how to use this.

NOTE: this project is developed using the C11 standard. If you are using
an older version of C, you can write your own header file.

### C++

The `cpp` folder contains a C++ header `nissy.h` (C++20 standard) and an
implementation file `nissy.cpp`. This interface wraps the calls to the
C functions in an object-oriented C++ interface for more convenient use.

The `cpp/examples` folder contains some examples for how to use this
interface. You can build them and run them with the build tool, for
example:

```
./build cpp cpp/examples/solve_h48h3k2.cpp
```

NOTE: If you prefer to use a C-style API, you'll have to write
your own header, similar to the `extern "C" {}` block at the top of
`cpp/nissy.cpp`. The C header `src/nissy.h` cannot be compiled as C++,
as it uses features of C that are not compatible with it.

### Python

The `python` folder contains a Python module. The API provided by
this module follows the C API quite closely, except its functions
sometimes return strings instead of writing to `char *` buffers.

To build the Python module you need the Python development headers
installed. You can check this from the output of `./build config`:

```
$ ./build config
...
Python bindings: version 3.13
```

Then to build the module:

```
$ ./build python
```

And to import it

```
$ python                                  # In the main folder
>>> import nissy_python_module as nissy   # In the python shell
```

From here you can call the library functions directly, for example:

```
>>> nissy.applymoves('ABCDEFGH=ABCDEFGHIJKL=A', "R U R' U'")
'WFCDERQH=AECDIFGHBJKL=A'
```

The `python/examples` folder contains some examples, that you
can run for example with:

```
$ python python/examples/solve.py
```

You can access the documentation for the Python module from within
a Python interpreter with `help(nissy)`. Cross-check this documentation
with the comments in nissy.h for more details.

NOTE: Support for the Python module is still rudimentary.

### JavaScript (and WebAssembly)

Bindings for JavaScript via a WebAssembly build (using
[emscripten](https://emscripten.org)) are work in progress.
The JavaScript module can be built with

```
$ ./build web
```

An example web app running nissy can be found in the `web/http` folder.
You can run a web server in that folder to check it out, but you need
to set some extra headers to make it work. For example, if you are using
[darkhttpd](https://github.com/emikulic/darkhttpd) you can start the server
with the following command:

```
$ darkhttpd web/http/ \
  --header 'Cross-Origin-Opener-Policy: same-origin' \
  --header 'Cross-Origin-Embedder-Policy: require-corp' \
  --mimetypes web/http/mime
```

## Cube format

This format is a "base 32" encoding of the cube. It is not meant to be
human-readable, but it is compact while still being plain text. Each
piece, including the orientation value, is encoded as a number from 0
to 31, and this number is then converted to an uppercase letter (0-26)
or to a lowercase letter (27-31).

The format looks like this:

```
cccccccc=eeeeeeeeeeee=r
```

Where the first 8 characters represent the corner, the 12 characters
after the first 12 represent the edges and the last character represents
the orientation of the cube with respect to the base orientation.

Edges are numbered as follows (see also constants.h):

```
UF=0 UB=1 DB=2 DF=3 UR=4 UL=5 DL=6 DR=7 FR=8 FL=9 BL=10 BR=11
```

If the edge is misoriented with respect to the F/B axis, the value
is increased by 16.

Corners are numbered as follows:

```
UFR=0 UBL=1 DFL=2 DBR=3 UFL=4 UBR=5 DFR=6 DBL=7
```

If the corner is twisted clockwise with respect to the U/D
axis orientation, the value is increased by 8. If it is twisted
counter-clowsie, the value is increased by 16.

The orientations of the cube are numbered as follows, where e.g. "RB"
means that the R face in on top and the B face is on front:

```
UF=0  UR=1  UB=2  UL=3
DF=4  DR=5  DB=6  DL=7
RF=8  RD=9  RB=10 RU=11
LF=12 LD=13 LB=14 LU=15
FD=16 FR=17 FU=18 FL=19
BD=20 BR=21 BU=22 BL=23
```

In this format, the solved cube looks like this:

```
ABCDEFGH=ABCDEFGHIJKL=A
```

The cube after the move Fw looks like this:

```
ANCPETGR=AbaDEFGHIJRS=L
```

A cube in B32 format is always 23 characters long (24 if the terminating
null character is included).

# Benchmarks for the H48 optimal solver and comparison with vcube

This page contains some benchmarks for some of the possible
configurations of the H48 optimal solver. For comparison we also
include similar measurements for Andrew Skalski's impressive optimal
solver [vcube](https://github.com/Voltara/vcube), which has been taken
as a benchmark reference throughout the development of H48. The two
solvers differ both in method (although they both use an A* search)
and implementation.

Similar benchmarks can be found:

* In Chen Shuang's
  [dedicated github page](https://github.com/cs0x7f/cube_solver_test/wiki),
  which includes many more solvers. At the time of writing, the version
  of H48 used for these benchmarks is a rather old one. Note that
  cube48opt is a re-implementation of the H48 solver by Chen Shuang.
* In Enrico Tenuti's [thesis](https://github.com/enricotenuti/h48thesis),
  also using an old implementation of H48, but including more data and
  nice plots.

## Setting

*All movecounts are in Half Turn Metric*

For the benchmarks we used some sets of random positions that can be found
in `benchmarks/scrambles`. We divide them by optimal solution length,
because the time to solve a scramble grows exponentially on the number of
moves, so mixing up positions with long and short solutions is going to
make the shorter solution almost irrelevant. Even within the same solution
length the time to solve a random position can vary significantly,
so we have used sets of 25 scrambles and taken the average of those,
except for the [Superflip](https://en.wikipedia.org/wiki/Superflip),
which is a single scramble.

For short solutions, the very low numbers we get with the largest solver
may be particularly inaccurate, because writing the solution (together
with some log messages) to standard output may take a significant portion
of the time, depending on the terminal emulator used and other factors.

The main test we performed was finding a single optimal solution, and we
compared the results with vcube. This test was run in a single-thread
configuration and in two multithread configurations (with 4 and 16
threads).  We also ran a test on finding *all* optimal solutions which,
as far as I know, is a use case not supported by vcube; this latter test
was only run on 16 threads.

Since the size of the pruning table used by the solver is of utmost
importance, we include two statistics: time per cube and time per cube
*adjusted by table size*.  The adjustment we took simply consists in
multiplying the time per cube by the size of the table. Empirically
this leads to more even results across the board, as the speed of the
solvers of the same family seems to scale linearly with the size of the
pruning table.

All benchmark tests were done on the following configuration:

* CPU: AMD Ryzen 7 7700 (8 cores, 16 virtual threads)
* Memory: 2x Corsair Vengeance 32GB 5600MHz
* Motherboard: Gigabyte B650M K
* Operating system: Debian 12 (Linux kernel 6.1.0)
* Compiler: GCC 12.2.0 for H48 and Clang 14.0.6 for vcube

## Single solution

Average time for finding a single optimal solution.

### Single thread

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11 k2|56.1GiB |  0.23  |  1.15  |  5.08  | 31.30  | 278.47  |
|vcube 404 |31.8GiB |  0.30  |  1.25  |  6.87  | 57.49  | 291.31  |
|H48 h10 k2|28.1GiB |  0.34  |  1.80  |  7.77  |        |         |
|vcube 308 |21.2GiB |  0.20  |  1.11  |  6.92  |        |         |
|H48 h9 k2 |14.1GiB |  0.42  |  2.84  | 12.86  |        |         |
|vcube 208 | 7.3GiB |  0.57  |  4.41  | 20.75  |        |         |
|H48 h8 k2 | 7.1GiB |  0.86  |  6.66  | 27.40  |        |         |
|H48 h7 k2 | 3.6GiB |  1.47  |  8.90  | 42.46  |        |         |
|vcube 112 | 2.4GiB |  1.01  |  9.39  |        |        |         |
|H48 h6 k2 | 1.8GiB |  2.28  | 16.89  |        |        |         |

Time per cube adjusted for tables size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11 k2|56.1GiB | 12.90  | 64.51  | 284.99 |1755.93 |15622.17 |
|vcube 404 |31.8GiB |  9.54  | 39.75  | 218.47 |1828.18 | 9263.66 |
|H48 h10 k2|28.1GiB |  9.55  | 50.58  | 218.34 |        |         |
|vcube 308 |21.2GiB |  4.24  | 23.53  | 146.70 |        |         |
|H48 h9 k2 |14.1GiB |  5.92  | 40.04  | 181.33 |        |         |
|vcube 208 | 7.3GiB |  4.16  | 32.19  | 151.48 |        |         |
|H48 h8 k2 | 7.1GiB |  6.11  | 47.29  | 194.54 |        |         |
|H48 h7 k2 | 3.6GiB |  5.29  | 32.04  | 152.86 |        |         |
|vcube 112 | 2.4GiB |  2.42  | 22.53  |        |        |         |
|H48 h6 k2 | 1.8GiB |  4.10  | 30.40  |        |        |         |

### Multithread (4 threads)

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11 k2|56.1GiB |  0.06  |  0.31  |  1.31  |  7.96  |  77.81  |
|vcube 404 |31.8GiB |  0.10  |  0.38  |  1.88  | 16.98  |   (a)   |
|H48 h10 k2|28.1GiB |  0.10  |  0.47  |  2.00  | 13.54  | 114.35  |
|vcube 308 |21.2GiB |  0.06  |  0.42  |  1.95  | 17.73  |   (a)   |
|H48 h9 k2 |14.1GiB |  0.14  |  0.83  |  3.82  | 25.98  | 162.72  |
|vcube 208 | 7.3GiB |  0.17  |  1.49  |  5.88  |        |   (a)   |
|H48 h8 k2 | 7.1GiB |  0.27  |  2.02  |  7.94  |        |         |
|H48 h7 k2 | 3.6GiB |  0.35  |  2.59  | 12.41  |        |         |
|vcube 112 | 2.4GiB |  0.29  |  3.15  | 12.06  |        |   (a)   |
|H48 h6 k2 | 1.8GiB |  0.65  |  4.79  | 23.91  |        |         |

Time per cube adjusted for tables size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11 k2|56.1GiB |  3.37  | 17.39  | 73.49  | 446.56 | 4365.92 |
|vcube 404 |31.8GiB |  3.80  | 12.08  | 59.78  | 539.96 |   (a)   |
|H48 h10 k2|28.1GiB |  2.81  | 13.21  | 56.20  | 380.47 | 3213.24 |
|vcube 308 |21.2GiB |  1.27  |  8.90  | 41.34  | 375.88 |   (a)   |
|H48 h9 k2 |14.1GiB |  1.97  | 11.70  | 53.86  | 366.32 | 2294.35 |
|vcube 208 | 7.3GiB |  1.24  | 10.88  | 42.92  |        |   (a)   |
|H48 h8 k2 | 7.1GiB |  1.92  | 14.34  | 56.37  |        |         |
|H48 h7 k2 | 3.6GiB |  1.26  |  9.32  | 44.68  |        |         |
|vcube 112 | 2.4GiB |  0.70  |  7.56  | 28.94  |        |   (a)   |
|H48 h6 k2 | 1.8GiB |  1.17  |  8.62  | 43.04  |        |         |

(a) vcube cannot parallelize on a single scramble, the results for the
Superflip are going to be the same as in the single thread case.

### Multithread (16 threads)

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11 k2|56.1GiB |  0.02  |  0.10  |  0.43  |  2.48  |  26.29  |
|vcube 404 |31.8GiB |  0.03  |  0.16  |  0.67  |  6.36  |   (a)   |
|H48 h10 k2|28.1GiB |  0.03  |  0.16  |  0.74  |  4.43  |         |
|vcube 308 |21.2GiB |  0.04  |  0.22  |  0.89  |  9.53  |   (a)   |
|H48 h9 k2 |14.1GiB |  0.04  |  0.26  |  1.18  |  8.31  |         |
|vcube 208 | 7.3GiB |  0.08  |  0.80  |  2.38  |        |   (a)   |
|H48 h8 k2 | 7.1GiB |  0.08  |  0.60  |  2.48  |        |         |
|H48 h7 k2 | 3.6GiB |  0.11  |  0.81  |  3.91  |        |         |
|vcube 112 | 2.4GiB |  0.15  |  1.66  |  5.18  |        |   (a)   |
|H48 h6 k2 | 1.8GiB |  0.21  |  1.53  |  7.82  |        |         |

Time per cube adjusted for tables size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11 k2|56.1GiB |  1.12  |  5.61  | 24.12  | 139.13 | 1474.87 |
|vcube 404 |31.8GiB |  1.08  |  5.09  | 21.31  | 202.25 |   (a)   |
|H48 h10 k2|28.1GiB |  0.84  |  4.50  | 20.79  | 124.48 |         |
|vcube 308 |21.2GiB |  0.85  |  4.66  | 18.87  | 202.04 |   (a)   |
|H48 h9 k2 |14.1GiB |  0.56  |  3.67  | 16.64  | 117.17 |         |
|vcube 208 | 7.3GiB |  0.58  |  5.84  | 17.37  |        |   (a)   |
|H48 h8 k2 | 7.1GiB |  0.57  |  4.26  | 17.60  |        |         |
|H48 h7 k2 | 3.6GiB |  0.40  |  2.92  | 14.07  |        |         |
|vcube 112 | 2.4GiB |  0.36  |  3.98  | 12.43  |        |   (a)   |
|H48 h6 k2 | 1.8GiB |  0.38  |  2.75  | 14.08  |        |         |

(a) vcube cannot parallelize on a single scramble, the results for the
Superflip are going to be the same as in the single thread case.

## All optimal solutions

Average time for finding all optimal solutions.

### Multithread (16 threads)

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|
|:---------|:-------|-------:|-------:|-------:|-------:|
|H48 h11 k2|56.1GiB |  0.05  |  0.50  |  4.24  | 19.75  |
|H48 h10 k2|28.1GiB |  0.08  |  0.88  |  6.94  |        |
|H48 h9 k2 |14.1GiB |  0.13  |  1.39  | 13.50  |        |
|H48 h8 k2 | 7.1GiB |  0.25  |  2.85  |        |        |
|H48 h7 k2 | 3.6GiB |  0.36  |  4.24  |        |        |
|H48 h6 k2 | 1.8GiB |  0.69  |  8.20  |        |        |

Time per cube adjusted for tables size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|
|:---------|:-------|-------:|-------:|-------:|-------:|
|H48 h11 k2|56.1GiB |  2.81  | 28.05  | 237.86 |1107.98 |
|H48 h10 k2|28.1GiB |  2.25  | 24.73  | 195.01 |        |
|H48 h9 k2 |14.1GiB |  1.83  | 19.60  | 190.35 |        |
|H48 h8 k2 | 7.1GiB |  1.77  | 20.24  |        |        |
|H48 h7 k2 | 3.6GiB |  1.30  | 15.26  |        |        |
|H48 h6 k2 | 1.8GiB |  1.24  | 14.76  |        |        |

## Other notes

* Missing values in the table mean that the test is very slow and I did
  not want to wait for it to finish. I may add these values in the future.
* All the measurements above exclude the time needed to load the pruning
  tables into memory, which can be quite significant for large tables. To
  repeat these measurements, one can use the tool `301_solve_file`. For
  example:

```
./build tool solve_file h48h7k2 ./benchmarks/scrambles/scrambles-16.txt
```

  To find all solutions, add something like `99999 0` at the end of the
  command. This will tell the tool to find up to `99999` solutions that
  are at most `0` moves longer than optimal.
* The measurements also excluded the one-off computation of the pruning
  tables which, for reasons related to the cube coordinates used, is
  significantly slower for H48 compared to vcube.
* H48's and vcube's approach to multithreading are extremely different:
  H48 parallelize the search for each cube individually, vcube solves
  multiple cubes in parallel by dedicating a single thread to each of
  them. Both apporaches have pros and cons: vcube's approch has less
  overhead in coordination between the threads, but often some threads
  may be left without work when there are no more cubes left to solve.
* Per-cube parallelization means that H48 will always be faster than
  vcube when solving a single cube.
* vcube only supports x86 processors (Intel, AMD), while H48 runs on any
  architecture, including e.g. ARM (Macbook M series, android phones)
  and can be compiled to WebAssembly as well.

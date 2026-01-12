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
* Operating system: Debian 13 (Linux kernel 6.12.57)
* Compiler: GCC 14.2.0 for H48 and Clang 19.1.7 for vcube

## Results (click on each item to expand)

<details><summary>Single solution, single thread</summary>

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.11|    0.75|    3.43|   27.28|    19.30|
|H48 h11   |56.5 GiB|    0.09|    0.50|    2.24|   15.73|    15.55|
|vcube 404 |31.8 GiB|    0.23|    1.24|    6.10|   59.33|   268.26|
|H48 h10   |28.3 GiB|    0.15|    0.76|    3.36|   23.51|    38.05|
|vcube 308 |21.2 GiB|    0.17|    1.02|    6.20|   58.70|   604.35|
|H48 h9    |14.1 GiB|    0.24|    1.48|    6.69|        |         |
|vcube 208 | 7.3 GiB|    0.56|    4.36|   20.58|        |         |
|H48 h8    | 7.1 GiB|    0.46|    3.36|   14.13|        |         |
|H48 h7    | 3.5 GiB|    0.63|    4.85|   23.25|        |         |
|vcube 112 | 2.4 GiB|    0.96|    9.29|   40.52|        |         |
|H48 h6    | 1.8 GiB|    1.25|    9.45|   46.31|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    6.43|   43.80|  199.38| 1587.43|  1122.73|
|H48 h11   |56.5 GiB|    5.21|   28.28|  126.68|  889.01|   878.77|
|vcube 404 |31.8 GiB|    7.40|   39.47|  194.01| 1887.94|  8535.87|
|H48 h10   |28.3 GiB|    4.22|   21.37|   94.86|  664.22|  1075.08|
|vcube 308 |21.2 GiB|    3.51|   21.71|  131.50| 1245.26| 12819.94|
|H48 h9    |14.1 GiB|    3.34|   20.97|   94.57|        |         |
|vcube 208 | 7.3 GiB|    4.08|   31.74|  149.68|        |         |
|H48 h8    | 7.1 GiB|    3.25|   23.75|   99.82|        |         |
|H48 h7    | 3.5 GiB|    2.22|   17.12|   82.14|        |         |
|vcube 112 | 2.4 GiB|    2.33|   22.53|   98.23|        |         |
|H48 h6    | 1.8 GiB|    2.21|   16.70|   81.86|        |         |

<img src="img/17moves1thread.png">
<img src="img/18moves1thread.png">
<img src="img/19moves1thread.png">
<img src="img/20moves1thread.png">
</details>

<details><summary>Single solution, 4 threads</summary>

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.03|    0.27|    1.04|    7.70|   (a)   |
|H48 h11   |56.5 GiB|    0.04|    0.14|    0.60|    3.97|     4.19|
|vcube 404 |31.8 GiB|    0.07|    0.30|    1.65|   16.17|   (a)   |
|H48 h10   |28.3 GiB|    0.05|    0.20|    0.91|    6.65|     9.81|
|vcube 308 |21.2 GiB|    0.05|    0.35|    1.78|   16.61|   (a)   |
|H48 h9    |14.1 GiB|    0.07|    0.39|    1.74|   12.54|    18.96|
|vcube 208 | 7.3 GiB|    0.16|    1.47|    5.86|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.13|    0.90|    3.71|        |         |
|H48 h7    | 3.5 GiB|    0.17|    1.28|    6.12|        |         |
|vcube 112 | 2.4 GiB|    0.29|    3.13|   11.95|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.33|    2.47|   12.22|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    2.03|   15.69|   60.25|  447.97|   (a)   |
|H48 h11   |56.5 GiB|    2.04|    8.17|   33.77|  224.59|   236.92|
|vcube 404 |31.8 GiB|    2.32|    9.50|   52.46|  514.50|   (a)   |
|H48 h10   |28.3 GiB|    1.37|    5.76|   25.83|  187.88|   277.18|
|vcube 308 |21.2 GiB|    1.02|    7.52|   37.82|  352.36|   (a)   |
|H48 h9    |14.1 GiB|    1.03|    5.45|   24.58|  177.17|   267.79|
|vcube 208 | 7.3 GiB|    1.18|   10.69|   42.63|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.92|    6.39|   26.20|        |         |
|H48 h7    | 3.5 GiB|    0.62|    4.53|   21.62|        |         |
|vcube 112 | 2.4 GiB|    0.69|    7.59|   28.97|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.59|    4.37|   21.61|        |         |

(a) vcube cannot parallelize on a single scramble, the results for the
superflip are going to be the same as in the single thread case.

<img src="img/17moves4threads.png">
<img src="img/18moves4threads.png">
<img src="img/19moves4threads.png">
<img src="img/20moves4threads.png">
</details>

<details><summary>Single solution, 16 threads</summary>

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.02|    0.13|    0.45|    2.84|   (a)   |
|H48 h11   |56.5 GiB|    0.02|    0.06|    0.22|    1.33|     1.84|
|vcube 404 |31.8 GiB|    0.04|    0.14|    0.65|    6.08|   (a)   |
|H48 h10   |28.3 GiB|    0.03|    0.08|    0.33|    2.34|     4.18|
|vcube 308 |21.2 GiB|    0.03|    0.19|    0.78|    6.67|   (a)   |
|H48 h9    |14.1 GiB|    0.04|    0.15|    0.64|    4.45|     8.09|
|vcube 208 | 7.3 GiB|    0.08|    0.79|    2.43|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.06|    0.34|    1.36|        |         |
|H48 h7    | 3.5 GiB|    0.07|    0.47|    2.20|        |         |
|vcube 112 | 2.4 GiB|    0.15|    1.63|    5.10|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.13|    0.91|    4.39|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.95|    7.83|   26.04|  165.03|   (a)   |
|H48 h11   |56.5 GiB|    1.34|    3.48|   12.19|   74.93|   103.98|
|vcube 404 |31.8 GiB|    1.21|    4.60|   20.76|  193.43|   (a)   |
|H48 h10   |28.3 GiB|    0.74|    2.37|    9.46|   66.13|   118.23|
|vcube 308 |21.2 GiB|    0.67|    4.01|   16.48|  141.49|   (a)   |
|H48 h9    |14.1 GiB|    0.51|    2.15|    8.98|   62.92|   114.31|
|vcube 208 | 7.3 GiB|    0.56|    5.78|   17.68|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.40|    2.40|    9.59|        |         |
|H48 h7    | 3.5 GiB|    0.26|    1.66|    7.77|        |         |
|vcube 112 | 2.4 GiB|    0.35|    3.95|   12.37|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.23|    1.61|    7.76|        |         |

(a) vcube cannot parallelize on a single scramble, the results for the
superflip are going to be the same as in the single thread case.

<img src="img/17moves16threads.png">
<img src="img/18moves16threads.png">
<img src="img/19moves16threads.png">
<img src="img/20moves16threads.png">
</details>

<details><summary>All solutions, 16 threads</summary>

*Note: vcube does not have an option for finding multiple solutions.*

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11   |56.5 GiB|    0.04|    0.26|    2.22|   18.96|    16.95|
|H48 h10   |28.3 GiB|    0.05|    0.42|    3.82|   34.42|    36.80|
|H48 h9    |14.1 GiB|    0.08|    0.73|    7.28|        |         |
|H48 h8    | 7.1 GiB|    0.15|    1.56|   15.41|        |         |
|H48 h7    | 3.5 GiB|    0.21|    2.38|   26.52|        |         |
|H48 h6    | 1.8 GiB|    0.39|    4.67|   53.00|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11   |56.5 GiB|    2.11|   14.55|  125.21| 1071.23|   957.47|
|H48 h10   |28.3 GiB|    1.48|   11.96|  107.83|  972.35|  1039.73|
|H48 h9    |14.1 GiB|    1.08|   10.29|  102.88|        |         |
|H48 h8    | 7.1 GiB|    1.03|   10.99|  108.87|        |         |
|H48 h7    | 3.5 GiB|    0.74|    8.41|   93.69|        |         |
|H48 h6    | 1.8 GiB|    0.70|    8.25|   93.68|        |         |

</details>

## Comments on the results

* Adjusting for table size, H48 is generally faster than vcube.
* The gap between the two solvers is larger for scrambles with
  longer optimal solutions.
* On sets of 25 scrambles, H48 performs better than vcube when using
  multiple threads, compared to their single-threaded performance.
  However, this advantage will likely disappear (or
  even invert) if we increase the size of the set.

## Other notes

* To repeat the benchmarks, use `./benchmarks/run-h48-benchmarks.sh`.
* All the measurements above exclude the time needed to load the pruning
  tables into memory, which can be quite significant for large tables.
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
* For H48, both GCC and Clang have been tried, with the same options;
  the resulting executable was about 10% faster with GCC compared to Clang.
  vcube only supports compiling with Clang.
* The performance of the H48 solver depends slightly, but measurably, on the
  alignment of the pruning table in memory. This is not handled by the core
  library, but by the program that uses it. For these tests, we have used as
  reference implementation the program in `tools/301_solve_file`, which
  ensures 64-byte alignment.

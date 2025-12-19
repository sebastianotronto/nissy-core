<details><summary>Single solution, single thread</summary>

Time per cube (in seconds, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.11|    0.75|    3.43|   27.28|    19.30|
|H48 h11   |56.5 GiB|    0.13|    0.73|    3.43|   23.28|    19.26|
|vcube 404 |31.8 GiB|    0.23|    1.24|    6.10|   59.33|   268.26|
|H48 h10   |28.3 GiB|    0.23|    1.20|    6.18|   43.51|    48.84|
|vcube 308 |21.2 GiB|    0.17|    1.02|    6.20|   58.70|   604.35|
|H48 h9    |14.1 GiB|    0.36|    2.47|   11.79|        |         |
|vcube 208 | 7.3 GiB|    0.56|    4.36|   20.58|        |         |
|H48 h8    | 7.1 GiB|    0.79|    6.09|   25.58|        |         |
|H48 h7    | 3.5 GiB|    1.07|    8.59|   42.16|        |         |
|vcube 112 | 2.4 GiB|    0.96|    9.29|   40.52|        |         |
|H48 h6    | 1.8 GiB|    2.12|   16.29|   82.24|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    6.43|   43.80|  199.38| 1587.43|  1122.73|
|H48 h11   |56.5 GiB|    7.33|   41.47|  193.67| 1315.32|  1088.34|
|vcube 404 |31.8 GiB|    7.40|   39.47|  194.01| 1887.94|  8535.87|
|H48 h10   |28.3 GiB|    6.60|   34.00|  174.65| 1229.18|  1379.90|
|vcube 308 |21.2 GiB|    3.51|   21.71|  131.50| 1245.26| 12819.94|
|H48 h9    |14.1 GiB|    5.03|   34.85|  166.51|        |         |
|vcube 208 | 7.3 GiB|    4.08|   31.74|  149.68|        |         |
|H48 h8    | 7.1 GiB|    5.61|   43.02|  180.71|        |         |
|H48 h7    | 3.5 GiB|    3.80|   30.34|  148.96|        |         |
|vcube 112 | 2.4 GiB|    2.33|   22.53|   98.23|        |         |
|H48 h6    | 1.8 GiB|    3.75|   28.79|  145.36|        |         |

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
|H48 h11   |56.5 GiB|    0.04|    0.22|    0.97|    6.98|     5.95|
|vcube 404 |31.8 GiB|    0.07|    0.30|    1.65|   16.17|   (a)   |
|H48 h10   |28.3 GiB|    0.07|    0.36|    1.79|   12.42|    14.03|
|vcube 308 |21.2 GiB|    0.05|    0.35|    1.78|   16.61|   (a)   |
|H48 h9    |14.1 GiB|    0.11|    0.70|    3.48|   25.61|    25.37|
|vcube 208 | 7.3 GiB|    0.16|    1.47|    5.86|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.23|    1.74|    7.19|        |         |
|H48 h7    | 3.5 GiB|    0.32|    2.45|   12.12|        |         |
|vcube 112 | 2.4 GiB|    0.29|    3.13|   11.95|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.61|    4.61|   24.22|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    2.03|   15.69|   60.25|  447.97|   (a)   |
|H48 h11   |56.5 GiB|    2.53|   12.71|   54.58|  394.33|   336.47|
|vcube 404 |31.8 GiB|    2.32|    9.50|   52.46|  514.50|   (a)   |
|H48 h10   |28.3 GiB|    2.11|   10.23|   50.61|  350.78|   396.48|
|vcube 308 |21.2 GiB|    1.02|    7.52|   37.82|  352.36|   (a)   |
|H48 h9    |14.1 GiB|    1.57|    9.96|   49.23|  361.85|   358.45|
|vcube 208 | 7.3 GiB|    1.18|   10.69|   42.63|        |   (a)   |
|H48 h8    | 7.1 GiB|    1.66|   12.27|   50.82|        |         |
|H48 h7    | 3.5 GiB|    1.11|    8.67|   42.83|        |         |
|vcube 112 | 2.4 GiB|    0.69|    7.59|   28.97|        |   (a)   |
|H48 h6    | 1.8 GiB|    1.08|    8.16|   42.81|        |         |

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
|H48 h11   |56.5 GiB|    0.02|    0.08|    0.33|    2.22|     2.41|
|vcube 404 |31.8 GiB|    0.04|    0.14|    0.65|    6.08|   (a)   |
|H48 h10   |28.3 GiB|    0.03|    0.13|    0.58|    4.21|     5.56|
|vcube 308 |21.2 GiB|    0.03|    0.19|    0.78|    6.67|   (a)   |
|H48 h9    |14.1 GiB|    0.04|    0.25|    1.10|    8.05|    10.77|
|vcube 208 | 7.3 GiB|    0.08|    0.79|    2.43|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.08|    0.57|    2.40|        |         |
|H48 h7    | 3.5 GiB|    0.11|    0.83|    4.00|        |         |
|vcube 112 | 2.4 GiB|    0.15|    1.63|    5.10|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.21|    1.51|    7.74|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.95|    7.83|   26.04|  165.03|   (a)   |
|H48 h11   |56.5 GiB|    1.12|    4.61|   18.90|  125.27|   136.10|
|vcube 404 |31.8 GiB|    1.21|    4.60|   20.76|  193.43|   (a)   |
|H48 h10   |28.3 GiB|    0.86|    3.61|   16.47|  118.85|   157.13|
|vcube 308 |21.2 GiB|    0.67|    4.01|   16.48|  141.49|   (a)   |
|H48 h9    |14.1 GiB|    0.60|    3.51|   15.57|  113.71|   152.21|
|vcube 208 | 7.3 GiB|    0.56|    5.78|   17.68|        |   (a)   |
|H48 h8    | 7.1 GiB|    0.59|    4.03|   16.98|        |         |
|H48 h7    | 3.5 GiB|    0.41|    2.94|   14.12|        |         |
|vcube 112 | 2.4 GiB|    0.35|    3.95|   12.37|        |   (a)   |
|H48 h6    | 1.8 GiB|    0.38|    2.67|   13.68|        |         |

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
|H48 h11   |56.5 GiB|    0.05|    0.41|    3.69|   31.65|    21.80|
|H48 h10   |28.3 GiB|    0.07|    0.70|    6.73|   58.61|    48.62|
|H48 h9    |14.1 GiB|    0.12|    1.28|   12.93|        |         |
|H48 h8    | 7.1 GiB|    0.25|    2.72|   27.42|        |         |
|H48 h7    | 3.5 GiB|    0.37|    4.24|   47.10|        |         |
|H48 h6    | 1.8 GiB|    0.69|    8.18|   91.85|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|  Solver  |  Size  |17 moves|18 moves|19 moves|20 moves|Superflip|
|:---------|:-------|-------:|-------:|-------:|-------:|--------:|
|H48 h11   |56.5 GiB|    2.57|   23.07|  208.71| 1788.12|  1231.86|
|H48 h10   |28.3 GiB|    2.01|   19.77|  190.14| 1655.95|  1373.79|
|H48 h9    |14.1 GiB|    1.72|   18.07|  182.67|        |         |
|H48 h8    | 7.1 GiB|    1.74|   19.21|  193.74|        |         |
|H48 h7    | 3.5 GiB|    1.29|   14.98|  166.41|        |         |
|H48 h6    | 1.8 GiB|    1.21|   14.47|  162.36|        |         |

</details>

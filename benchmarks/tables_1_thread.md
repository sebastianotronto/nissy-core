<!-- The following details block can be found in benchmarks/tables_1_thread.md -->
<details><summary>Results: Single solution, single thread</summary>

Time per cube (in seconds, lower is better).

|Solver|Size|17 moves|18 moves|19 moves|20 moves|Superflip|
|:-----|:---|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    0.11|    0.75|    3.43|   27.28|    19.30|
|egg h11   |58.0 GiB|    0.01|    0.06|    0.24|    2.16|     0.14|
|H48 h11   |56.5 GiB|    0.08|    0.43|    1.86|   12.20|    15.39|
|vcube 404 |31.8 GiB|    0.23|    1.24|    6.10|   59.33|   268.26|
|egg h10   |29.0 GiB|    0.02|    0.10|    0.47|    4.00|     0.23|
|H48 h10   |28.3 GiB|    0.13|    0.69|    3.37|   23.15|    38.00|
|vcube 308 |21.2 GiB|    0.17|    1.02|    6.20|   58.70|   604.35|
|egg h9    |14.5 GiB|    0.03|    0.19|    0.91|        |         |
|H48 h9    |14.1 GiB|    0.22|    1.43|    6.56|        |         |
|vcube 208 | 7.3 GiB|    0.56|    4.36|   20.58|        |         |
|egg h8    | 7.2 GiB|    0.07|    0.39|    1.82|        |         |
|H48 h8    | 7.1 GiB|    0.46|    3.43|   14.18|        |         |
|egg h7    | 3.6 GiB|    0.11|    0.81|    4.35|        |         |
|H48 h7    | 3.5 GiB|    0.63|    4.84|   23.31|        |         |
|vcube 112 | 2.4 GiB|    0.96|    9.29|   40.52|        |         |
|egg h6    | 1.8 GiB|    0.20|    1.53|    8.19|        |         |
|H48 h6    | 1.8 GiB|    1.26|    9.51|   47.14|        |         |

Time per cube adjusted for table size (in seconds \* GiB, lower is better).

|Solver|Size|17 moves|18 moves|19 moves|20 moves|Superflip|
|:-----|:---|-------:|-------:|-------:|-------:|--------:|
|vcube 212 |58.2 GiB|    6.43|   43.80|  199.38| 1587.43|  1122.73|
|egg h11   |58.0 GiB|    0.55|    3.31|   13.99|  125.17|     8.04|
|H48 h11   |56.5 GiB|    4.56|   24.55|  104.96|  689.22|   869.37|
|vcube 404 |31.8 GiB|    7.40|   39.47|  194.01| 1887.94|  8535.87|
|egg h10   |29.0 GiB|    0.50|    2.90|   13.69|  115.88|     6.80|
|H48 h10   |28.3 GiB|    3.62|   19.45|   95.15|  654.05|  1073.69|
|vcube 308 |21.2 GiB|    3.51|   21.71|  131.50| 1245.26| 12819.94|
|egg h9    |14.5 GiB|    0.47|    2.74|   13.19|        |         |
|H48 h9    |14.1 GiB|    3.11|   20.18|   92.68|        |         |
|vcube 208 | 7.3 GiB|    4.08|   31.74|  149.68|        |         |
|egg h8    | 7.2 GiB|    0.48|    2.82|   13.16|        |         |
|H48 h8    | 7.1 GiB|    3.23|   24.26|  100.15|        |         |
|egg h7    | 3.6 GiB|    0.39|    2.94|   15.77|        |         |
|H48 h7    | 3.5 GiB|    2.22|   17.12|   82.35|        |         |
|vcube 112 | 2.4 GiB|    2.33|   22.53|   98.23|        |         |
|egg h6    | 1.8 GiB|    0.36|    2.76|   14.83|        |         |
|H48 h6    | 1.8 GiB|    2.24|   16.80|   83.33|        |         |

<img src="img/17moves1thread.png">
<img src="img/18moves1thread.png">
<img src="img/19moves1thread.png">
<img src="img/20moves1thread.png">
</details>

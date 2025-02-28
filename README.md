# deltaFlow
Power System Analysis Tool

## Usage:
```shell
./bin/deltaFlow.pl 
```

```
2025-02-28 07:31:06 - INFO     - ---------------------------------*- MATLAB -*-----------------------------------
2025-02-28 07:31:06 - INFO     - 
2025-02-28 07:31:06 - INFO     -                                    /\\
2025-02-28 07:31:06 - INFO     -                                   /  \\
2025-02-28 07:31:06 - INFO     -                                  /    \\
2025-02-28 07:31:06 - INFO     -                                 /      \\
2025-02-28 07:31:06 - INFO     -                                 =========
2025-02-28 07:31:06 - INFO     - 
2025-02-28 07:31:06 - INFO     -                                 deltaFlow
2025-02-28 07:31:06 - INFO     -                         Power System Analysis Tool
2025-02-28 07:31:06 - INFO     -                            Copyright (c) 2025
2025-02-28 07:31:06 - INFO     - --------------------------------------------------------------------------------
2025-02-28 07:31:06 - INFO     - Choose the method for power flow solution:
2025-02-28 07:31:06 - INFO     - 1. Gauss-Seidel
2025-02-28 07:31:06 - INFO     - 2. Newton-Raphson
Enter your choice (1 for Gauss-Seidel, 2 for Newton-Raphson): >>1
Enter the maximum number of iterations (e.g., 1024): >>1024
Enter the tolerance (e.g., 1E-16): >>1E-16
Enter the path for the bus data file: >>../data/bus-data.csv
Enter the path for the line data file: >>../data/line-data.csv
2025-02-28 07:31:23 - INFO     - Starting Gauss-Seidel method ...
Enter the relaxation coefficient (0 < R_coeff < 2): >>1.6
2025-02-28 07:31:25 - DEBUG    - Over-relaxation enabled (R_coeff > 1), this will accelerate convergence.
2025-02-28 07:31:25 - DEBUG    - Gauss-Seidel converged after: 117 iterations
2025-02-28 07:31:25 - DEBUG    - Convergence Discrepancy: 0.00000000000000009126689985681991
2025-02-28 07:31:25 - DEBUG    - ID    Type  V        delta    Pg       Qg       Pl       Ql       Qgmax    Qgmin    Gs       Bs
2025-02-28 07:31:25 - DEBUG    - 1     1     1.000    0.000    3.948    1.143    0.000    0.000    0.000    0.000    0.000    0.000
2025-02-28 07:31:25 - DEBUG    - 2     3     0.834    -22.406  0.000    0.000    8.000    2.800    0.000    0.000    0.000    0.000
2025-02-28 07:31:25 - DEBUG    - 3     2     1.050    -0.597   5.200    3.375    0.800    0.400    4.000    -2.800   0.000    0.000
2025-02-28 07:31:25 - DEBUG    - 4     3     1.019    -2.834   0.000    0.000    0.000    0.000    0.000    0.000    0.000    0.000
2025-02-28 07:31:25 - DEBUG    - 5     3     0.974    -4.548   0.000    0.000    0.000    0.000    0.000    0.000    0.000    0.000
``` 

# deltaFlow
Power System Analysis Tool

## Usage:
```shell
./bin/deltaFlow.pl 
```

```
2025-02-08 14:11:13 - INFO     - ---------------------------------*- MATLAB -*-----------------------------------
2025-02-08 14:11:13 - INFO     - 
2025-02-08 14:11:13 - INFO     -                                    /\\
2025-02-08 14:11:13 - INFO     -                                   /  \\
2025-02-08 14:11:13 - INFO     -                                  /    \\
2025-02-08 14:11:14 - INFO     -                                 /      \\
2025-02-08 14:11:14 - INFO     -                                 =========
2025-02-08 14:11:14 - INFO     - 
2025-02-08 14:11:14 - INFO     -                                 deltaFlow
2025-02-08 14:11:14 - INFO     -                         Power System Analysis Tool
2025-02-08 14:11:14 - INFO     -                            Copyright (C) 2025
2025-02-08 14:11:14 - INFO     - --------------------------------------------------------------------------------
2025-02-08 14:11:14 - INFO     - Choose the method for power flow solution:
2025-02-08 14:11:14 - INFO     - 1. Gauss-Seidel
2025-02-08 14:11:14 - INFO     - 2. Newton-Raphson
Enter your choice (1 for Gauss-Seidel, 2 for Newton-Raphson): >>1 
Enter the maximum number of iterations (e.g., 1024): >>1024
Enter the tolerance (e.g., 1E-16): >>1E-16
Enter the path for the bus data file: >>../data/bus-data.csv
Enter the path for the line data file: >>../data/line-data.csv
2025-02-08 14:11:44 - INFO     - Starting Gauss-Seidel method ...
Enter the relaxation coefficient (0 < R_coeff < 2): >>1.6
2025-02-08 14:11:48 - DEBUG    - Over-relaxation enabled (R_coeff > 1), this will accelerate convergence.
2025-02-08 14:11:48 - DEBUG    - Gauss-Seidel converged after: 117 iterations
2025-02-08 14:11:48 - DEBUG    - Convergence Discrepancy: 0.00000000000000009126689985681991
2025-02-08 14:11:48 - DEBUG    - Bus ID     Voltage Phasor            Voltage         Phase (deg)     P               Q              
2025-02-08 14:11:48 - DEBUG    - 1          1                         1.00000000      0.00            3.948388        1.142829       
2025-02-08 14:11:48 - DEBUG    - 2          0.77082-0.31781i          0.83376782      -22.41          -8.000000       -2.800000      
2025-02-08 14:11:48 - DEBUG    - 3          1.0499-0.010947i          1.05000000      -0.60           4.400000        2.974796       
2025-02-08 14:11:48 - DEBUG    - 4          1.0181-0.050396i          1.01930228      -2.83           0.000000        0.000000       
2025-02-08 14:11:48 - DEBUG    - 5          0.97122-0.077254i         0.97428847      -4.55           0.000000        0.000000   
``` 

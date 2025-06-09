# deltaFlow

`deltaFlow` is a command-line tool for solving the power flow problem. It reads bus and branch data from CSV files and computes the steady-state power flow using numerical methods.

## Build Instructions

Requires a C++17-compatible compiler.

To build the project, run:

```sh
./bin/build.pl
```

## Usage

```sh
./deltaFlow [OPTIONS] <method>
```

### Required Arguments

* `-b`, `--bus <file>`: Path to the bus data CSV file
* `-l`, `--branch <file>`: Path to the branch data CSV file
* `<method>`: Power flow solver to use — either `gauss-seidel` or `newton-raphson`

### Optional Arguments

* `-t`, `--tolerance <value>`: Convergence tolerance (default: `1E-8`)
* `-m`, `--max-iterations <int>`: Maximum number of iterations (default: `1024`)
* `-r`, `--relaxation <value>`: Relaxation factor for Gauss-Seidel method (default: `1.0`; ignored for Newton-Raphson)
* `-h`, `--help`: Display help information and exit
* `-v`, `--version`: Show version information and exit

## Examples

```sh
./deltaFlow -b bus.csv -l branch.csv gauss-seidel
```

```sh
./deltaFlow --bus bus.csv --branch branch.csv newton-raphson
```

## Notes

* Both bus and branch files must be provided.
* The `--relaxation` option applies only to the Gauss-Seidel method.

## Version Information

To check the version:

```sh
./deltaFlow --version
```

## Help

To view help information:

```sh
./deltaFlow --help
```

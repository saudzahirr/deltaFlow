# deltaFlow

![deltaFlow](./docs/assets/deltaFlow.png)

**deltaFlow** is a command-line power flow analysis tool for electrical power systems.
It solves the steady-state power flow equations using the Gauss-Seidel and Newton-Raphson
iterative methods, with automatic reactive power limit (Q-limit) enforcement for
voltage-controlled (PV) buses.

deltaFlow reads standard industry input formats — IEEE Common Data Format (`.cdf`, `.txt`)
and PSS/E Raw Format (`.raw`) — and produces bus voltage/power summaries and line flow reports.

---

## Features

- **Solvers:** Gauss-Seidel (with relaxation) and Newton-Raphson
- **Q-limit enforcement:** Automatic PV-to-PQ bus type switching when reactive limits are violated
- **Input formats:** IEEE Common Data Format and PSS/E Raw Format (v32/v33)
- **Validated:** Tested against IEEE 14, 30, 57, 118, and 300-bus standard test cases
- **Cross-platform:** Builds on Linux (GCC) and Windows (MSVC)

---

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| [Eigen](https://eigen.tuxfamily.org/) | 3.4.0 | Linear algebra |
| [fmt](https://fmt.dev/) | 10.2.1 | Formatting and logging |
| [Catch2](https://github.com/catchorg/Catch2) | 3.5.4 | Unit testing |

Dependencies are managed automatically via [Conan](https://conan.io/).

---

## Build Instructions

**Requirements:** C++17 compiler, CMake (>= 3.25), Conan 2, Perl

```sh
./bin/build.pl -b    # Build only
./bin/build.pl -t    # Build and run tests
./bin/build.pl -d    # Generate documentation (requires Doxygen)
```

---

## Usage

```
deltaFlow [OPTIONS] <input-file> <solver>
```

### Required arguments

| Argument | Description |
|----------|-------------|
| `<input-file>` | Path to input file (`.cdf`, `.txt`, or `.raw`) |
| `<solver>` | Solver method: `GAUSS` or `NEWTON` |

### Options

| Option | Description | Default |
|--------|-------------|---------|
| `-j, --job <name>` | Job name (used for output labeling) | Input filename stem |
| `-t, --tolerance <value>` | Convergence tolerance | `1E-8` |
| `-m, --max-iterations <int>` | Maximum solver iterations | `1024` |
| `-r, --relaxation <value>` | Relaxation coefficient (Gauss-Seidel only) | `1.0` |
| `-h, --help` | Display help message | |
| `-v, --version` | Show version and exit | |

---

## Documentation

API documentation is generated with Doxygen:

```sh
./bin/build.pl -d
```

---

## License

deltaFlow is licensed under the [GNU General Public License v3.0](LICENSE).

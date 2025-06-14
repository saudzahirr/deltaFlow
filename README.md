# deltaFlow

![deltaFlow](./docs/deltaFlow.png)

**deltaFlow** is a command-line tool that solves steady-state power flow problems using numerical methods like Gauss-Seidel and Newton-Raphson. It reads input from CSV files for bus and branch data.

---

## 🔧 Build Instructions

Requires: A C++17-compatible compiler and CMake.

Build the project with:

```sh
./bin/build.pl -t   # Run build and tests
```

Other options:

* `-b`, `--build`: Build only (no tests)
* `-d`, `--doc`: Generate documentation (requires Doxygen)

---

## 🚀 Usage

```sh
./deltaFlow [OPTIONS] <method>
```

### Required:

* `-b`, `--bus <file>`: Bus data CSV
* `-l`, `--branch <file>`: Branch data CSV
* `<method>`: Power flow method (`gauss-seidel` or `newton-raphson`)

### Optional:

* `-t`, `--tolerance <value>`: Convergence tolerance (default: `1E-8`)
* `-m`, `--max-iterations <int>`: Max iterations (default: `1024`)
* `-r`, `--relaxation <value>`: Relaxation factor (Gauss-Seidel only, default: `1.0`)
* `-h`, `--help`: Show help
* `-v`, `--version`: Show version

---

## 🧪 Examples

```sh
./deltaFlow -b data/bus.csv -l data/line.csv gauss-seidel
./deltaFlow --bus data/bus.csv --line data/line.csv newton-raphson
```

---

## 📌 Notes

* Both bus and branch files are required.
* `--relaxation` is ignored with Newton-Raphson.

---

## 📄 Documentation

Generate with:

```sh
./bin/build.pl -d
```

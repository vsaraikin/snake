![CI](https://github.com/vsaraikin/snake/actions/workflows/ci.yml/badge.svg)
![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)
![SFML 3.0](https://img.shields.io/badge/SFML-3.0.2-green.svg)
![Tests](https://img.shields.io/badge/tests-16%20passed-brightgreen.svg)
![Coverage](https://img.shields.io/badge/coverage-47%25-yellow.svg)

# Snake

Classic snake game built with C++23 and SFML 3.

![Screenshot](assets/img.png)

## Build & Run

```bash
make build   # configure + compile
make run     # build + launch
make test    # build + run 16 unit tests
make clean   # remove build artifacts
```

## Code Quality

```bash
make format        # auto-format with clang-format
make format-check  # verify formatting (CI)
make lint          # static analysis with clang-tidy
make coverage      # test coverage report (requires LLVM)
```

## Requirements

- CMake 3.25+
- C++23 compiler (Clang 17+ / GCC 14+)
- SFML 3.0.2 and Catch2 v3.7.1 (fetched automatically)

# Overview

cpp-audio is an open source, cross platform, C++ library for reading and writing
audio data.

## Documentation

The available functions are documented with Doxygen style comments in the C++
header files. For now the easiest way to view the documentation is by simply
opening the header files and reading the comments, but I plan on finding a way
to actually build and formally publish the documentation in the near future.

# Build Instructions

cpp-audio can be built as a standalone CMake project.

    cmake -H. -Bbuild
    cmake --build build

To test the project, run CTest from the build directory.

    cd build
    ctest

## Build Requirements

* CMake v3.11.4 (or later)
* C++17 compliant compiler (or later)
* Conan package manager v1.29.2 (or later)

## Dependencies

This project requires the Conan package manager to automatically manage all of
its dependencies. To install Conan see:
https://docs.conan.io/en/latest/installation.html
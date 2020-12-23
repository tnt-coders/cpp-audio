# Overview

cpp-audio is an open source, cross platform, C++ library for reading and writing
audio data.

## Documentation

[![Documentation](https://codedocs.xyz/tnt-coders/cpp-audio.svg)](https://codedocs.xyz/tnt-coders/cpp-audio/)

Documentation can be found at <https://codedocs.xyz/tnt-coders/cpp-audio/>.

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

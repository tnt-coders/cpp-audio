from conans import ConanFile, CMake, tools

class CppAudio(ConanFile):
    author = "TNT Coders <tnt-coders@googlegroups.com>"
    build_requires = "gtest/1.8.1@bincrafters/stable"
    default_options = {"shared": False}
    description = "C++ library for reading and writing audio files"
    exports_sources = "CMakeLists.txt", "docs/*", "include/*", "src/*", "test/*"
    generators = "cmake", "cmake_paths"
    license = "GNU Lesser General Public License v3.0"
    name = "cpp-audio"
    options = {"shared": [True, False]}
    requires = "cpp-dsp/0.0.0@tnt-coders/testing"
    settings = "os", "compiler", "build_type", "arch"
    topics = ("audio")
    url = "https://github.com/tnt-coders/cpp-audio"
    version = "0.0.0"

    def configure(self):
        tools.check_min_cppstd(self, "17")

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = self.collect_libs()

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake
import os
import re

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, load, rmdir

required_conan_version = ">=2.0.0"


class FtxuiConan(ConanFile):
    name = "ftxui"
    description = "C++ Functional Terminal User Interface."
    license = "MIT"
    url = "https://github.com/ArthurSonzogni/FTXUI"
    homepage = "https://github.com/ArthurSonzogni/FTXUI"
    topics = ("terminal", "tui", "cli", "console", "functional")
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    exports_sources = (
        "CMakeLists.txt",
        "cmake/*",
        "include/*",
        "src/*",
        "ftxui.pc.in",
        "LICENSE",
        # CMakeLists.txt unconditionally add_subdirectory()s these two
        # directories; each one's own CMakeLists.txt returns immediately
        # when its FTXUI_BUILD_* option is off, so nothing else from
        # examples/ or doc/ needs to be exported.
        "examples/CMakeLists.txt",
        "doc/CMakeLists.txt",
    )

    def set_version(self):
        cmake_lists = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
        match = re.search(r"project\(\s*ftxui.*?VERSION\s+(\S+)", cmake_lists, re.DOTALL)
        self.version = match.group(1)

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self, src_folder=".")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED_LIBS"] = bool(self.options.shared)
        tc.variables["FTXUI_BUILD_DOCS"] = False
        tc.variables["FTXUI_BUILD_EXAMPLES"] = False
        tc.variables["FTXUI_BUILD_MODULES"] = False
        tc.variables["FTXUI_BUILD_TESTS"] = False
        tc.variables["FTXUI_BUILD_TESTS_FUZZER"] = False
        tc.variables["FTXUI_ENABLE_INSTALL"] = True
        tc.variables["FTXUI_QUIET"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "ftxui")

        self.cpp_info.components["screen"].set_property("cmake_target_name", "ftxui::screen")
        self.cpp_info.components["screen"].libs = ["ftxui-screen"]

        self.cpp_info.components["dom"].set_property("cmake_target_name", "ftxui::dom")
        self.cpp_info.components["dom"].libs = ["ftxui-dom"]
        self.cpp_info.components["dom"].requires = ["screen"]

        self.cpp_info.components["component"].set_property("cmake_target_name", "ftxui::component")
        self.cpp_info.components["component"].libs = ["ftxui-component"]
        self.cpp_info.components["component"].requires = ["dom"]

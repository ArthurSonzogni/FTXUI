{
  description = "C++ Functional Terminal User Interface library";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {self, nixpkgs, flake-utils}:
    flake-utils.lib.eachDefaultSystem (system:
    let pkgs = import nixpkgs { inherit system; }; in
    let llvm = pkgs.llvmPackages_latest; in
    {
      packages = rec {
        default = pkgs.stdenv.mkDerivation rec {
          pname = "ftxui";
          version = "v4.0.0";
          src = pkgs.fetchFromGitHub {
            owner = "ArthurSonzogni";
            repo = "FTXUI";
            rev = version;
            sha256 = "sha256-3kAhHDUwzwdvHc8JZAcA14tGqa6w69qrN1JXhSxNBQY=";
          };

          nativeBuildInputs = [
            pkgs.cmake
          ];

          cmakeFlags = [
            "-DFTXUI_ENABLE_INSTALL=ON"
            "-DFTXUI_BUILD_EXAMPLES=OFF"
            "-DFTXUI_BUILD_TESTS=OFF"
            "-DFTXUI_BUILD_DOCS=OFF"
            "-DCMAKE_BUILD_TYPE=Release"
          ];

          meta = {
            homepage = "https://arthursonzogni.github.io/FTXUI/";
            description = "C++ Functional Terminal User Interface.";
            longDescription = ''
              Functional Terminal (X) User interface
              
              A simple C++ library for terminal based user interfaces!
              Feature
              - Functional style. Inspired by [1] and React
              - Simple and elegant syntax (in my opinion)
              - Keyboard & mouse navigation.
              - Support for UTF8 and fullwidth chars (→ 测试)
              - Support for animations. Demo 1, Demo 2
              - Support for drawing. Demo
              - No dependencies
              - Cross platform: Linux/MacOS (main target), WebAssembly, Windows (Thanks to contributors!).
              - Learn by examples, and tutorials
              - Multiple packages: CMake FetchContent (preferred), vcpkg, pkgbuild, conan, nix, etc...
              - Good practises: documentation, tests, fuzzers, performance tests, automated CI, automated packaging, etc...
            '';
            license = pkgs.lib.licenses.mit;
            platforms = pkgs.lib.platforms.all;
          };
        };

        ftxui = default;
      };

      devShells = {
        default = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.clang-tools
            llvm.clang
          ];
        };
      };
    }
  );
}

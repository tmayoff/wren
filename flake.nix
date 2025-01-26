{
  description = "Wren game engine";

  nixConfig = {
    extra-substituters = [
      "https://tmayoff.cachix.org"
    ];

    extra-trusted-public-keys = [
      "tmayoff.cachix.org-1:MxpzBMSFgtdDe1ZoDdENnwaRGmworx1aSRriKDpXFn8="
    ];
  };

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = {
    self,
    nixpkgs,
    nixgl,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        overlays = [
          nixgl.overlay
        ];

        pkgs = import nixpkgs {
          inherit system overlays;
        };

        flecs = pkgs.stdenv.mkDerivation {
          name = "flecs";
          src = pkgs.fetchFromGitHub {
            owner = "SanderMertens";
            repo = "flecs";
            rev = "v4.0.2";
            hash = "sha256-HYolfWnsqXAUh1DDwvorLa+t6VZDaP+2+J7zsK3uP40=";
          };

          nativeBuildInputs = with pkgs; [
            cmake
          ];
        };

        slang = pkgs.stdenv.mkDerivation rec {
          pname = "slang";
          version = "v2025.1";
          src = pkgs.fetchFromGitHub {
            owner = "shader-slang";
            repo = "slang";
            rev = "${version}";
            hash = "sha256-uTf2GOEaqcZ8ZKGBFit5UU1JycMLYpKSq0Zvxxct+JI=";
            fetchSubmodules = true;
          };

          cmakeFlags = ["-DSLANG_SLANG_LLVM_FLAVOR=USE_SYSTEM_LLVM"];

          nativeBuildInputs = with pkgs; [cmake python3];

          buildInputs = with pkgs; [llvmPackages_13.libllvm llvmPackages_13.clang-unwrapped xorg.libX11];
        };

        rawNativeBuildInputs = with pkgs; [
          pkg-config
          meson
          cmake
          ninja
          slang
        ];

        rawBuildInputs = with pkgs; [
          boost

          SDL2
          spdlog
          # nlohmann_json
          tomlplusplus

          # vulkan / shaders
          vulkan-headers
          vulkan-loader
          vulkan-memory-allocator
          spirv-headers

          flecs

          # tracy

          # backward-cpp
          # binutils
          # elfutils
          # libdwarf
          # backward-cpp
        ];
        vulkan_layer_path = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d:${pkgs.renderdoc}/share/vulkan/implicit_layer.d";
      in {
        packages = rec {
          wren_editor = pkgs.stdenv.mkDerivation {
            name = "wren_editor";
            src = ./.;

            VK_LAYER_PATH = vulkan_layer_path;

            nativeBuildInputs = rawNativeBuildInputs;
            buildInputs = rawBuildInputs;

            BOOST_INCLUDEDIR = "${pkgs.lib.getDev pkgs.boost}/include";
            BOOST_LIBRARYDIR = "${pkgs.lib.getLib pkgs.boost}/lib";
          };

          default = wren_editor;
        };

        devShell = pkgs.mkShell.override {stdenv = pkgs.llvmPackages_19.stdenv;} {
          hardeningDisable = ["all"];
          VK_LAYER_PATH = vulkan_layer_path;

          nativeBuildInputs = with pkgs;
            [
              vulkan-tools
              llvmPackages_19.clang-tools
              llvmPackages_19.libllvm
              sccache

              just

              gdb

              renderdoc

              # Documentation
              doxygen
              graphviz

              # Coverage
              # gcovr
              lcov
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              lldb
              muon
              mesonlsp
              vulkan-validation-layers

              pkgs.nixgl.nixVulkanIntel
            ]
            ++ rawBuildInputs;

          SPIRV_INCLUDEDIR = "${pkgs.lib.getDev pkgs.spirv-headers}";
          BOOST_INCLUDEDIR = "${pkgs.lib.getDev pkgs.boost}/include";
          BOOST_LIBRARYDIR = "${pkgs.lib.getLib pkgs.boost}/lib";

          shellHook = ''
            export VK_ICD_FILENAMES=$(nixVulkanIntel printenv VK_ICD_FILENAMES)
            export LD_LIBRARY_PATH=$(nixVulkanIntel printenv LD_LIBRARY_PATH):$LD_LIBRARY_PATH

            export CXX="sccache clang++"
          '';
        };
      }
    );
}

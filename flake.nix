{
  description = "Wren game engine";

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

        boost = pkgs.boost185;

        rawNativeBuildInputs = with pkgs; [
          pkg-config
          meson
          cmake
          ninja
        ];

        rawBuildInputs = with pkgs; [
          boost.dev

          SDL2
          spdlog
          # nlohmann_json
          tomlplusplus

          # vulkan / shaders
          vulkan-headers
          vulkan-loader
          vulkan-memory-allocator
          shaderc
          spirv-headers

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
              llvmPackages_17.libllvm
              sccache

              just

              gdb

              renderdoc

              # Documentation
              doxygen
              graphviz

              # Coverage
              # lcov
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              lldb
              muon
              mesonlsp
              vulkan-validation-layers

              pkgs.nixgl.nixVulkanIntel
              grcov
            ]
            ++ rawBuildInputs;

          SPIRV_INCLUDEDIR = "${pkgs.lib.getDev pkgs.spirv-headers}";
          BOOST_INCLUDEDIR = "${pkgs.lib.getDev boost}/include";
          BOOST_LIBRARYDIR = "${pkgs.lib.getLib boost}/lib";

          shellHook = ''
            export VK_ICD_FILENAMES=$(nixVulkanIntel printenv VK_ICD_FILENAMES)
            export LD_LIBRARY_PATH=$(nixVulkanIntel printenv LD_LIBRARY_PATH):$LD_LIBRARY_PATH

            export CXX="sccache clang++"
          '';
        };
      }
    );
}

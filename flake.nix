{
  description = "Wren game engine";

  inputs = {
    # nix-mesonlsp.url = "https://flakehub.com/f/tmayoff/nix-mesonlsp/0.1.7.tar.gz";
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
    nixpkgs-unstable.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = {
    self,
    nixpkgs,
    nixpkgs-unstable,
    nixgl,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        overlays = [
          nixgl.overlay
          # nix-mesonlsp.overlay.default

          (final: prev: {
            xmake = prev.xmake.overrideAttrs (old: {
              src = prev.fetchFromGitHub {
                owner = "xmake-io";
                repo = "xmake";
                rev = "b3c6d968249e01ce2b00417c4d63ae524e883afd";
                hash = "sha256-uxjxEJDdR+1QjCksjFPok3Pt6qA8tkGYA68SOdqfNfs=";
                fetchSubmodules = true;
              };
              patches = [];
            });
          })
        ];

        pkgs = import nixpkgs {
          inherit system overlays;
        };

        unstable = import nixpkgs-unstable {
          inherit system overlays;
        };

        boost = pkgs.boost185;

        # vma = pkgs.stdenv.mkDerivation {
        #   name = "VulkanMemoryAllocator";
        #   src = pkgs.fetchFromGitHub {
        #     owner = "GPUOpen-LibrariesAndSDKs";
        #     repo = "VulkanMemoryAllocator";
        #     rev = "7942b798289f752dc23b0a79516fd8545febd718";
        #     hash = "sha256-x/5OECXCG4rxNtyHZKaMnrNbDjxiP9bQFtQiqEFjNKQ=";
        #   };

        #   nativeBuildInputs = with pkgs; [
        #     cmake
        #   ];
        # };

        rawNativeBuildInputs = with pkgs; [
          pkg-config
          # meson
          # cmake
          # ninja
          unstable.xmake

          # doxygen
          # graphviz
        ];

        rawBuildInputs = with pkgs; [
          boost.dev

          SDL2
          spdlog
          # nlohmann_json
          # tomlplusplus

          # vulkan / shaders
          vulkan-headers
          vulkan-loader
          # vma
          shaderc
          spirv-headers

          # tracy

          # # backward-cpp
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

        devShell = pkgs.mkShell.override {stdenv = unstable.llvmPackages_19.stdenv;} {
          hardeningDisable = ["all"];
          VK_LAYER_PATH = vulkan_layer_path;

          nativeBuildInputs = with pkgs;
            [
              unstable.lua-language-server

              vulkan-tools
              unstable.llvmPackages_19.clang-tools
              # sccache

              just

              gdb

              # renderdoc

              # unstable.tracy-x11 # for the profiler
              wayland
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              unstable.lldb
              vulkan-validation-layers

              pkgs.nixgl.nixVulkanIntel
            ]
            ++ rawBuildInputs;

          SPIRV_INCLUDEDIR = "${pkgs.lib.getDev pkgs.spirv-headers}";
          BOOST_INCLUDEDIR = "${pkgs.lib.getDev boost}/include";
          BOOST_LIBRARYDIR = "${pkgs.lib.getLib boost}/lib";

          shellHook = ''
            export VK_ICD_FILENAMES=$(nixVulkanIntel printenv VK_ICD_FILENAMES)
            export LD_LIBRARY_PATH=$(nixVulkanIntel printenv LD_LIBRARY_PATH):$LD_LIBRARY_PATH
            unset LD
          '';
        };
      }
    );
}

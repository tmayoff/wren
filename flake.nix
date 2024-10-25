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

          (final: prev: {
            xmake = prev.xmake.overrideAttrs (old: {
              src = prev.fetchFromGitHub {
                owner = "xmake-io";
                repo = "xmake";
                rev = "deb55641c247a86ed0321075977d9a067f73b00d";
                hash = "sha256-xi4JhRK8mb3+lsJSv+oP4AF+SGrRNXOMUXsK22Lw6yw=";
                fetchSubmodules = true;
              };
              patches = [];
            });
          })
        ];

        pkgs = import nixpkgs {
          inherit system overlays;
        };

        boost = pkgs.boost185;

        rawNativeBuildInputs = with pkgs; [
          pkg-config

          xmake
          cmake

          doxygen
          graphviz
        ];

        rawBuildInputs = with pkgs; [
          boost.dev

          SDL2
          spdlog
          tomlplusplus

          # vulkan / shaders
          vulkan-headers
          vulkan-loader
          shaderc
          spirv-headers
        ];
      in {
        devShell = pkgs.mkShell.override {stdenv = pkgs.llvmPackages_19.stdenv;} {
          hardeningDisable = ["all"];

          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d:${pkgs.renderdoc}/share/vulkan/implicit_layer.d";

          nativeBuildInputs = with pkgs;
            [
              llvmPackages_19.clang-tools
              llvmPackages_19.clangUseLLVM

              vulkan-tools

              just

              gdb

              renderdoc
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              lldb
              vulkan-validation-layers

              pkgs.nixgl.nixVulkanIntel
            ]
            ++ rawBuildInputs;

          BOOST_INCLUDEDIR = "${pkgs.lib.getDev boost}/include";
          BOOST_LIBRARYDIR = "${pkgs.lib.getLib boost}/lib";

          XMAKE_GLOBALDIR = "./.xmake";
          XMAKE_PKG_CACHEDIR = "./.xmake/cache";
          XMAKE_PKG_INSTALLDIR = "./.xmake/pkgs";

          shellHook = ''
            export VK_ICD_FILENAMES=$(nixVulkanIntel printenv VK_ICD_FILENAMES)
            export LD_LIBRARY_PATH=$(nixVulkanIntel printenv LD_LIBRARY_PATH):$LD_LIBRARY_PATH
            unset LD
          '';
        };
      }
    );
}

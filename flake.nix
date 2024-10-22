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
        ];

        pkgs = import nixpkgs {
          inherit system overlays;
        };

        unstable = import nixpkgs-unstable {
          inherit system overlays;
        };

        boost = pkgs.boost185;

        imgui_subproj = pkgs.stdenv.mkDerivation rec {
          name = "imgui-docking";
          version = "1.91.0";
          src = pkgs.fetchurl {
            url = "https://github.com/ocornut/imgui/archive/refs/tags/v${version}.tar.gz";
            hash = "sha256-bmLIclLms3JbpHihwE3GBKoKrux4/tz0AR8eUlSPTMk=";
          };

          installPhase = ''
            mkdir -p $out
            cp -r ./* $out
          '';
        };

        tracy_subproj = pkgs.stdenv.mkDerivation rec {
          name = "imgui-docking";
          version = "0.10";
          src = pkgs.fetchFromGitHub {
            owner = "wolfpld";
            repo = "tracy";
            rev = "v${version}";
            hash = "sha256-HofqYJT1srDJ6Y1f18h7xtAbI/Gvvz0t9f0wBNnOZK8=";
          };

          installPhase = ''
            mkdir -p $out
            cp -r ./* $out
          '';
        };

        vma = pkgs.stdenv.mkDerivation {
          name = "VulkanMemoryAllocator";
          src = pkgs.fetchFromGitHub {
            owner = "GPUOpen-LibrariesAndSDKs";
            repo = "VulkanMemoryAllocator";
            rev = "7942b798289f752dc23b0a79516fd8545febd718";
            hash = "sha256-x/5OECXCG4rxNtyHZKaMnrNbDjxiP9bQFtQiqEFjNKQ=";
          };

          nativeBuildInputs = with pkgs; [
            cmake
          ];
        };

        # spirv-reflect = pkgs.stdenv.mkDerivation {
        #   name = "spirv-reflect";
        #   src = pkgs.fetchFromGitHub {
        #     owner = "KhronosGroup";
        #     repo = "SPIRV-Reflect";
        #     rev = "vulkan-sdk-1.3.275.0";
        #     hash = "sha256-WnbNEyoutiWs+4Cu9Nv9KfNNmT4gKe/IzyiL/bLb3rg=";
        #   };

        #   cmakeFlags = [
        #     "-DSPIRV_REFLECT_STATIC_LIB=On"
        #   ];

        #   nativeBuildInputs = [
        #     pkgs.meson
        #     pkgs.cmake
        #     pkgs.ninja
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

          # SDL2
          spdlog
          tl-expected
          # nlohmann_json
          # tomlplusplus

          # # vulkan / shaders
          # vulkan-headers
          # vulkan-loader
          # vma
          # shaderc
          # spirv-headers

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

            patchPhase = ''
              mkdir -p subprojects/
              cp -r ${imgui_subproj} subprojects/imgui-1.91.0-docking/
              # cp -r subprojects/packagefiles/imgui-docking/* subprojects/imgui-1.91.0-docking/
              cp -r ${tracy_subproj} subprojects/tracy-0.10/
              ls -ls subprojects

              # chmod 644 -R subprojects
              meson subprojects packagefiles --apply
            '';

            BOOST_INCLUDEDIR = "${pkgs.lib.getDev pkgs.boost}/include";
            BOOST_LIBRARYDIR = "${pkgs.lib.getLib pkgs.boost}/lib";
          };

          default = wren_editor;
        };

        devShell = pkgs.mkShell.override {stdenv = pkgs.clangStdenv;} {
          hardeningDisable = ["all"];
          VK_LAYER_PATH = vulkan_layer_path;

          nativeBuildInputs = with pkgs;
            [
              unstable.lua-language-server

              # vulkan-tools
              # clang-tools
              # unstable.mesonlsp
              # sccache
              # muon

              # just

              # gdb

              # renderdoc

              # unstable.tracy-x11 # for the profiler
              # wayland
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              # unstable.lldb
              # vulkan-validation-layers

              # pkgs.nixgl.nixVulkanIntel
            ]
            ++ rawBuildInputs;

          BOOST_INCLUDEDIR = "${pkgs.lib.getDev boost}/include";
          BOOST_LIBRARYDIR = "${pkgs.lib.getLib boost}/lib";

          shellHook = ''
            # export VK_ICD_FILENAMES=$(nixVulkanIntel printenv VK_ICD_FILENAMES)
            # export LD_LIBRARY_PATH=$(nixVulkanIntel printenv LD_LIBRARY_PATH):$LD_LIBRARY_PATH
            # CXX="sccache clang++"
          '';
        };
      }
    );
}

{
  description = "Wren game engine";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import (nixpkgs) {
          inherit system;
        };

        rawNativeBuildInputs = with pkgs; [
          pkg-config
          meson
          muon
          cmake
          ninja
          vscode-extensions.llvm-org.lldb-vscode
          tracy
          renderdoc
        ];

        rawBuildInputs = with pkgs; [
          catch2_3
          SDL2
          spdlog
          tl-expected
          boost
          gsl-lite
          vulkan-validation-layers
          vulkan-headers
          vulkan-loader
          vulkan-tools
          shaderc
          spirv-headers
          glm
        ];
      in rec {
        vulkan_layer_path = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d:${pkgs.renderdoc}/share/vulkan/implicit_layer.d";
 
        spirv-reflect = pkgs.stdenv.mkDerivation {
          name = "spirv-reflect";
          src = pkgs.fetchFromGitHub {
            owner = "KhronosGroup";
            repo = "SPIRV-Reflect";
            rev = "vulkan-sdk-1.3.275.0";
            hash = "sha256-WnbNEyoutiWs+4Cu9Nv9KfNNmT4gKe/IzyiL/bLb3rg=";
          };
    
          cmakeFlags = [
            "-DSPIRV_REFLECT_STATIC_LIB=On"
          ];

          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
          ];
        };

        packages = rec {
          wren_editor = pkgs.stdenv.mkDerivation {
            name = "wren_editor";
            src = ./.;
            VK_LAYER_PATH = vulkan_layer_path;

            nativeBuildInputs = rawNativeBuildInputs;
            buildInputs = rawBuildInputs;

            installPhase = ''
              mkdir -p $out/bin
              ninja install
            '';
          };

          default = wren_editor;
        };

        devShell = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
            NIX_CFLAGS_COMPILE = "-U_FORTIFY_SOURCE";
            VK_LAYER_PATH = vulkan_layer_path;
            #ENABLE_VULKAN_RENDERDOC_CAPTURE=1;
           
            nativeBuildInputs = with pkgs; [
              clang-tools
              gdb
              muon
              just

              mkdocs
              python311Packages.mkdocs-material
              python311Packages.mkdocs-mermaid2-plugin
            ] ++ rawNativeBuildInputs;

            buildInputs = [
            ] ++ rawBuildInputs;
          };

          shellHook = ''
            export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:${pkgs.spirv-cross}"
          '';
      }
    );
}

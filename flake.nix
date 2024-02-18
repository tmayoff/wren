{
  description = "Wren game engine";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import (nixpkgs) {
          inherit system;
        };

        rawNativeBuildInputs = with pkgs; [
          pkg-config
          meson
          cmake
          ninja
        ];

        rawBuildInputs = with pkgs; [
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
          spirv-cross
        ];
      in rec {

        vulkan_layer_path = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
 
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
           
            nativeBuildInputs = with pkgs; [
              clang-tools
              gdb
              muon
              just

              mkdocs
              python311Packages.mkdocs-material
              python311Packages.mkdocs-mermaid2-plugin
            ] ++ rawNativeBuildInputs;

            buildInputs = with pkgs; [
            ] ++ rawBuildInputs;
          };

          shellHook = ''
            export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:${pkgs.spirv-cross}"
          '';
      }
    );
}

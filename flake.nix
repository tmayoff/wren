{
  description = "Wren game engine";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import (nixpkgs) {
          inherit system;
        };
      in
      {

        devShell = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
          NIX_CFLAGS_COMPILE = "-U_FORTIFY_SOURCE";
          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";

          nativeBuildInputs = with pkgs; [
            clang-tools
            gdb

            pkg-config

            python3
            meson
            muon
            ninja

          ];

          buildInputs = with pkgs; [
            SDL2
            boost
            vulkan-validation-layers
            vulkan-headers
            vulkan-loader
            vulkan-tools
          ];
        };
      }
    );
}

{
  description = "Wren game engine";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
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

        rawNativeBuildInputs = with pkgs; [
          pkg-config
          meson
          cmake
          ninja
        ];

        rawBuildInputs = with pkgs; [
          # catch2_3
          SDL2
          spdlog
          tl-expected
          boost
          vulkan-headers
          tracy
          vma
          shaderc
          spirv-headers
          fontconfig
          imgui
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
            pkgs.meson
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

        devShell = pkgs.mkShell.override {stdenv = pkgs.clangStdenv;} {
          NIX_CFLAGS_COMPILE = "-U_FORTIFY_SOURCE";
          VK_LAYER_PATH = vulkan_layer_path;

          nativeBuildInputs = with pkgs;
            [
              vulkan-loader
              vulkan-tools

              pkgs.nixgl.nixVulkanIntel
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              vulkan-validation-layers
            ]
            ++ rawBuildInputs;
        };

        shellHook = ''
          export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:${pkgs.spirv-cross}"
        '';
      }
    );
}

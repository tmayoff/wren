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

        imgui_patched = pkgs.stdenv.mkDerivation rec {
          name = "imgui";
          version = "1.89.9";
          srcs = [
            (pkgs.fetchFromGitHub {
              name = "imgui";
              owner = "ocornut";
              repo = "imgui";
              rev = "v${version}";
              hash = "sha256-0k9jKrJUrG9piHNFQaBBY3zgNIKM23ZA879NY+MNYTU=";
            })
            (pkgs.fetchzip {
              name = "imgui-meson-patch";
              url = "https://wrapdb.mesonbuild.com/v2/imgui_${version}-1/get_patch.zip";
              hash = "sha256-ZTs7rqoASbMP7qsDb8G/q4IiLX+aPct2Nwyv16oulfk=";
            })
          ];

          sourceRoot = ".";

          installPhase = ''
            mkdir -p $out
            cp -r imgui/* $out
            cp -r imgui-meson-patch/* $out
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
          unstable.meson
          cmake
          ninja
        ];

        rawBuildInputs = with pkgs; [
          doxygen
          graphviz
        
          boost185

          SDL2
          spdlog
          tl-expected
          vulkan-headers
          vulkan-loader
          tracy
          vma
          shaderc
          spirv-headers
          fontconfig
          binutils
          elfutils
          libdwarf
          backward-cpp
        ];
      in rec {
        vulkan_layer_path = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d:${pkgs.renderdoc}/share/vulkan/implicit_layer.d";

        packages = rec {
          wren_editor = pkgs.stdenv.mkDerivation {
            name = "wren_editor";
            src = ./.;

            VK_LAYER_PATH = vulkan_layer_path;

            nativeBuildInputs = rawNativeBuildInputs;
            buildInputs = rawBuildInputs;

            patchPhase = ''
              mkdir -p subprojects
              cp -r ${imgui_patched} subprojects/imgui-${imgui_patched.version}
              ls -ls subprojects
            '';

            mesonFlags = [
              "-Dauto_features=disabled"
            ];

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
              vulkan-tools
              clang-tools
              ccache
              muon

              just

              unstable.tracy-x11 # for the profiler
              wayland

              pkgs.nixgl.nixVulkanIntel
            ]
            ++ rawNativeBuildInputs;

          buildInputs = with pkgs;
            [
              unstable.lldb
              vulkan-validation-layers
            ]
            ++ rawBuildInputs;

          BOOST_INCLUDEDIR = "${pkgs.lib.getDev pkgs.boost}/include";
          BOOST_LIBRARYDIR = "${pkgs.lib.getLib pkgs.boost}/lib";
        };

        shellHook = ''
          export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:${pkgs.spirv-cross}"
        '';
      }
    );
}

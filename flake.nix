{
  description = "Skill-based matchmaking para osu!";

  inputs.nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  inputs.self.submodules = true;

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = [
      "x86_64-linux"
      "aarch64-linux"
      "aarch64-darwin"
    ];
    forEachSupportedSystem = f:
      nixpkgs.lib.genAttrs supportedSystems (
        system:
          f {
            pkgs = import nixpkgs {inherit system;};
          }
      );
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    imgui-latest = pkgs.imgui.overrideAttrs {
      version = "1.93.0";
      src = pkgs.fetchFromGitHub {
        owner = "ocornut";
        repo = "imgui";
        tag = "v1.93.0";
        hash = "sha256-SSrZe05l78C2nea80JmK6Xc8VPYdnRkrOVXG1sYo2VE=";
      };
    };
  in {
    packages = forEachSupportedSystem (
      {pkgs}: {
        # Development version without optimizations and with address sanitizer
        default = pkgs.clangStdenv.mkDerivation {
          pname = "osu-mm";
          version = "0.0.1";
          src = self;
          nativeBuildInputs = with pkgs; [
            pkg-config
          ];
          buildInputs = with pkgs; [
            imgui-latest
            sdl3
            sdl3-ttf
            sdl3-image
            wayland
          ];
          IMGUI_BUILD_OPENGL3_BINDING = true;
          IMGUI_BUILD_GLFW_BINDING = true;
          installFlags = [
            "PREFIX=${placeholder "out"}"
          ];
        };

        # default = pkgs.stdenv.mkDerivation {
        #   pname = "osu-mm";
        #   version = "0.0.1";
        #   src = self;
        #   nativeBuildInputs = with pkgs; [
        #     pkg-config
        #     gcc
        #   ];
        #   buildInputs = with pkgs; [
        #     # rapidcsv
        #     imgui-latest
        #     glfw3
        #   ];
        #   buildPhase = ''
        #     g++ -std=c++11 main.cpp "src/user.cpp" "src/osumm.cpp" -g -O0 -fsanitize=address -fno-omit-frame-pointer -o osu-mm
        #   '';
        #   installPhase = ''
        #     mkdir -p $out/bin
        #     cp osu-mm $out/bin/
        #   '';
        # };
      }
    );
    devShells = forEachSupportedSystem (
      {pkgs}: {
        default = pkgs.mkShell {
          packages = with pkgs;
            [
              gcc
              clang-tools
              cppcheck
              pkg-config
              rapidcsv
              imgui-latest
              sdl3
              sdl3-ttf
              sdl3-image
              wayland
            ]
            ++ (
              if system == "aarch64-darwin"
              then []
              else [lldb]
            );
          shellHook = ''
            export SHELL=/run/current-system/sw/bin/bash
          '';
        };
      }
    );
  };
}

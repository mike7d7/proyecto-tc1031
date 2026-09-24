{
  description = "Skill-based matchmaking para osu!";

  inputs.nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

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
  in {
    packages = forEachSupportedSystem (
      {pkgs}: {
        default = pkgs.stdenv.mkDerivation {
          pname = "osu-mm";
          version = "0.0.1";
          src = self;
          nativeBuildInputs = with pkgs; [
            pkg-config
            gcc
          ];
          buildInputs = with pkgs; [
            # rapidcsv
          ];
          buildPhase = ''
            g++ -std=c++11 main.cpp "src/user.cpp" -o osu-mm
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp osu-mm $out/bin/
          '';
        };
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

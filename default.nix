{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation {
  name = "forma_normal";
  src = ./.;
  nativeBuildInputs = with pkgs; [ cmake ninja ];
  buildInputs = with pkgs; [
    glfw
    xorg.libX11.dev
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
  ];
  hardeningDisable = [ "all" ];
}

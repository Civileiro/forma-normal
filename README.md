# Calculadora de Forma Normal

![image](https://user-images.githubusercontent.com/87790842/159117430-b97e0034-1605-45c2-bdec-448a1e37eaeb.png)

## Pré-requisitos para a compilação

Antes de começar, verifique se você atendeu aos seguintes requisitos:
* Sua máquina suporta OpenGL

Se não usar Nix precisa-rá de: 
* `cmake` 3.16 ou maior 
* Um gerador (Ex. `make`, `ninja`, `Visual Studio`)
* Um compilador de C++ 17

## Compilando

Nix:

```
nix-build
```
O executavel estara em `<pasta do projeto>/result/bin/CalculadoraFN`


Linux:

```
cmake -S . -B build
cmake --build build
```
O executavel estara em `<pasta do projeto>/build/CalculadoraFN`


Windows:
// TODO: UTF-8 + MSVC = pain


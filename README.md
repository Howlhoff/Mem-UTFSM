# Mem - UTFSM
## Author: Diego Beltran
## Title: Algebra de matrices utilizando estructuras compactas tipo k^2-tree

## Pasos de ejecucion

- Instalar libreria sdsl: https://github.com/simongog/sdsl-lite

- Si tiene makefile instalado, ejecutar en terminal linux

> make run

- En caso contrario, ejecutar siguiente comando

> g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib main.cpp -o main -lsdsl -ldivsufsort -ldivsufsort64

> ./main
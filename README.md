# SO2025-Tarea2
En esta tarea se implementó una barrera reutilizable con la librería pthread en C, y un simulador de memoria virtual con el mismo lenguaje.

## Compilación
### barrier.c
```
gcc barrier.c -pthread
```
### sim.c
```
gcc sim.c -lm
```

## Ejecución
### barrier.c
```
./a.out <cantidad-de-hebras> <cantidad-de-etapas>
```
### sim.c
```
./a.out <cantidad-de-marcos> <tamaño-de-marcos> nombre-archivo.txt
o
./a.out <cantidad-de-marcos> <tamaño-de-marcos> [--verbose] nombre-archivo.txt
```

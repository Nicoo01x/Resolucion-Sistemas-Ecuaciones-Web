// Caja de herramientas que uso para laburar con matrices densas de doubles.

#ifndef MATRIZ_H
#define MATRIZ_H

#include <cstddef>
#include <iomanip>
#include <iostream>

#include "tipos.h"

using namespace std;

inline constexpr double EPS = 1e-9;

Matriz crearMatriz(size_t filas, size_t columnas, double valor = 0.0);

bool esCasiCero(double valor);

void limpiarResiduo(double& valor);

void limpiarResiduoMatriz(Matriz& matriz);

void imprimirMatriz(const Matriz& matriz, ostream& salida);

#endif // MATRIZ_H





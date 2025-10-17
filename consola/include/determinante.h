// Mis helpers para sacar determinantes con Sarrus o con Laplace recursiva.

#ifndef DETERMINANTE_H 
#define DETERMINANTE_H

#include <string>
#include <vector>

#include "tipos.h"

using namespace std;

double determinanteAutomatico(const Matriz& matriz, vector<string>& pasos);

double sarrus3x3(const Matriz& matriz, vector<string>& pasos);

double laplaceRecursiva(const Matriz& matriz, vector<string>& pasos, int profundidad = 0);

double determinante2x2(const Matriz& matriz);

#endif // DETERMINANTE_H





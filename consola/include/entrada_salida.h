// Todo el ida y vuelta con el usuario (menus, lecturas, prints) pasa por aca.

#ifndef ENTRADA_SALIDA_H
#define ENTRADA_SALIDA_H

#include <iosfwd>
#include <string>

#include "tipos.h"

using namespace std;

int menuPrincipal();

int solicitarEntero(const string& mensaje, int minimo);

Matriz solicitarMatrizAumentada(int n);

Matriz solicitarMatrizCuadrada(int n);

bool confirmarAccion(const string& mensaje, bool valorPorDefecto = true);

void mostrarPasos(const vector<Paso>& pasos, ostream& destino);

void mostrarVectorSolucion(const vector<double>& solucion, ostream& destino);

void mostrarTitulo(const string& titulo, ostream& destino);

#endif // ENTRADA_SALIDA_H




// Interfaces que uso para aplicar Gauss o Gauss-Jordan sin repetir codigo.

#ifndef RESOLVER_H
#define RESOLVER_H

#include <cstddef>
#include <string>
#include <vector>

#include "tipos.h"

using namespace std;

enum class MetodoSistema {
    Gauss,
    GaussJordan
};

MetodoSistema seleccionarMetodoSistema(size_t orden);

string nombreMetodoSistema(MetodoSistema metodo);

ResultadoSistema resolverSistemaAutomatico(Matriz matriz, vector<Paso>& pasos, MetodoSistema& metodoSeleccionado);

ResultadoSistema resolverPorGauss(Matriz matriz, vector<Paso>& pasos);

ResultadoSistema resolverPorGaussJordan(Matriz matriz, vector<Paso>& pasos);

string describirTipoSistema(const ResultadoSistema& resultado);

#endif // RESOLVER_H





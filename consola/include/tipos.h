// Defino los tipos que comparto entre modulos para evitar repetir estructuras.

#ifndef TIPOS_H
#define TIPOS_H

#include <string>
#include <vector>

using namespace std;

using Matriz = vector<vector<double>>;

struct ResultadoSistema {
    bool tieneSolucion = false;
    bool infinitas = false;
    vector<double> x;
};

struct Paso {
    string descripcion;
    Matriz matriz;
};

#endif // TIPOS_H





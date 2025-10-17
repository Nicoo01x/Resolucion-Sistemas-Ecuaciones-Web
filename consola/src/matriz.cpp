// Funciones sencillas para manejar matrices de doubles.

#include "matriz.h"

#include <cmath>

using namespace std;

Matriz crearMatriz(size_t filas, size_t columnas, double valor) {
    // Crea una matriz con todas las celdas arrancando en el mismo valor.
    return Matriz(filas, vector<double>(columnas, valor));
}

bool esCasiCero(double valor) {
    // Trata como cero cualquier numero chiquito dentro de EPS.
    return fabs(valor) <= EPS;
}

void limpiarResiduo(double& valor) {
    // Si queda un residuo chico despues de operar, lo bajamos a cero.
    if (esCasiCero(valor)) {
        valor = 0.0;
    }
}

void limpiarResiduoMatriz(Matriz& matriz) {
    // Recorre toda la matriz aplicando limpiarResiduo para que no queden restos raros.
    for (auto& fila : matriz) {
        for (auto& elemento : fila) {
            limpiarResiduo(elemento);
        }
    }
}

void imprimirMatriz(const Matriz& matriz, ostream& salida) {
    // Imprime la matriz con dos decimales y columnas prolijas para que se entienda.
    salida << fixed << setprecision(2);
    for (const auto& fila : matriz) {
        for (const auto& elemento : fila) {
            salida << setw(12) << elemento << " ";
        }
        salida << '\n';
    }
}


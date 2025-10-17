// Funciones que resuelven sistemas lineales usando Gauss o Gauss-Jordan.

#include "resolver.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>

#include "matriz.h"

using namespace std;

MetodoSistema seleccionarMetodoSistema(size_t orden) {
    // Elige el metodo mas comodo segun cuantas ecuaciones hay.
    if (orden <= 3) {
        return MetodoSistema::GaussJordan;
    }
    return MetodoSistema::Gauss;
}

string nombreMetodoSistema(MetodoSistema metodo) {
    // Devuelve el nombre del metodo en texto amigable.
    switch (metodo) {
    case MetodoSistema::Gauss:
        return "Eliminacion de Gauss";
    case MetodoSistema::GaussJordan:
        return "Gauss-Jordan";
    default:
        return "Metodo desconocido";
    }
}

namespace {

void registrarPaso(const Matriz& matriz, const string& descripcion, vector<Paso>& pasos) {
    // Guarda la matriz y una descripcion para reconstruir los pasos despues.
    Paso paso;
    paso.descripcion = descripcion;
    paso.matriz = matriz;
    pasos.push_back(paso);
}

bool filaSinCoeficientes(const vector<double>& fila, int columnasCoef) {
    // Chequea si la fila tiene solo ceros en la parte de coeficientes.
    for (int j = 0; j < columnasCoef; ++j) {
        if (!esCasiCero(fila[j])) {
            return false;
        }
    }
    return true;
}

bool filaInconsistente(const vector<double>& fila, int columnasCoef) {
    // Detecta filas tipo 0 ... 0 | b con b distinto de cero (sistema incompatible).
    return filaSinCoeficientes(fila, columnasCoef) && !esCasiCero(fila[columnasCoef]);
}

bool backSubstitution(const Matriz& matriz, vector<double>& solucion) {
    // Hace la sustitucion hacia atras en una matriz triangular.
    const int n = static_cast<int>(matriz.size());
    if (n == 0) {
        return false;
    }
    const int columnas = static_cast<int>(matriz[0].size());
    solucion.assign(static_cast<size_t>(n), 0.0);
    for (int i = n - 1; i >= 0; --i) {
        double pivote = matriz[i][i];
        if (esCasiCero(pivote)) {
            return false;
        }
        double termino = matriz[i][columnas - 1];
        for (int j = i + 1; j < n; ++j) {
            termino -= matriz[i][j] * solucion[static_cast<size_t>(j)];
        }
        solucion[static_cast<size_t>(i)] = termino / pivote;
    }
    return true;
}

ResultadoSistema construirResultado(const Matriz& matriz, bool triangular) {
    // Mira la matriz final para saber si hay solucion y cargarla si se puede.
    ResultadoSistema resultado;
    const int n = static_cast<int>(matriz.size());
    if (n == 0) {
        return resultado;
    }
    const int columnasCoef = static_cast<int>(matriz[0].size()) - 1;
    int filasConCoeficientes = 0;
    for (const auto& fila : matriz) {
        if (filaInconsistente(fila, columnasCoef)) {
            resultado.tieneSolucion = false;
            resultado.infinitas = false;
            return resultado;
        }
        if (!filaSinCoeficientes(fila, columnasCoef)) {
            ++filasConCoeficientes;
        }
    }
    resultado.tieneSolucion = true;
    if (filasConCoeficientes < n) {
        resultado.infinitas = true;
        return resultado;
    }
    resultado.infinitas = false;
    resultado.x.assign(static_cast<size_t>(n), 0.0);
    if (triangular) {
        if (!backSubstitution(matriz, resultado.x)) {
            resultado.tieneSolucion = false;
            resultado.infinitas = false;
            resultado.x.clear();
        }
    } else {
        // Si ya esta reducida, leemos la ultima columna como solucion.
        for (int i = 0; i < n; ++i) {
            resultado.x[static_cast<size_t>(i)] = matriz[i][columnasCoef];
        }
    }
    return resultado;
}

} // namespace

ResultadoSistema resolverSistemaAutomatico(Matriz matriz,
                                           vector<Paso>& pasos,
                                           MetodoSistema& metodoSeleccionado) {
    // Decide que metodo correr y delega la resolucion.
    metodoSeleccionado = seleccionarMetodoSistema(matriz.size());
    if (metodoSeleccionado == MetodoSistema::GaussJordan) {
        return resolverPorGaussJordan(matriz, pasos);
    }
    return resolverPorGauss(matriz, pasos);
}

ResultadoSistema resolverPorGauss(Matriz matriz, vector<Paso>& pasos) {
    // Hace la eliminacion hacia adelante guardando cada operacion.
    pasos.clear();
    const int n = static_cast<int>(matriz.size());
    if (n == 0) {
        return {};
    }
    const int columnas = static_cast<int>(matriz[0].size());
    registrarPaso(matriz, "Matriz inicial (Gauss)", pasos);
    for (int col = 0; col < n; ++col) {
        int filaPivote = col;
        for (int fila = col; fila < n; ++fila) {
            if (fabs(matriz[fila][col]) > fabs(matriz[filaPivote][col])) {
                filaPivote = fila;
            }
        }
        if (esCasiCero(matriz[filaPivote][col])) {
            continue;
        }
        if (filaPivote != col) {
            swap(matriz[filaPivote], matriz[col]);
            ostringstream descripcion;
            descripcion << "Se intercambia F" << (col + 1) << " con F" << (filaPivote + 1);
            registrarPaso(matriz, descripcion.str(), pasos);
        }
        for (int fila = col + 1; fila < n; ++fila) {
            double factor = matriz[fila][col] / matriz[col][col];
            if (esCasiCero(factor)) {
                continue;
            }
            // Sacamos el valor que queda debajo del pivote modificando la fila.
            for (int j = col; j < columnas; ++j) {
                matriz[fila][j] -= factor * matriz[col][j];
                limpiarResiduo(matriz[fila][j]);
            }
            ostringstream descripcion;
            descripcion << fixed << setprecision(2);
            descripcion << "F" << (fila + 1) << " <- F" << (fila + 1) << " - (" << factor << ")*F" << (col + 1);
            registrarPaso(matriz, descripcion.str(), pasos);
        }
    }
    limpiarResiduoMatriz(matriz);
    registrarPaso(matriz, "Matriz escalonada", pasos);
    return construirResultado(matriz, true);
}

ResultadoSistema resolverPorGaussJordan(Matriz matriz, vector<Paso>& pasos) {
    // Deja la matriz en forma reducida limpiando arriba y abajo de cada pivote.
    pasos.clear();
    const int n = static_cast<int>(matriz.size());
    if (n == 0) {
        return {};
    }
    const int columnas = static_cast<int>(matriz[0].size());
    registrarPaso(matriz, "Matriz inicial (Gauss-Jordan)", pasos);
    int filaActual = 0;
    for (int col = 0; col < columnas - 1 && filaActual < n; ++col) {
        int filaPivote = filaActual;
        for (int fila = filaActual; fila < n; ++fila) {
            if (fabs(matriz[fila][col]) > fabs(matriz[filaPivote][col])) {
                filaPivote = fila;
            }
        }
        if (esCasiCero(matriz[filaPivote][col])) {
            continue;
        }
        if (filaPivote != filaActual) {
            swap(matriz[filaPivote], matriz[filaActual]);
            ostringstream descripcion;
            descripcion << "Se intercambia F" << (filaActual + 1) << " con F" << (filaPivote + 1);
            registrarPaso(matriz, descripcion.str(), pasos);
        }
        double pivote = matriz[filaActual][col];
        for (int j = col; j < columnas; ++j) {
            matriz[filaActual][j] /= pivote;
            limpiarResiduo(matriz[filaActual][j]);
        }
        {
            ostringstream descripcion;
            descripcion << fixed << setprecision(2);
            descripcion << "F" << (filaActual + 1) << " se divide por " << pivote;
            registrarPaso(matriz, descripcion.str(), pasos);
        }
        for (int fila = 0; fila < n; ++fila) {
            if (fila == filaActual) {
                continue;
            }
            double factor = matriz[fila][col];
            if (esCasiCero(factor)) {
                continue;
            }
            // Apaga el resto de la columna del pivote usando la fila actual.
            for (int j = col; j < columnas; ++j) {
                matriz[fila][j] -= factor * matriz[filaActual][j];
                limpiarResiduo(matriz[fila][j]);
            }
            ostringstream descripcion;
            descripcion << fixed << setprecision(2);
            descripcion << "F" << (fila + 1) << " <- F" << (fila + 1) << " - (" << factor << ")*F"
                        << (filaActual + 1);
            registrarPaso(matriz, descripcion.str(), pasos);
        }
        ++filaActual;
    }
    limpiarResiduoMatriz(matriz);
    registrarPaso(matriz, "Matriz reducida", pasos);
    return construirResultado(matriz, false);
}

string describirTipoSistema(const ResultadoSistema& resultado) {
    // Devuelve la etiqueta abreviada (SS, CI o CD) segun el resultado.
    if (!resultado.tieneSolucion) {
        return "SS (Sistema incompatible)";
    }
    if (resultado.infinitas) {
        return "CI (Sistema compatible indeterminado)";
    }
    return "CD (Sistema compatible determinado)";
}

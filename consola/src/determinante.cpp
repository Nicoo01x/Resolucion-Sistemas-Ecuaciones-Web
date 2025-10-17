// Aca resuelvo determinantes eligiendo Sarrus o Laplace segun convenga.

#include "determinante.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "matriz.h"

using namespace std;

namespace {
string espacios(int nivel) {
    // Devuelve unos espacios extra segun la profundidad para mantener prolijos los textos.
    return string(static_cast<size_t>(nivel * 2), ' ');
}

string numero(double valor) {
    // Arma un string con dos decimales asi los pasos se leen mas parejos.
    ostringstream salida;
    salida << fixed << setprecision(2) << valor;
    return salida.str();
}

} // namespace

double determinanteAutomatico(const Matriz& matriz, vector<string>& pasos) {
    // Elige el metodo que mas conviene segun el tamano de la matriz y va contando la decision.
    pasos.clear();
    if (matriz.empty()) {
        pasos.push_back("La matriz no puede estar vacia.");
        return 0.0;
    }
    const size_t orden = matriz.size();
    for (const auto& fila : matriz) {
        if (fila.size() != orden) {
            pasos.push_back("La matriz debe ser cuadrada para calcular la determinante.");
            return 0.0;
        }
    }
    const int n = static_cast<int>(orden);
    if (n == 1) {
        pasos.push_back("Metodo: Caso base matriz 1x1.");
        pasos.push_back("Determinante = " + numero(matriz[0][0]));
        return matriz[0][0];
    }
    if (n == 2) {
        pasos.push_back("Metodo: Expansion de Laplace (caso base 2x2).");
        const double det = determinante2x2(matriz);
        pasos.push_back("Determinante = a11*a22 - a12*a21 = " + numero(det));
        return det;
    }
    if (n == 3) {
        pasos.push_back("Metodo: Regla de Sarrus para matrices 3x3.");
        return sarrus3x3(matriz, pasos);
    }
    pasos.push_back("Metodo: Expansion de Laplace recursiva para matriz de orden " + to_string(n) + ".");
    return laplaceRecursiva(matriz, pasos, 0);
}

double sarrus3x3(const Matriz& matriz, vector<string>& pasos) {
    // Usa la regla de Sarrus sumando diagonales principales y secundarias para mezclarlas al final.
    const double diagPrincipal =
        matriz[0][0] * matriz[1][1] * matriz[2][2] +
        matriz[0][1] * matriz[1][2] * matriz[2][0] +
        matriz[0][2] * matriz[1][0] * matriz[2][1];
    const double diagSecundaria =
        matriz[0][2] * matriz[1][1] * matriz[2][0] +
        matriz[0][0] * matriz[1][2] * matriz[2][1] +
        matriz[0][1] * matriz[1][0] * matriz[2][2];
    pasos.push_back("Suma diagonal principal: " + numero(diagPrincipal));
    pasos.push_back("Suma diagonal secundaria: " + numero(diagSecundaria));
    double resultado = diagPrincipal - diagSecundaria;
    pasos.push_back("Determinante = principal - secundaria = " + numero(resultado));
    return resultado;
}

double laplaceRecursiva(const Matriz& matriz, vector<string>& pasos, int profundidad) {
    // Aplica Laplace por la primera fila y llama recursivamente a cada menor que aparece.
    const int n = static_cast<int>(matriz.size());
    if (n == 1) {
        pasos.push_back(espacios(profundidad) + "Base 1x1: det = " + numero(matriz[0][0]));
        return matriz[0][0];
    }
    if (n == 2) {
        double det = determinante2x2(matriz);
        pasos.push_back(espacios(profundidad) + "Determinante 2x2 auxiliar = " + numero(det));
        return det;
    }
    double suma = 0.0;
    for (int col = 0; col < n; ++col) {
        double elemento = matriz[0][col];
        if (esCasiCero(elemento)) {
            continue;
        }
        // Armamos el menor a mano sacando la fila 0 y la columna del elemento actual.
        Matriz menor = crearMatriz(static_cast<size_t>(n - 1), static_cast<size_t>(n - 1), 0.0);
        for (int i = 1; i < n; ++i) {
            int destinoCol = 0;
            for (int j = 0; j < n; ++j) {
                if (j == col) {
                    continue;
                }
                menor[i - 1][destinoCol] = matriz[i][j];
                ++destinoCol;
            }
        }
        double signo = ((col % 2) == 0) ? 1.0 : -1.0;
        pasos.push_back(espacios(profundidad) + "Elemento a[1][" + to_string(col + 1) + "] = " + numero(elemento) +
                       " con signo " + (signo > 0 ? "+" : "-"));
        double detMenor = laplaceRecursiva(menor, pasos, profundidad + 1);
        double aporte = signo * elemento * detMenor;
        pasos.push_back(espacios(profundidad) + "Aporte = " + numero(signo) + " * " + numero(elemento) + " * " +
                       numero(detMenor) + " = " + numero(aporte));
        suma += aporte;
    }
    if (profundidad == 0) {
        pasos.push_back("Determinante final tras Laplace = " + numero(suma));
    }
    return suma;
}

double determinante2x2(const Matriz& matriz) {
    // Caso mini 2x2: aplico la formula clasica a*d - b*c sin vueltas.
    if (matriz.size() != 2 || matriz[0].size() != 2) {
        return 0.0;
    }
    return matriz[0][0] * matriz[1][1] - matriz[0][1] * matriz[1][0];
}





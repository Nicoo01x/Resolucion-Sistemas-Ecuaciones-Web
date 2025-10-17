// Todo el manejo de entrada y salida lo piloteo desde aca.

#include "entrada_salida.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#include "matriz.h"

using namespace std;

namespace {

double solicitarDouble(const string& mensaje) {
    // Pide un double y sigue preguntando hasta que la entrada tenga sentido.
    double valor = 0.0;
    while (true) {
        cout << mensaje;
        if (cin >> valor) {
            return valor;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Intente nuevamente." << endl;
    }
}

void limpiarFlujo() {
    // Limpia el buffer para que el Enter anterior no moleste.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

} // namespace

int menuPrincipal() {
    // Muestra las opciones del programa y obliga a elegir una valida.
    cout << endl << "=== RESOLUCION DE SISTEMAS Y DETERMINANTES ===" << endl;
    cout << "1) Resolver sistema (seleccion automatica)" << endl;
    cout << "2) Resolver sistema por eliminacion de Gauss" << endl;
    cout << "3) Resolver sistema por Gauss-Jordan" << endl;
    cout << "4) Calcular determinante (seleccion automatica)" << endl;
    cout << "5) Salir" << endl;
    int opcion = solicitarEntero("Seleccione una opcion: ", 1);
    while (opcion < 1 || opcion > 5) {
        cout << "Opcion invalida." << endl;
        opcion = solicitarEntero("Seleccione una opcion: ", 1);
    }
    return opcion;
}

int solicitarEntero(const string& mensaje, int minimo) {
    // Pide un entero minimo y no sale hasta que el usuario le pega.
    int valor = 0;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor >= minimo) {
            limpiarFlujo();
            return valor;
        }
        cin.clear();
        limpiarFlujo();
        cout << "Valor invalido. Ingrese un entero >= " << minimo << "." << endl;
    }
}

Matriz solicitarMatrizAumentada(int n) {
    // Arma la matriz aumentada pidiendo coeficientes y terminos independientes uno por uno.
    Matriz matriz = crearMatriz(static_cast<size_t>(n), static_cast<size_t>(n + 1), 0.0);
    cout << "Ingrese las ecuaciones del sistema." << endl;
    for (int i = 0; i < n; ++i) {
        cout << endl << "Ecuacion " << (i + 1) << ":" << endl;
        for (int j = 0; j < n; ++j) {
            ostringstream mensaje;
            mensaje << "Coeficiente de x" << (j + 1) << ": ";
            matriz[i][j] = solicitarDouble(mensaje.str());
        }
        matriz[i][n] = solicitarDouble("Termino independiente: ");
    }
    limpiarFlujo();
    return matriz;
}

Matriz solicitarMatrizCuadrada(int n) {
    // Pide una matriz cuadrada n x n llenando cada posicion.
    Matriz matriz = crearMatriz(static_cast<size_t>(n), static_cast<size_t>(n), 0.0);
    cout << "Ingrese la matriz cuadrada (" << n << " x " << n << "):" << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            ostringstream mensaje;
            mensaje << "a[" << (i + 1) << "][" << (j + 1) << "] = ";
            matriz[i][j] = solicitarDouble(mensaje.str());
        }
    }
    limpiarFlujo();
    return matriz;
}

bool confirmarAccion(const string& mensaje, bool valorPorDefecto) {
    // Toma una respuesta tipo si/no y usa un valor por defecto si se aprieta Enter.
    string opcionPorDefecto = valorPorDefecto ? "[S/n]" : "[s/N]";
    while (true) {
        cout << mensaje << " " << opcionPorDefecto << ": ";
        string respuesta;
        getline(cin, respuesta);
        if (respuesta.empty()) {
            return valorPorDefecto;
        }
        if (respuesta == "s" || respuesta == "S" || respuesta == "si" || respuesta == "SI" || respuesta == "Si") {
            return true;
        }
        if (respuesta == "n" || respuesta == "N" || respuesta == "no" || respuesta == "NO" || respuesta == "No") {
            return false;
        }
        cout << "Respuesta invalida. Use 's' o 'n'." << endl;
    }
}

void mostrarPasos(const vector<Paso>& pasos, ostream& destino) {
    // Lista cada paso del metodo con su matriz para ver como va cambiando.
    destino << fixed << setprecision(2);
    int indice = 1;
    for (const auto& paso : pasos) {
        destino << endl << "Paso " << indice++ << ": " << paso.descripcion << endl;
        imprimirMatriz(paso.matriz, destino);
    }
}

void mostrarVectorSolucion(const vector<double>& solucion, ostream& destino) {
    // Muestra el vector solucion en formato [ x1, x2, ... ].
    destino << fixed << setprecision(2);
    destino << "[ ";
    for (size_t i = 0; i < solucion.size(); ++i) {
        destino << solucion[i];
        if (i + 1 < solucion.size()) {
            destino << ", ";
        }
    }
    destino << " ]" << endl;
}

void mostrarTitulo(const string& titulo, ostream& destino) {
    // Escribe un titulo con banditas === para dividir secciones.
    destino << endl << "=== " << titulo << " ===" << endl;
}





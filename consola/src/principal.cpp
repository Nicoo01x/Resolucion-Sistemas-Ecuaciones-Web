// Programa principal pensado para coordinar la entrada, resolucion y reportes.

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "archivos.h"
#include "determinante.h"
#include "entrada_salida.h"
#include "matriz.h"
#include "resolver.h"

using namespace std;

namespace {

void mostrarResumenSistema(const ResultadoSistema& resultado) {
    // Cuenta rapido que tipo de sistema salio y muestra el vector si es unico.
    cout << "\nTipo de sistema: " << describirTipoSistema(resultado) << "\n";
    if (!resultado.tieneSolucion) {
        cout << "El sistema no tiene solucion.\n";
        return;
    }
    if (resultado.infinitas) {
        cout << "El sistema tiene infinitas soluciones.\n";
        return;
    }
    cout << "Vector solucion aproximado: ";
    mostrarVectorSolucion(resultado.x, cout);
}

string construirReporteSistema(const vector<Paso>& pasos,
                               const ResultadoSistema& resultado,
                               MetodoSistema metodo) {
    // Arma el reporte completo del sistema paso a paso para poder guardarlo.
    ostringstream reporte;
    reporte << fixed << setprecision(2);
    reporte << "=== REPORTE DEL SISTEMA DE ECUACIONES ===\n";
    reporte << "Metodo elegido: " << nombreMetodoSistema(metodo) << "\n";
    reporte << "Clasificacion: " << describirTipoSistema(resultado) << "\n\n";
    for (size_t i = 0; i < pasos.size(); ++i) {
        reporte << "Paso " << (i + 1) << ": " << pasos[i].descripcion << "\n";
        imprimirMatriz(pasos[i].matriz, reporte);
        reporte << "\n";
    }
    if (!resultado.tieneSolucion) {
        reporte << "Conclusiones: Sistema incompatible.\n";
    } else if (resultado.infinitas) {
        reporte << "Conclusiones: Sistema compatible indeterminado.\n";
    } else {
        ostringstream vectorSolucion;
        mostrarVectorSolucion(resultado.x, vectorSolucion);
        reporte << "Conclusiones: Sistema compatible determinado.\n";
        reporte << "Vector solucion: " << vectorSolucion.str();
    }
    return reporte.str();
}

void presentarResultadoSistema(const vector<Paso>& pasos,
                               const ResultadoSistema& resultado,
                               MetodoSistema metodo) {
    // Muestra el proceso en pantalla y pregunta si lo queremos guardar.
    cout << "Metodo utilizado: " << nombreMetodoSistema(metodo) << "\n";
    mostrarPasos(pasos, cout);
    mostrarResumenSistema(resultado);

    const string reporte = construirReporteSistema(pasos, resultado, metodo);

    bool guardar = confirmarAccion("Guardar reporte del sistema en ./salidas/?", true);
    if (guardar) {
        string ruta;
        if (guardarReporte(reporte, ruta)) {
            cout << "Reporte guardado en: " << ruta << "\n";
        } else {
            cout << "No fue posible guardar el reporte.\n";
        }
    }
}

ResultadoSistema resolverSistemaPorMetodo(Matriz matriz,
                                          vector<Paso>& pasos,
                                          MetodoSistema metodo) {
    // Llama al metodo pedido para resolver el sistema y deja el resto igual.
    switch (metodo) {
    case MetodoSistema::Gauss:
        return resolverPorGauss(matriz, pasos);
    case MetodoSistema::GaussJordan:
        return resolverPorGaussJordan(matriz, pasos);
    default:
        return {};
    }
}

void ejecutarSistemaConMetodo(MetodoSistema metodo) {
    // Pide dimension y datos, corre el metodo elegido y muestra que pasa.
    const string titulo = (metodo == MetodoSistema::Gauss)
                              ? "Resolucion por eliminacion de Gauss"
                              : "Resolucion por Gauss-Jordan";
    mostrarTitulo(titulo, cout);
    int n = solicitarEntero("Ingrese la cantidad de incognitas (n >= 2): ", 2);
    Matriz matriz = solicitarMatrizAumentada(n);

    vector<Paso> pasos;
    const ResultadoSistema resultado = resolverSistemaPorMetodo(matriz, pasos, metodo);
    presentarResultadoSistema(pasos, resultado, metodo);
}

void ejecutarSistemaAutomatico() {
    // Decide entre Gauss o Gauss-Jordan segun el orden y ejecuta.
    mostrarTitulo("Resolucion automatica de sistemas", cout);
    int n = solicitarEntero("Ingrese la cantidad de incognitas (n >= 2): ", 2);
    Matriz matriz = solicitarMatrizAumentada(n);

    vector<Paso> pasos;
    MetodoSistema metodo = MetodoSistema::Gauss;
    ResultadoSistema resultado = resolverSistemaAutomatico(matriz, pasos, metodo);

    presentarResultadoSistema(pasos, resultado, metodo);
}

void ejecutarDeterminante() {
    // Pide la matriz cuadrada y va mostrando cada paso del calculo de la determinante.
    mostrarTitulo("Calculo de determinante (seleccion automatica)", cout);
    cout << "Metodo fijo: regla de Sarrus disponible solo para matrices 3x3.\n";
    const int ordenPermitido = 3;
    int ordenIngresado = solicitarEntero("Ingrese el orden de la matriz cuadrada (n = 3): ", ordenPermitido);
    while (ordenIngresado != ordenPermitido) {
        cout << "Solo se permiten matrices de orden 3 para calcular la determinante.\n";
        ordenIngresado = solicitarEntero("Ingrese el orden de la matriz cuadrada (n = 3): ", ordenPermitido);
    }
    Matriz matriz = solicitarMatrizCuadrada(ordenPermitido);

    vector<string> pasosDeterminante;
    double valor = determinanteAutomatico(matriz, pasosDeterminante);

    for (const auto& linea : pasosDeterminante) {
        cout << linea << '\n';
    }
    cout << fixed << setprecision(2);
    cout << "Determinante = " << valor << "\n";

    ostringstream reporte;
    reporte << fixed << setprecision(2);
    reporte << "=== REPORTE DE DETERMINANTE ===\n";
    reporte << "Orden de la matriz: " << ordenPermitido << "\n";
    reporte << "Resultado: " << valor << "\n";
    for (const auto& linea : pasosDeterminante) {
        reporte << linea << '\n';
    }
    const string reporteTexto = reporte.str();

    bool guardar = confirmarAccion("Guardar reporte del determinante en ./salidas/?", true);
    if (guardar) {
        string ruta;
        if (guardarReporte(reporteTexto, ruta)) {
            cout << "Reporte guardado en: " << ruta << "\n";
        } else {
            cout << "No fue posible guardar el reporte.\n";
        }
    }
}

void mostrarHistorialProcedimientos() {
    // Lee los reportes en texto y los muestra por pantalla.
    mostrarTitulo("Ejercicios anteriores", cout);
    const vector<ReporteGuardado> registros = cargarReportesTexto();
    if (registros.empty()) {
        cout << "No hay ejercicios guardados en el historial.\n";
        return;
    }
    for (size_t i = 0; i < registros.size(); ++i) {
        cout << "\n" << registros[i].titulo << "\n";
        cout << registros[i].contenido << "\n";
        if (i + 1 < registros.size()) {
            cout << string(40, '-') << "\n";
        }
    }
}

} // namespace

int main() {
    // Loop principal que muestra el menu y reacciona segun la opcion elegida.
    cout << "=== Aplicacion de Algebra Lineal ===\n";
    bool continuar = true;
    while (continuar) {
        int opcion = menuPrincipal();
        switch (opcion) {
        case 1:
            ejecutarSistemaAutomatico();
            break;
        case 2:
            ejecutarSistemaConMetodo(MetodoSistema::Gauss);
            break;
        case 3:
            ejecutarSistemaConMetodo(MetodoSistema::GaussJordan);
            break;
        case 4:
            ejecutarDeterminante();
            break;
        case 5:
            mostrarHistorialProcedimientos();
            break;
        case 6:
            continuar = false;
            cout << "Ejecucion finalizada.\n";
            break;
        default:
            cout << "Opcion invalida.\n";
            break;
        }
    }
    return 0;
}

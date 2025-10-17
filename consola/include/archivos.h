// Mis utilidades para generar reportes y ordenar la carpeta de salidas.

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <string>
#include <vector>

using namespace std;

struct ReporteGuardado {
    string titulo;
    string contenido;
};

bool guardarReporte(const string& contenido, string& rutaGenerada);

vector<ReporteGuardado> cargarReportesTexto();

#endif // ARCHIVOS_H





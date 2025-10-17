// Mis utilidades para generar reportes y ordenar la carpeta de salidas.

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <string>

using namespace std;

string generarTimestamp();

bool guardarReporte(const string& contenido, string& rutaGenerada);

#endif // ARCHIVOS_H





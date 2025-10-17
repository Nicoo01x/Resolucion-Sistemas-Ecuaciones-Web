// Aca manejo todo lo que tenga que ver con archivos y reportes.

#include "archivos.h"

#include <chrono> //chrono es para manejar tiempos
#include <ctime> //ctime es para controlar el tiempo y fechas )
#include <filesystem> // filesystem es para manejar archivos y carpetas
#include <fstream> //fstream es para manejar archivos
#include <iomanip> //iomanip es para formatear la salida
#include <sstream> //sstream es para manejar flujos de string

using namespace std;

namespace fs = filesystem;

string generarTimestamp() {
    // Genera un nombre con fecha y hora tipo YYYYMMDD_HHMMSS para no pisar archivos.
    const auto ahora = chrono::system_clock::now();
    const time_t tiempo = chrono::system_clock::to_time_t(ahora);
    tm fechaLocal{};
#ifdef _WIN32
    localtime_s(&fechaLocal, &tiempo);
#else
    fechaLocal = *localtime(&tiempo);
#endif
    ostringstream formato; //ostringstream es un flujo que escribe en un string
    formato << put_time(&fechaLocal, "%Y%m%d_%H%M%S");
    return formato.str();
}

bool guardarReporte(const string& contenido, string& rutaGenerada) {
    // Guarda el texto en ./salidas creando la carpeta si falta y devolviendo la ruta final.
    try {
        const fs::path carpeta = fs::path("salidas");
        if (!fs::exists(carpeta)) {
            fs::create_directories(carpeta);
        }
        const string nombre = "reporte_" + generarTimestamp() + ".txt";
        const fs::path ruta = carpeta / nombre;
        ofstream archivo(ruta);
        if (!archivo) {
            return false;
        }
        // Guardamos todo y devolvemos la ruta absoluta para avisar al final.
        archivo << contenido;
        archivo.close();
        rutaGenerada = fs::absolute(ruta).string();
        return true;
    } catch (const exception&) {
        return false;
    }
}




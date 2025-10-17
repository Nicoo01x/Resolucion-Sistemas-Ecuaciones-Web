// Aca manejo todo lo que tenga que ver con archivos y reportes.

#include "archivos.h"

#include <filesystem> // filesystem es para manejar archivos y carpetas
#include <fstream> //fstream es para manejar archivos
#include <algorithm>
#include <cctype>
#include <sstream> //sstream es para manejar flujos de string
#include <vector>

using namespace std;

namespace fs = filesystem;

namespace {

const fs::path CARPETA_SALIDAS{"salidas"};
const string PREFIJO_ARCHIVO = "ejercicio ";

bool asegurarCarpetaSalidas() {
    try {
        if (!fs::exists(CARPETA_SALIDAS)) {
            fs::create_directories(CARPETA_SALIDAS);
        }
        return true;
    } catch (const exception&) {
        return false;
    }
}

int extraerIndice(const fs::path& ruta) {
    string nombre = ruta.stem().string();
    string nombreMinusculas = nombre;
    transform(nombreMinusculas.begin(), nombreMinusculas.end(), nombreMinusculas.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    if (nombreMinusculas.rfind(PREFIJO_ARCHIVO, 0) != 0) {
        return -1;
    }
    string numeroStr = nombre.substr(PREFIJO_ARCHIVO.size());
    try {
        return stoi(numeroStr);
    } catch (const exception&) {
        return -1;
    }
}

int obtenerSiguienteIndice() {
    if (!fs::exists(CARPETA_SALIDAS)) {
        return 1;
    }
    int maxIndice = 0;
    for (const auto& entrada : fs::directory_iterator(CARPETA_SALIDAS)) {
        if (!entrada.is_regular_file()) {
            continue;
        }
        const int indice = extraerIndice(entrada.path());
        if (indice > maxIndice) {
            maxIndice = indice;
        }
    }
    return maxIndice + 1;
}

} // namespace

bool guardarReporte(const string& contenido, string& rutaGenerada) {
    // Guarda el texto en ./salidas creando la carpeta si falta y devolviendo la ruta final.
    try {
        if (!asegurarCarpetaSalidas()) {
            return false;
        }
        const int indice = obtenerSiguienteIndice();
        const string nombre = PREFIJO_ARCHIVO + to_string(indice) + ".txt";
        const fs::path ruta = CARPETA_SALIDAS / nombre;
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

vector<ReporteGuardado> cargarReportesTexto() {
    // Lee todos los reportes guardados como ejercicio N.txt.
    vector<ReporteGuardado> registros;
    if (!fs::exists(CARPETA_SALIDAS)) {
        return registros;
    }
    vector<pair<int, fs::path>> archivosOrdenados;
    for (const auto& entrada : fs::directory_iterator(CARPETA_SALIDAS)) {
        if (!entrada.is_regular_file()) {
            continue;
        }
        const int indice = extraerIndice(entrada.path());
        if (indice <= 0) {
            continue;
        }
        archivosOrdenados.emplace_back(indice, entrada.path());
    }
    sort(archivosOrdenados.begin(), archivosOrdenados.end(),
         [](const auto& a, const auto& b) { return a.first < b.first; });

    for (const auto& [indice, ruta] : archivosOrdenados) {
        ifstream archivo(ruta);
        if (!archivo) {
            continue;
        }
        ostringstream buffer;
        buffer << archivo.rdbuf();
        ReporteGuardado registro;
        registro.titulo = "Ejercicio " + to_string(indice);
        registro.contenido = buffer.str();
        registros.push_back(move(registro));
    }
    return registros;
}




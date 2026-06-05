/**
 *Punto de entrada del programa. Parsea argumentos y despacha al modo correcto.
 *
 * Modos de ejecución:
 *
 *   --benchmark        Ejecuta el benchmark completo (medicion.hpp) sobre tamaños
 *                      y distribuciones predefinidas, y genera mediciones.csv.
 *
 *   -i <archivo.csv>   Carga un archivo CSV con enteros ordenados y entra al modo
 *                      interactivo (cases.hpp) para buscar valores en los tres casos.
 *
 * Uso:
 *   ./main --benchmark
 *   ./main -i ruta/del/archivo.csv
 *
 
 */

#include <cstring>
#include <vector>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <print>
#include "include/generar.hpp"
#include "include/medicion.hpp"
#include "include/cases.hpp"

using namespace std;

int main(int argc, char** argv) {

    // Validar cantidad de argumentos (2 para --benchmark, 3 para -i <archivo>)
    if (argc < 2 || argc > 3)
        return (println("Usage: ./main --benchmark  |  ./main -i <archivo.csv>"), 1);

    // ============================================================
    // MODO BENCHMARK
    // ============================================================
    if (argc == 2 && strncmp(argv[1], "--benchmark", 11) == 0) {
        println("Benchmark mode");
        println("args: {}", argv[1]);

        // Tamaños de arreglo a probar
        // Advertencia: 100000000 (10^8) puede agotar la memoria RAM disponible
        vector<int> sizes = {10000000, 100000000};

        // Desviaciones estándar para la distribución gaussiana
        // Valores bajos → gaps pequeños (más compresibles)
        // Valores altos → gaps grandes (menos compresibles)
        vector<double> dev = {1.0, 5.0, 15.0, 30.0};

        // Cantidades de búsquedas a medir por estructura y tamaño
        vector<int> searchSize = {1000, 10000, 100000};

        // Ejecutar benchmark completo → genera mediciones.csv
        Medicion::medicion<int32_t>(sizes, dev, searchSize);

      

    // ============================================================
    // MODO ARCHIVO (interactivo)
    // ============================================================
    } else if (argc == 3 && strncmp(argv[1], "-i", 2) == 0) {
        string filepath = argv[2];
        println("File mode, loading file {}", filepath);

        ifstream file(filepath);
        if (file.is_open()) {
            vector<int32_t> initial_v;
            string line, token;

            // Leer el CSV línea por línea, separando por comas
            // Acepta números en una sola línea o distribuidos en varias
            while (getline(file, line)) {
                stringstream ss(line);
                while (getline(ss, token, ',')) {
                    if (!token.empty()) {
                        // atoi convierte string a int; rango válido: int32_t [-2^31, 2^31-1]
			// TODO: añadir validación de que el número esté en el rango válido
                        initial_v.push_back(atoi(token.c_str()));
                    }
                }
            }

            // Imprimir todos los elementos cargados (útil para verificar la carga)
            for (int i = 0; i < (int)initial_v.size() - 1; ++i) cout << initial_v[i] << " ";
            cout << endl;

            // Iniciar sesión interactiva: el usuario elige caso y número a buscar
            Case::cases(initial_v);
        }


    }

    return 0;
}

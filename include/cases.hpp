#pragma once
#include <chrono>
#include "busqueda.hpp"
#include "compresion.hpp"
#include "generar.hpp"
#include <iostream>
#include <print>

/**
 * @file cases.hpp
 * @brief Modo interactivo: búsqueda manual en los tres casos.
 *
 * Implementa la función `cases()` que recibe un arreglo ordenado ya cargado
 * desde archivo y permite al usuario buscar valores de forma interactiva,
 * seleccionando el caso (estructura) a usar.
 *
 * Flujo de ejecución:
 *   1. Se construyen automáticamente las tres estructuras a partir de A.
 *   2. El usuario elige caso (1, 2 o 3) y el número a buscar.
 *   3. Se ejecuta la búsqueda y se muestra el resultado con tiempo en microsegundos.
 *   4. Se pregunta si desea continuar (otra búsqueda o salir).
 *
 * NOTA: Los rangos [L, R) son EXCLUSIVOS, consistentes con busqueda.hpp.
 */

namespace Case {
    using namespace std;

    template <typename T>
    void cases(const vector<T> &A) {
        int n = (int)A.size();

        // Construir estructuras auxiliares a partir del arreglo cargado
        vector<T> gap;
        Generar::vectorGap(A, gap, n);

        vector<T> sample;
        Generar::vectorSample(A, sample, n);
        int b = 1024; // salto estático (debe coincidir con busqueda.hpp)

        // Comprimir con Elias-Fano (puede lanzar excepción si hay negativos)
        Compresion::EliasFano ef;
        try {
            ef = Compresion::eliasFanoCodif(A);
        } catch (const invalid_argument &e) {
            println("Advertencia: no se pudo construir Elias-Fano: {}", e.what());
            println("El Caso 3 no estará disponible en esta sesión.");
        }

        int case_num = -1, number = 0;
        print("Ingrese caso (1, 2, 3 o 0 para VOLVER): ");
        cin >> case_num;

        if (case_num == 0) return;

        print("Ingrese numero a buscar: ");
        cin >> number;

        bool cont = true;
        while (cont) {

            // -------------------------------------------------------
            // CASO 1: Búsqueda binaria sobre el arreglo original
            // -------------------------------------------------------
            if (case_num == 1) {
                auto tsearch1 = chrono::high_resolution_clock::now();
                auto it = lower_bound(A.begin(), A.end(), number);
                auto tsearch2 = chrono::high_resolution_clock::now();
                auto tsearch = chrono::duration_cast<chrono::microseconds>(tsearch2 - tsearch1);

                if (it == A.end() || *it != number)
                    println("Numero no fue encontrado.");
                else
                    println("Numero encontrado en posicion {}, en {} us",
                            (int)(it - A.begin()) + 1, tsearch.count());
            }

            // -------------------------------------------------------
            // CASO 2: Gap-Coding + Sample
            // -------------------------------------------------------
            else if (case_num == 2) {
                auto tsearch1 = chrono::high_resolution_clock::now();
                int L = 0, R = 0;
                bool found = false;
                if (Busqueda::sample_range(sample, b, (T)number, n, L, R))
                    found = Busqueda::GC_decode(gap, sample, (T)number, b, L, R);
                auto tsearch2 = chrono::high_resolution_clock::now();
                auto tsearch = chrono::duration_cast<chrono::microseconds>(tsearch2 - tsearch1);

                if (found) println("Numero encontrado con Gap-Coding en {} us", tsearch.count());
                else       println("Numero no fue encontrado.");
            }

            // -------------------------------------------------------
            // CASO 3: Elias-Fano
            // -------------------------------------------------------
            else if (case_num == 3) {
                if (ef.low.empty()) {
                    println("Caso 3 no disponible (Elias-Fano no pudo construirse).");
                } else {
                    auto tsearch1 = chrono::high_resolution_clock::now();
                    int L = 0, R = 0;
                    bool found = false;
                    if (Busqueda::sample_range(sample, b, (T)number, n, L, R))
                        found = Busqueda::EF_decode(ef, (T)number, L, R);
                    auto tsearch2 = chrono::high_resolution_clock::now();
                    auto tsearch = chrono::duration_cast<chrono::microseconds>(tsearch2 - tsearch1);

                    if (found) println("Numero encontrado con Elias-Fano en {} us", tsearch.count());
                    else       println("Numero no fue encontrado.");
                }
            }
            else {
                println("Caso invalido. Ingrese 1, 2 o 3.");
            }

            // -------------------------------------------------------
            // Continuar o salir
            // -------------------------------------------------------
            string req;
            print("Desea continuar? (s/1 para si, n/0 para no): ");
            cin >> req;

            if (req == "n" || req == "0") {
                cont = false;
            } else {
                print("Ingrese caso (1, 2, 3 o 0 para VOLVER): ");
                cin >> case_num;
                if (case_num == 0) return;
                print("Ingrese numero a buscar: ");
                cin >> number;
            }
        }
    }
}
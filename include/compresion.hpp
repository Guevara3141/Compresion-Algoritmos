#pragma once

#include <vector>
#include <cmath>

/**
 * @file compresion.hpp
 * @brief Implementación de la codificación Elias-Fano (Caso 3).
 *
 * Elias-Fano es una técnica de compresión cuasi-sucinta para arreglos de
 * enteros ordenados. Divide cada valor en dos partes:
 *
 *   - Bits bajos (low):  los k bits menos significativos, almacenados directamente.
 *   - Bits altos (high): el cociente v / 2^k, codificado en unario.
 *
 * El parámetro k se elige como k = floor(log2(n)), donde n es el tamaño del arreglo.
 *
 * Esta separación permite saltar eficientemente a una posición aproximada usando
 * los bits altos, y luego leer los bits bajos para reconstruir el valor exacto.
 *
 * Nota: A diferencia de Gap-Coding, Elias-Fano opera sobre los valores originales
 * del arreglo (no sobre los gaps). Por eso el flujo del Caso 3 mantiene el sample
 * del Caso 2 para acotar [L, R] y luego decodifica directamente sobre la estructura EF.
 */

namespace Compresion {
    using namespace std;

    /**
     * @brief Estructura que almacena la codificación Elias-Fano de un arreglo.
     *
     * @param k     Número de bits usados para la parte baja de cada valor.
     * @param low   Concatenación de los k bits bajos de todos los elementos.
     *              Tiene largo n*k bits (un bloque de k bits por elemento).
     * @param high  Codificación unaria de los bits altos de todos los elementos.
     *              Cada valor v contribuye con floor(v/2^k) ceros seguidos de "10".
     */
    struct EliasFano {
        int k;
        vector<bool> low;   // bits bajos: n*k bits en total
        vector<bool> high;  // bits altos en unario
    };

    /**
     * @brief Codifica un arreglo ordenado con la técnica Elias-Fano.
     *
     * Para cada elemento v del arreglo A:
     *   1. Se calculan los k bits bajos: lo = v % 2^k
     *   2. Se calculan los bits altos:   hi = v / 2^k
     *   3. Los k bits de lo se agregan a ef.low (del más significativo al menos).
     *   4. Los bits altos se codifican en unario: hi ceros seguidos de "1" y "0".
     *
     * Ejemplo con k=2 y v=9 (binario: 1001):
     *   lo = 9 % 4 = 1  → bits bajos: 01
     *   hi = 9 / 4 = 2  → unario: 0 0 1 0
     *
     * @tparam T Tipo entero del arreglo (ej. int32_t)
     * @param A Arreglo ordenado de entrada
     * @return Estructura EliasFano con la codificación completa
     */
    template <typename T>
    EliasFano eliasFanoCodif(const vector<T> &A) {
        EliasFano ef;
        long long int n = (int)A.size();

        // k = floor(log2(n)): cantidad de bits para la parte baja
        ef.k = (int)log2(n);

        // d = 2^k: divisor para separar bits altos y bajos
        int d = 1 << ef.k;

        for (size_t i = 0; i < A.size(); i++) {
            T v = A[i];
            int lo = v % d;   // k bits menos significativos
            int hi = v / d;   // bits altos (parte unaria)

            // Almacenar bits bajos de mayor a menor peso (big-endian dentro del bloque)
            for (int j = ef.k - 1; j >= 0; j--) {
                ef.low.push_back((lo >> j) & 1);
            }

            // Codificación unaria de los bits altos:
            // hi ceros (representan el "contador") seguidos del terminador "10"
            for (int j = 0; j < hi; j++) {
                ef.high.push_back(false); // cero: "aún no termina"
            }
            ef.high.push_back(true);  // 1: marca el valor
            ef.high.push_back(false); // 0: separador/terminador
        }

        return ef;
    }
}
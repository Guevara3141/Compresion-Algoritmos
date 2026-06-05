#pragma once

#include <vector>
#include <cmath>
#include <cassert>
#include <stdexcept>

/**
 * @file compresion.hpp
 * @brief Implementación de la codificación Elias-Fano (Caso 3).
 *
 * Elias-Fano es una técnica de compresión cuasi-sucinta para arreglos de
 * enteros ordenados NO NEGATIVOS. Divide cada valor en dos partes:
 *
 *   - Bits bajos (low):  los k bits menos significativos, almacenados directamente.
 *   - Bits altos (high): el cociente v / 2^k, codificado en unario.
 *
 * El parámetro k se elige como k = floor(log2(n)), donde n es el tamaño del arreglo.
 *
 * REQUISITO: todos los valores del arreglo de entrada deben ser >= 0.
 * Los generadores en generar.hpp garantizan esto centrando las distribuciones
 * en valores positivos (FIX: acción 3).
 *
 * Esta separación permite saltar eficientemente a una posición aproximada usando
 * los bits altos, y luego leer los bits bajos para reconstruir el valor exacto.
 */

namespace Compresion {
    using namespace std;

    /**
     * @brief Estructura que almacena la codificación Elias-Fano de un arreglo.
     *
     * @param k     Número de bits para la parte baja de cada valor (= floor(log2(n))).
     * @param low   Concatenación de los k bits bajos de todos los elementos (n*k bits).
     * @param high  Codificación unaria de los bits altos (largo variable).
     */
    struct EliasFano {
        int k;
        vector<bool> low;   // bits bajos: n*k bits en total
        vector<bool> high;  // bits altos en unario
    };

    /**
     * @brief Codifica un arreglo ordenado de enteros NO NEGATIVOS con Elias-Fano.
     *
     * Para cada elemento v del arreglo A:
     *   1. lo = v % 2^k  (k bits bajos, almacenados en ef.low)
     *   2. hi = v / 2^k  (bits altos, codificados en unario en ef.high)
     *
     * Codificación unaria de hi: hi ceros seguidos del terminador "10".
     *
     * Ejemplo con k=2 y v=9 (binario: 1001):
     *   lo = 9 % 4 = 1  → bits bajos en ef.low: 01
     *   hi = 9 / 4 = 2  → unario en ef.high:    0 0 1 0
     *
     * FIX (acción 3): Se valida que todos los valores sean >= 0.
     * Valores negativos producen hi negativo → loop `for (j < hi)` no ejecuta
     * → codificación silenciosamente incorrecta. Ahora se lanza excepción.
     *
     * @tparam T Tipo entero del arreglo (debe ser int32_t con valores >= 0,
     *           o uint32_t)
     * @param A Arreglo ordenado de entrada (todos los valores deben ser >= 0)
     * @return Estructura EliasFano con la codificación completa
     * @throws std::invalid_argument si algún valor de A es negativo
     */
    template <typename T>
    EliasFano eliasFanoCodif(const vector<T> &A) {
        EliasFano ef;
        long long int n = (long long int)A.size();

        // k = floor(log2(n)): cantidad de bits para la parte baja
        ef.k = (n > 1) ? (int)log2((double)n) : 1;

        // d = 2^k: divisor para separar bits altos y bajos
        long long int d = 1LL << ef.k;

        // Pre-reservar espacio estimado para mejorar rendimiento
        ef.low.reserve(n * ef.k);
        // ef.high es de largo variable; reservamos una estimación conservadora
        ef.high.reserve(n * 2);

        for (size_t i = 0; i < A.size(); i++) {
            T v = A[i];

            // FIX (acción 3): Elias-Fano requiere valores no-negativos.
            // Con int32_t, valores negativos generan hi < 0 → el loop
            // `for (j < hi)` no itera → codificación silenciosamente incorrecta.
            if (v < 0) {
                throw invalid_argument(
                    "eliasFanoCodif: el arreglo contiene valores negativos. "
                    "Elias-Fano requiere enteros no negativos. "
                    "Revise la generación de datos (vectorGaussSorted/vectorUniformeSorted)."
                );
            }

            long long int lo = (long long int)v % d;   // k bits menos significativos
            long long int hi = (long long int)v / d;   // bits altos (parte unaria)

            // Almacenar bits bajos de mayor a menor peso (big-endian dentro del bloque)
            for (int j = ef.k - 1; j >= 0; j--) {
                ef.low.push_back((lo >> j) & 1);
            }

            // Codificación unaria de los bits altos:
            // hi ceros seguidos del terminador "10"
            for (long long int j = 0; j < hi; j++) {
                ef.high.push_back(false); // cero
            }
            ef.high.push_back(true);  // 1 (marca el valor)
            ef.high.push_back(false); // 0 (terminador)
        }

        return ef;
    }
}
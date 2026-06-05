#pragma once

#include <vector>
#include <algorithm>
#include "compresion.hpp"

/**
 * @file busqueda.hpp
 * @brief Algoritmos de búsqueda para los Casos 2 y 3.
 *
 * Implementa tres operaciones principales:
 *
 *   1. sample_range: Búsqueda binaria sobre el sample para acotar el rango [L, R].
 *   2. GC_decode:    Decodificación secuencial sobre Gap-Coding en el rango [L, R] (Caso 2).
 *   3. EF_decode:    Decodificación secuencial sobre Elias-Fano en el rango [L, R] (Caso 3).
 *
 * El flujo típico de búsqueda es:
 *   sample_range() → GC_decode() o EF_decode()
 */

namespace Busqueda {
    using namespace std;

    /**
     * @brief Acota el rango de búsqueda usando búsqueda binaria sobre el sample.
     *
     * El sample contiene cada b-ésimo elemento del arreglo original A.
     * Esta función encuentra el bloque del sample donde podría estar x,
     * y devuelve los índices originales L y R que delimitan ese bloque.
     *
     * Si x < sample[0], retorna false (x no puede estar en A).
     *
     * Ejemplo:
     *   A = [2, 5, 6, 8, 10, 12], b = 2, sample = [2, 6, 10]
     *   Buscar x = 8:
     *     upper_bound(sample, 8) apunta a 10 (índice 2 en sample)
     *     sample_pos = 1 → L = 1*2 = 2 (A[2] = 6), R = 2*2 = 4 (A[4] = 10)
     *
     * @tparam T Tipo entero
     * @param sample  Índice de muestreo del arreglo original
     * @param b       Salto entre elementos del sample (típicamente 1024)
     * @param x       Valor a buscar
     * @param n       Tamaño total del arreglo original
     * @param L       Índice izquierdo del rango (salida)
     * @param R       Índice derecho del rango (salida)
     * @return true si x puede estar en el rango [L, R]; false si x < sample[0]
     */
    template<typename T>
    bool sample_range(const vector<T> &sample, const int &b, const T &x, const int &n, int &L, int &R) {
        if (x < sample[0]) return false;

        // Encontrar el primer elemento del sample mayor que x
        auto it = upper_bound(sample.begin(), sample.end(), x);
        int sample_pos = it - sample.begin() - 1; // índice en el sample donde estaría x

        L = sample_pos * b; // posición original del elemento sample[sample_pos]

        // R es la posición del siguiente elemento del sample (o el final del arreglo)
        if (sample_pos + 1 < (int)sample.size()) R = (sample_pos + 1) * b;
        else R = n;

        return true;
    }

    /**
     * @brief Busca x decodificando secuencialmente el Gap-Coding en el rango [L, R].
     *
     * El Gap-Coding almacena diferencias consecutivas del arreglo original.
     * Para reconstruir A[i] a partir de GC, se acumula desde el último punto
     * conocido (que es sample[L/b] = A[L]):
     *
     *   A[L]   = sample[L/b]       (valor exacto, disponible sin decodificar)
     *   A[L+1] = A[L]   + GC[L+1]
     *   A[L+2] = A[L+1] + GC[L+2]
     *   ...
     *
     * La búsqueda recorre desde L hasta R comparando cada valor reconstruido con x.
     *
     * Complejidad: O(b) en el peor caso, donde b es el tamaño del bloque del sample.
     *
     * @tparam T Tipo entero
     * @param GC      Arreglo de Gap-Coding
     * @param sample  Índice de muestreo
     * @param x       Valor a buscar
     * @param b       Salto entre elementos del sample
     * @param L       Índice izquierdo del rango (obtenido de sample_range)
     * @param R       Índice derecho del rango (obtenido de sample_range)
     * @return true si x está en el rango [L, R]; false si no se encuentra
     */
    template <typename T>
    bool GC_decode(const vector<T> &GC, const vector<T> &sample, const T &x, const int &b, const int &L, const int &R) {
        T curr = sample[L / b]; // A[L] = sample[L/b] (punto de partida conocido)

        if (curr == x) return true;

        // Reconstruir A[i] acumulando los gaps a partir de L
        for (int i = L + 1; i <= R; ++i) {
            curr = curr + GC[i]; // A[i] = A[i-1] + GC[i]
            if (curr == x) return true;
        }

        return false;
    }

    /**
     * @brief Busca x decodificando secuencialmente la codificación Elias-Fano en [L, R].
     *
     * La decodificación tiene dos partes para cada elemento i en [L, R]:
     *
     *   1. Bits bajos (low):  Se leen k bits consecutivos desde ef.low[i*k] para obtener lo.
     *   2. Bits altos (high): Se recorre ef.high usando un puntero. Cada "10" en secuencia
     *      representa un elemento; los ceros antes del "1" codifican en unario el valor hi.
     *
     * El valor reconstruido es: curr = hi * 2^k + lo
     *
     * La búsqueda salta primero hasta la posición L en ef.high antes de empezar a comparar.
     * Gracias a que el arreglo está ordenado, si curr > x se puede detener anticipadamente.
     *
     * Complejidad: O(b) en el rango, con costo adicional de O(hi_max) para avanzar en high.
     *
     * @param ef  Estructura Elias-Fano del arreglo
     * @param x   Valor a buscar
     * @param L   Índice izquierdo del rango
     * @param R   Índice derecho del rango
     * @return true si x está en [L, R]; false si no se encuentra
     */
    template <typename T>
    bool EF_decode(const Compresion::EliasFano& ef, const T &x, const int &L, const int &R) {
        int d = 1 << ef.k; // d = 2^k (divisor para reconstruir: curr = hi*d + lo)
        int high_ptr = 0;   // puntero sobre ef.high
        int current_index = 0; // índice lógico del elemento actual en ef.high

        // Paso 1: Saltar en ef.high hasta llegar al elemento L
        // Cada elemento ocupa exactamente un patrón "10" (más los ceros previos del unario)
        while (current_index < L && high_ptr + 1 < (int)ef.high.size()) {
            if (ef.high[high_ptr] == true && ef.high[high_ptr + 1] == false) {
                // Encontramos el terminador "10" de un elemento → avanzar al siguiente
                current_index++;
                high_ptr += 2;
            } else {
                high_ptr++;
            }
        }

        // Paso 2: Decodificar elementos desde L hasta R y comparar con x
        for (int i = L; i <= R; ++i) {
            // Reconstruir bits bajos: leer k bits desde ef.low[i*k]
            T lo = 0;
            int low_start = i * ef.k;
            for (int j = 0; j < ef.k; ++j) {
                lo = (lo << 1) | (ef.low[low_start + j] ? 1 : 0);
            }

            // Reconstruir bits altos: contar ceros antes del siguiente "10"
            T hi = 0;
            while (high_ptr + 1 < (int)ef.high.size()) {
                if (ef.high[high_ptr] == true && ef.high[high_ptr + 1] == false) {
                    // Terminador "10" → fin del valor alto, avanzar puntero
                    high_ptr += 2;
                    break;
                }
                if (ef.high[high_ptr] == false) {
                    hi++; // cada cero suma 1 al valor alto
                }
                high_ptr++;
            }

            // Combinar partes para obtener el valor original
            T curr = (hi * d) + lo;

            if (curr == x) return true;
            if (curr > x) return false; // secuencia ordenada: no puede estar más adelante
        }
        return false;
    }
}
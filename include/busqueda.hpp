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
 *   1. sample_range: Búsqueda binaria sobre el sample para acotar el rango [L, R).
 *   2. GC_decode:    Decodificación secuencial sobre Gap-Coding en el rango [L, R) (Caso 2).
 *   3. EF_decode:    Decodificación secuencial sobre Elias-Fano en el rango [L, R) (Caso 3).
 *
 * CONVENCIÓN DE RANGO: R es EXCLUSIVO en todas las funciones.
 * Es decir, el rango válido de índices es [L, R-1], y los loops
 * van hasta i < R (no i <= R) para evitar accesos fuera de rango.
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
     * y devuelve los índices originales L (inclusivo) y R (exclusivo).
     *
     * Casos de retorno false (x definitivamente no está en A):
     *   - x < sample[0]: menor que el mínimo del arreglo.
     *   - x > sample.back(): mayor que el máximo del arreglo (FIX: acción 2 y 5).
     *
     * Ejemplo:
     *   A = [2, 5, 6, 8, 10, 12], b = 2, sample = [2, 6, 10], n = 6
     *   Buscar x = 8:
     *     upper_bound(sample, 8) apunta a 10 (índice 2 en sample)
     *     sample_pos = 1 → L = 2, R = min(4, 6) = 4
     *     Rango válido de índices: [2, 3]  (R=4 es exclusivo)
     *
     * @tparam T Tipo entero
     * @param sample  Índice de muestreo del arreglo original
     * @param b       Salto entre elementos del sample (típicamente 1024)
     * @param x       Valor a buscar
     * @param n       Tamaño total del arreglo original
     * @param L       Índice izquierdo del rango, inclusivo (salida)
     * @param R       Índice derecho del rango, EXCLUSIVO (salida)
     * @return true si x puede estar en el rango [L, R); false si x no puede estar en A
     */
    template<typename T>
    bool sample_range(const vector<T> &sample, const int &b, const T &x, const int &n, int &L, int &R) {
        // x menor que el mínimo del arreglo
        if (x < sample[0]) return false;

        // Encontrar el primer elemento del sample estrictamente mayor que x
        auto it = upper_bound(sample.begin(), sample.end(), x);
        int sample_pos = it - sample.begin() - 1; // bloque del sample donde estaría x

        L = sample_pos * b; // índice original del elemento sample[sample_pos]

        // R es exclusivo: apunta al inicio del SIGUIENTE bloque (o al final del arreglo)
        // FIX (acción 2): R nunca excede n, y el loop en GC/EF debe ser i < R
        if (sample_pos + 1 < (int)sample.size())
            R = min((sample_pos + 1) * b, n); // clamp adicional por seguridad
        else
            R = n;

        return true;
    }

    /**
     * @brief Busca x decodificando secuencialmente el Gap-Coding en el rango [L, R).
     *
     * Reconstruye A[i] desde A[L] = sample[L/b] acumulando gaps:
     *   A[L]   = sample[L/b]
     *   A[i]   = A[i-1] + GC[i]   para i en (L, R)
     *
     * El rango es [L, R) EXCLUSIVO, es decir el loop va hasta i < R.
     * Esto garantiza que GC[i] con i = R-1 siempre sea un índice válido (< n).
     *
     * FIX (acción 2): el loop original usaba i <= R, que causaba acceso a GC[n]
     * cuando R = n (out-of-bounds / comportamiento indefinido).
     *
     * Complejidad: O(b) en el peor caso.
     *
     * @tparam T Tipo entero
     * @param GC      Arreglo de Gap-Coding (tamaño n)
     * @param sample  Índice de muestreo
     * @param x       Valor a buscar
     * @param b       Salto entre elementos del sample
     * @param L       Índice izquierdo del rango (inclusivo)
     * @param R       Índice derecho del rango (EXCLUSIVO)
     * @return true si x está en el rango [L, R); false si no se encuentra
     */
    template <typename T>
    bool GC_decode(const vector<T> &GC, const vector<T> &sample, const T &x, const int &b, const int &L, const int &R) {
        T curr = sample[L / b]; // A[L] = sample[L/b] (punto de partida conocido)

        if (curr == x) return true;

        // FIX (acción 2): i < R (exclusivo) en vez de i <= R (que accedía a GC[n])
        for (int i = L + 1; i < R; ++i) {
            curr = curr + GC[i]; // A[i] = A[i-1] + GC[i]
            if (curr == x) return true;
        }

        return false;
    }

    /**
     * @brief Busca x decodificando secuencialmente la codificación Elias-Fano en [L, R).
     *
     * El rango es [L, R) EXCLUSIVO: el loop decodifica elementos L, L+1, ..., R-1.
     *
     * Decodificación por elemento i:
     *   1. Bits bajos:  leer k bits desde ef.low[i*k] → valor lo
     *   2. Bits altos:  avanzar puntero en ef.high contando ceros → valor hi
     *   3. Reconstruir: curr = hi * 2^k + lo
     *
     * Optimización: si curr > x se detiene anticipadamente (secuencia ordenada).
     *
     * FIX (acción 2): el loop usa i < R (exclusivo) para consistencia con GC_decode
     * y con la convención de rango de sample_range.
     *
     * @param ef  Estructura Elias-Fano del arreglo
     * @param x   Valor a buscar
     * @param L   Índice izquierdo del rango (inclusivo)
     * @param R   Índice derecho del rango (EXCLUSIVO)
     * @return true si x está en [L, R); false si no se encuentra
     */
    template <typename T>
    bool EF_decode(const Compresion::EliasFano& ef, const T &x, const int &L, const int &R) {
        int d = 1 << ef.k;  // d = 2^k
        int high_ptr = 0;   // puntero sobre ef.high
        int current_index = 0; // índice lógico actual al recorrer ef.high

        // Paso 1: Avanzar en ef.high hasta el elemento L
        // Cada elemento ocupa un patrón "10" (más los ceros previos del unario)
        while (current_index < L && high_ptr + 1 < (int)ef.high.size()) {
            if (ef.high[high_ptr] == true && ef.high[high_ptr + 1] == false) {
                current_index++;
                high_ptr += 2;
            } else {
                high_ptr++;
            }
        }

        // Paso 2: Decodificar elementos L .. R-1 (R exclusivo)
        // FIX (acción 2): i < R en vez de i <= R
        for (int i = L; i < R; ++i) {
            // Reconstruir bits bajos: k bits desde ef.low[i*k]
            T lo = 0;
            int low_start = i * ef.k;
            for (int j = 0; j < ef.k; ++j) {
                lo = (lo << 1) | (ef.low[low_start + j] ? 1 : 0);
            }

            // Reconstruir bits altos: contar ceros antes del siguiente "10"
            T hi = 0;
            while (high_ptr + 1 < (int)ef.high.size()) {
                if (ef.high[high_ptr] == true && ef.high[high_ptr + 1] == false) {
                    high_ptr += 2;
                    break;
                }
                if (ef.high[high_ptr] == false) {
                    hi++;
                }
                high_ptr++;
            }

            T curr = (hi * static_cast<T>(d)) + lo;

            if (curr == x) return true;
            if (curr > x)  return false; // secuencia ordenada: no puede estar más adelante
        }
        return false;
    }
}
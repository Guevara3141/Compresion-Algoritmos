#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>

/**
 * @file generar.hpp
 * @brief Funciones de generación de vectores para los tres casos del proyecto.
 *
 * Contiene utilidades para crear arreglos ordenados con distribución exponencial
 * (lineal) y gaussiana aproximada, junto con las estructuras auxiliares de
 * Gap-Coding y Sample usadas en los Casos 2 y 3.
 *
 * NOTA: Todas las distribuciones generan valores no-negativos (int32_t >= 0)
 * para compatibilidad con la codificación Elias-Fano, que opera sobre enteros
 * no negativos. El rango válido es [0, 2^31 - 1].
 */

namespace Generar {
    using namespace std;

    // Generadores de números aleatorios globales al namespace (inicializados una sola vez)
    static random_device rd;
    static mt19937 generador(rd());

    /**
     * @brief Genera un vector de n elementos aleatorios uniformes (no negativos) y lo ordena.
     *
     * Los valores se distribuyen uniformemente en [0, INT32_MAX],
     * luego se ordenan con std::sort.
     *
     * @tparam T Tipo entero (ej. int32_t)
     * @param v Vector de salida (se reemplaza su contenido)
     * @param n Cantidad de elementos a generar
     */
    template<typename T>
    void vectorUniforme(vector<T> &v, const int &n) {
        v.clear();
        v.reserve(n);

        // Rango [0, max] para garantizar valores no-negativos (requerido por Elias-Fano)
        uniform_int_distribution<T> distribucion(0, numeric_limits<T>::max());

        for (int i = 0; i < n; ++i)
            v.push_back(distribucion(generador));

        sort(v.begin(), v.end());
    }

    /**
     * @brief Genera un vector de n elementos ordenado con distribución exponencial.
     *
     * Cada elemento se construye sumando un incremento aleatorio con distribución
     * exponencial de parámetro lambda al acumulador anterior:
     *   curr += Exponencial(lambda)
     *
     * Con lambda = 0.05, el incremento promedio es 1/lambda = 20, produciendo
     * una secuencia estrictamente creciente de enteros no-negativos.
     *
     * @tparam T Tipo del vector de salida (int32_t, etc.)
     * @param v Vector de salida
     * @param n Cantidad de elementos
     */
    template<typename T>
    void vectorUniformeSorted(vector<T> &v, const int &n) {
        v.clear();
        v.reserve(n);

        double lambda = 0.05;
        exponential_distribution<double> distribucion(lambda);
        double curr_val = 0.0;

        // Valor máximo del tipo T para prevenir overflow en el cast
        const double t_max = static_cast<double>(numeric_limits<T>::max());

        for (int i = 0; i < n; ++i) {
            curr_val += distribucion(generador);

            // Clamp para prevenir overflow al castear a T (FIX: acción 7)
            double clamped = min(curr_val, t_max);

            if constexpr (is_integral_v<T>)
                v.push_back(static_cast<T>(round(clamped)));
            else
                v.push_back(static_cast<T>(clamped));
        }
    }

    /**
     * @brief Genera un vector de n elementos con distribución normal y lo ordena.
     *
     * Los valores se sampean de N(offset, dev) donde offset es un valor positivo
     * grande para garantizar que todos los valores sean no-negativos, compatible
     * con Elias-Fano.
     *
     * @tparam T Tipo del vector (int32_t, etc.)
     * @param v Vector de salida
     * @param n Cantidad de elementos
     * @param dev Desviación estándar de la distribución normal
     */
    template<typename T>
    void vectorGauss(vector<T> &v, const int &n, const double &dev) {
        v.clear();
        v.reserve(n);

        // Offset positivo para garantizar valores >= 0 (requerido por Elias-Fano)
        // Se usa INT32_MAX/2 como centro para dejar espacio a ambos lados
        const double offset = static_cast<double>(numeric_limits<int32_t>::max()) / 2.0;
        const double t_max  = static_cast<double>(numeric_limits<T>::max());
        const double t_min  = 0.0; // Elias-Fano requiere no-negativos

        normal_distribution<double> distribucion(offset, dev);

        for (int i = 0; i < n; ++i) {
            double val = distribucion(generador);

            // Clamp para prevenir overflow (FIX: acción 7)
            val = clamp(val, t_min, t_max);

            if constexpr (is_integral_v<T>)
                v.push_back(static_cast<T>(round(val)));
            else
                v.push_back(static_cast<T>(val));
        }

        sort(v.begin(), v.end());
    }

    /**
     * @brief Genera un vector de n elementos ya ordenado con forma gaussiana,
     *        centrado en INT32_MAX/2 para garantizar valores no-negativos.
     *
     * Construye los valores directamente en orden creciente usando la función
     * cuantil de la normal (inversa de la CDF). El proceso es:
     *   1. Se acumula probabilidad p usando pasos pequeños con jitter aleatorio.
     *   2. Para cada p, se calcula el cuantil y se escala con dev y offset.
     *   3. Los valores se clampean a [0, T_MAX] para prevenir overflow.
     *
     * El offset (INT32_MAX/2) garantiza que todos los valores sean no-negativos,
     * requisito de la codificación Elias-Fano (FIX: acciones 3 y 7).
     *
     * @tparam T Tipo del vector
     * @param v Vector de salida
     * @param n Cantidad de elementos
     * @param dev Desviación estándar aproximada
     */
    template<typename T>
    void vectorGaussSorted(vector<T> &v, const int &n, const double &dev) {
        v.clear();
        v.reserve(n);

        const double pi     = acos(-1.0);
        // Offset positivo: centra la gaussiana en la mitad del rango positivo de int32_t
        // Esto garantiza que valores hasta ~3*dev sigan siendo no-negativos
        const double offset = static_cast<double>(numeric_limits<int32_t>::max()) / 2.0;
        const double t_max  = static_cast<double>(numeric_limits<T>::max());
        const double t_min  = 0.0;

        uniform_real_distribution<double> jitter(0.5, 1.5);
        double p_accumulator = 0.0;
        double base_step = 1.0 / (n + 100.0);

        for (int i = 0; i < n; ++i) {
            p_accumulator += base_step * jitter(generador);
            double p = clamp(p_accumulator, 1e-9, 1.0 - 1e-9);

            double r   = sqrt(-2.0 * log(min(p, 1.0 - p)));
            double val = r * cos(pi * (1.0 - p));
            val = val * dev + offset; // escalar con dev y centrar en offset positivo

            // Clamp estricto para prevenir overflow al castear (FIX: acción 7)
            val = clamp(val, t_min, t_max);

            if constexpr (is_integral_v<T>)
                v.push_back(static_cast<T>(round(val)));
            else
                v.push_back(static_cast<T>(val));
        }
    }

    /**
     * @brief Construye el arreglo de Gap-Coding a partir de un arreglo ordenado.
     *
     * gap[0] = v[0]
     * gap[i] = v[i] - v[i-1]   para i > 0
     *
     * Ejemplo: v = [2, 7, 10, 12] → gap = [2, 5, 3, 2]
     *
     * @tparam T Tipo entero
     * @param v   Arreglo ordenado original (entrada)
     * @param gap Arreglo de gaps resultante (salida)
     * @param n   Cantidad de elementos
     */
    template<typename T>
    void vectorGap(const vector<T> &v, vector<T> &gap, const int &n) {
        gap.clear();
        gap.reserve(n);
        gap.push_back(v[0]);

        for (int i = 1; i < n; ++i)
            gap.push_back(v[i] - v[i - 1]);
    }

    /**
     * @brief Construye el índice de muestreo (sample) del arreglo original.
     *
     * Almacena cada b-ésimo elemento del arreglo v (con b = 1024).
     * Tamaño resultante: ceil(n / b) ≈ n/1024 elementos.
     *
     * @tparam T Tipo entero
     * @param v      Arreglo original ordenado (entrada)
     * @param sample Índice de muestreo resultante (salida)
     * @param n      Cantidad de elementos en v
     */
    template<typename T>
    void vectorSample(const vector<T> &v, vector<T> &sample, const int &n) {
        sample.clear();
        int b = 1024; // salto estático (debe coincidir con busqueda.hpp y cases.hpp)
        sample.reserve(n / b + 1);
        int i = 0;
        while (i < (int)v.size()) {
            sample.push_back(v[i]);
            i += b;
        }
    }

}
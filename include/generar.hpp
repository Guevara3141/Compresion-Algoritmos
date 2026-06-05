#pragma once

#include <vector>
#include <algorithm>
#include <random>

/**
 *Funciones de generación de vectores para los tres casos del proyecto.
 *
 * Contiene utilidades para crear arreglos ordenados con distribución exponencial
 * (lineal) y gaussiana aproximada, junto con las estructuras auxiliares de
 * Gap-Coding y Sample usadas en los Casos 2 y 3.
 *

 */

namespace Generar {
    using namespace std;

    // Generadores de números aleatorios globales al namespace (inicializados una sola vez)
    static random_device rd;
    static mt19937 generador(rd());

    /**
     * Genera un vector de n elementos aleatorios uniformes y lo ordena.
     *
     * Los valores se distribuyen uniformemente en todo el rango del tipo T,
     * luego se ordenan con std::sort. A diferencia de vectorUniformeSorted,
     * esta versión no garantiza incrementos mínimos entre elementos (puede
     * haber duplicados).
     *
     * @tparam T Tipo entero (ej. int32_t)
     * @param v Vector de salida (se reemplaza su contenido)
     * @param n Cantidad de elementos a generar
     */
    template<typename T>
    void vectorUniforme(vector<T> &v, const int &n) {
        v.clear();
        v.reserve(n);

        uniform_int_distribution<T> distribucion(
            numeric_limits<T>::min(),
            numeric_limits<T>::max()
        );

        for (int i = 0; i < n; ++i)
            v.push_back(distribucion(generador));

        sort(v.begin(), v.end());
    }

    /**
     * @brief Genera un vector de n elementos ordenado con distribución exponencial.
     *
     * Cada elemento se construye sumando un incremento aleatorio con distribución
     * exponencial de parámetro lambda al acumulador anterior:
     *
     *   curr += Exponencial(lambda)
     *
     * Con lambda = 0.05, el incremento promedio entre elementos consecutivos es
     * 1/lambda = 20. Esto produce una secuencia estrictamente creciente sin
     * duplicados, representativa de datos tipo "timestamp" o "ID incremental".
     *
     * Es la distribución usada para el Caso 1 (lineal) en el benchmark.
     *
     * @tparam T Tipo del vector de salida (int32_t, float, etc.)
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

        for (int i = 0; i < n; ++i) {
            curr_val += distribucion(generador);

            // Para tipos enteros, redondear al entero más cercano
            if constexpr (is_integral_v<T>)
                v.push_back(static_cast<T>(round(curr_val)));
            else
                v.push_back(static_cast<T>(curr_val));
        }
    }

    /**
     * @brief Genera un vector de n elementos con distribución normal y lo ordena.
     *
     * Los valores se sampean de N(0, dev) y luego se ordenan con std::sort.
     * Útil para generar datos con alta concentración alrededor del centro.
     *
     * Nota: al usar sort después de samplear, la secuencia resultante es
     * genuinamente gaussiana pero no tiene incrementos crecientes controlados.
     * Para una versión que garantice orden sin sort, ver vectorGaussSorted.
     *
     * @tparam T Tipo del vector (int32_t, double, etc.)
     * @param v Vector de salida
     * @param n Cantidad de elementos
     * @param dev Desviación estándar de la distribución normal
     */
    template<typename T>
    void vectorGauss(vector<T> &v, const int &n, const double &dev) {
        v.clear();
        v.reserve(n);

        normal_distribution<double> distribucion(0, dev);
        for (int i = 0; i < n; ++i) {
            double val = distribucion(generador);

            if constexpr (is_integral_v<T>)
                v.push_back(static_cast<T>(round(val)));
            else
                v.push_back(static_cast<T>(val));
        }

        sort(v.begin(), v.end());
    }

    /**
     * @brief Genera un vector de n elementos ya ordenado con forma aproximadamente gaussiana,
     *        sin usar sort explícito.
     *
     * En lugar de samplear N(0, dev) y ordenar, esta función construye los valores
     * directamente en orden creciente usando la función cuantil de la normal (inversa
     * de la CDF). El proceso es:
     *
     *   1. Se acumula un probabilidad p usando pasos pequeños con jitter aleatorio,
     *      para simular puntos de cuantil igualmente espaciados con algo de ruido.
     *   2. Para cada p, se calcula el valor del cuantil usando la aproximación:
     *        r = sqrt(-2 * log(min(p, 1-p)))
     *        val = r * cos(π*(1-p)) * dev
     *   3. Los valores resultantes están ya en orden creciente por construcción.
     *
     * El jitter en el paso de probabilidad evita que los elementos queden
     * perfectamente equiespaciados en cuantil, dando una distribución más natural.
     *
     * Advertencia: la aproximación no es exactamente N(0,dev) pero tiene la misma
     * forma acampanada y concentración central. Suficiente para los experimentos
     * del proyecto.
     *
     * Es la función usada para el Caso Normal en el benchmark (evita el costo de sort).
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

        const double pi = acos(-1.0);
        const double mean = 0.0;
        uniform_real_distribution<double> jitter(0.5, 1.5); // ruido sobre el paso de cuantil
        double p_accumulator = 0.0;
        double base_step = 1.0 / (n + 100.0); // paso base para recorrer [0, 1]

        for (int i = 0; i < n; ++i) {
            p_accumulator += base_step * jitter(generador); // avanzar con jitter
            double p = clamp(p_accumulator, 1e-9, 1 - 1e-9); // evitar log(0)

            // Aproximación de la inversa de la CDF normal usando la fórmula de Box-Muller parcial
            double r = sqrt(-2 * log(min(p, 1 - p)));
            double val = r * cos(pi * (1 - p));
            val = val * dev + mean;

            if constexpr (is_integral_v<T>)
                v.push_back(static_cast<T>(round(val)));
            else
                v.push_back(static_cast<T>(val));
        }
    }

    /**
     * @brief Construye el arreglo de Gap-Coding a partir de un arreglo ordenado.
     *
     * Representa cada elemento como la diferencia respecto al anterior:
     *   gap[0] = v[0]
     *   gap[i] = v[i] - v[i-1]   para i > 0
     *
     * Ejemplo: v = [2, 7, 10, 12] → gap = [2, 5, 3, 2]
     *
     * Con distribuciones de incremento pequeño (como vectorUniformeSorted con
     * lambda=0.05), los gaps son en promedio ~20, lo que permite comprimirlos
     * en muchos menos bits que los valores originales de 32 bits.
     *
     * @tparam T Tipo entero
     * @param v Arreglo ordenado original (entrada)
     * @param gap Arreglo de gaps resultante (salida)
     * @param n Cantidad de elementos
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
     * El sample permite búsqueda binaria para acotar rápidamente el rango [L, R]
     * antes de decodificar secuencialmente sobre GC o Elias-Fano.
     *
     * Ejemplo: v = [2, 7, 10, 12, 15, 18, ...], b = 2 → sample = [2, 10, 15, ...]
     *
     * Tamaño resultante: ceil(n / b) ≈ n/1024 elementos.
     * Para n = 10^7 → sample tiene ~9766 entradas.
     *
     * @tparam T Tipo entero
     * @param v Arreglo original ordenado (entrada)
     * @param sample Índice de muestreo resultante (salida)
     * @param n Cantidad de elementos en v
     */
    template<typename T>
    void vectorSample(const vector<T> &v, vector<T> &sample, const int &n) {
        sample.clear();
        sample.reserve(n);
        int b = 1024; // salto estático (debe coincidir con busqueda.hpp y cases.hpp)
        int i = 0;
        while (i < (int)v.size()) {
            sample.push_back(v[i]);
            i += b;
        }
    }

}
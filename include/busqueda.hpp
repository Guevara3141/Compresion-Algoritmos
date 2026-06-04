#pragma once

#include <vector>
#include <algorithm>
#include "compresion.hpp"


namespace Busqueda {
    using namespace std;

    template<typename T>
    bool sample_range(const vector<T> &sample, const int &b, const T &x, const int &n, int &L, int &R) {
        if (x < sample[0]) return false;
        auto it = upper_bound(sample.begin(), sample.end(), x);
        int sample_pos = it - sample.begin() - 1;

        L = sample_pos * b;

        if (sample_pos + 1 < (int)sample.size()) R = (sample_pos+1) * b;
        else R = n;

        return true;
    }



    /*
    retorna posiciones en el arreglo original
    Index_original = sample_index * b
    b=salto

    A={2, 5, 6, 8, 10, 12}
    b=2, sample={2, 6, 10}
    x=8
    L=6{sample_idx=1}, R=10{sample_idx=2}
    L = 1*b = 2 (A[2] = 6)
    R = 2*b = 4 (A[4] = 10)
    */

    template <typename T>
    bool GC_decode(const vector<T> &GC, const vector<T> &sample, const T &x, const int &b, const int &L, const int &R) {
        T curr = sample[L/b]; // A[L] = sample[L/b]
        if (curr == x) return true;

        for (int i=L+1; i <= R; ++i) {
            curr = curr + GC[i];    
        /*
        como sample[L/b] = A[L], y GC[i] = A[i] - A[i-1]
        entonces A[i] = GC[i] + A[i-1]
        por lo tanto curr = A[L] = A[i-1]
        */
            if (curr == x) return true;
        }

        return false;
    }

    template <typename T>
    bool EF_decode(const Compresion::EliasFano& ef, const T &x, const int &L, const int &R) {
        int d = 1 << ef.k;
        int high_ptr = 0;
        int current_index = 0;

        // 1. Saltar hasta el elemento L en la codificación unaria
        while (current_index < L && high_ptr + 1 < (int)ef.high.size()) {
            if (ef.high[high_ptr] == true && ef.high[high_ptr+1] == false) {
                current_index++;
                high_ptr += 2;
            } else {
                high_ptr++;
            }
        }

        // 2. Buscar reconstruyendo valores desde L hasta R
        for (int i = L; i <= R; ++i) {
            // Reconstruir low
            T lo = 0;
            int low_start = i * ef.k;
            for (int j = 0; j < ef.k; ++j) {
                lo = (lo << 1) | (ef.low[low_start + j] ? 1 : 0);
            }

            // Reconstruir high leyendo la cantidad de ceros antes del siguiente "10"
            T hi = 0;
            while (high_ptr + 1 < (int)ef.high.size()) {
                if (ef.high[high_ptr] == true && ef.high[high_ptr+1] == false) {
                    high_ptr += 2;
                    break;
                }
                if (ef.high[high_ptr] == false) {
                    hi++;
                }
                high_ptr++;
            }

            // Combinar y evaluar
            T curr = (hi * d) + lo;
            if (curr == x) return true;
            if (curr > x) return false; // Optimización: la secuencia es monótona
        }
        return false;
    }
}
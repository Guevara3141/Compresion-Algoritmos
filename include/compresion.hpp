#pragma once

#include <vector>
#include <cmath>

namespace Compresion {
    using namespace std;

    // estructura para almacenar la codificación de Elias-Fano con bools
    struct EliasFano {
        int k;
        vector<bool> low;
        vector<bool> high;
    };

    template <typename T>
    EliasFano eliasFanoCodif(const vector<T> &A) {
        EliasFano ef;
        long long int n = (int)A.size();

        //k = log2(n) redondeado hacia abajo
        ef.k = (int)log2(n); 

        int d = 1 << ef.k; 

        for (size_t i = 0; i < A.size(); i++) {
            T v = A[i];
            int lo = v % d;
            int hi = v / d;

            // Guardar los k bits bajos
            for (int j = ef.k - 1; j >= 0; j--) {
                ef.low.push_back((lo >> j) & 1);
            }

            // Guardar bits altos en unario (hi ceros seguidos de 10)
            for (int j = 0; j < hi; j++) {
                ef.high.push_back(false); // agrega 0s
            }
            ef.high.push_back(true);  // 1
            ef.high.push_back(false); // 0
        }

        return ef;
    } 
}
#pragma once

#include <vector>

namespace Busqueda {
    using namespace std;

    template<typename T>
    bool sample_range(vector<T> &sample, int b, T x, int n, int &L, int &R) {
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
    bool GC_decode(vector<T> &GC, vector<T> &sample, T x, int b, int L, int R) {
        T curr = sample[L/b]; // A[L] = sample[L/b]
        cout << "curr: " << curr << endl;
        if (curr == x) return true;

        for (int i=L+1; i <= R; ++i) {
            curr = curr + GC[i];    
        /*
        como sample[L/b] = A[L], y GC[i] = A[i] - A[i-1]
        entonces A[i] = GC[i] + A[i-1]
        por lo tanto curr = A[L] = A[i-1]
        */
            cout << "i: " << i << "     curr: " << curr << endl;
            if (curr == x) return true;
            
        }

        return false;
    }
}
#pragma once

#include <vector>

namespace Busqueda {
    using namespace std;

    template<typename T>
    pair<int, int> sample_range(vector<T> &sample, int b, T x) {
        auto it = lower_bound(sample.begin(), sample.end());
        int sample_pos = it - sample.begin();

        int L_idx, R_idx;
        if (sample_pos == 0) L_idx = sample_pos;
        else L_idx = (sample_pos-1) * b;

        if (it == sample.end()) R_idx = sample.size();
        else R_idx = sample_pos * b;

        return {L_idx, R_idx};
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

}
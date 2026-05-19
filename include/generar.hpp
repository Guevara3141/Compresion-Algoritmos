#pragma once

#include <bits/stdc++.h>

namespace Generar {
    using namespace std;

    static random_device rd;
    static mt19937 generador(rd());

    template<typename T>
    vector<T> vectorUniforme(int n) {
        vector<T> v;
                v.reserve(n);

        uniform_int_distribution<T> distribucion(
            numeric_limits<T>::min(),
            numeric_limits<T>::max()
        );

        for (int i = 0; i < n; ++i)
            v.push_back(distribucion(generador));

        return v;
    }

}

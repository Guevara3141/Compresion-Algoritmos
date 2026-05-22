#pragma once

#include <vector>
#include <random>

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

    template<typename T>
    vector<T> vectorGap(vector<T> v, int n) {
        vector<T> gap(n);
        gap[0] = v[0];

        for (int i = 1; i < n; ++i) 
            gap[i] = v[i] - v[i-1];

        return gap;
    }

    template<typename T>
    vector<T> vectorSample(vector<T> v, int n) {
        vector<T> sample;
        sample.reserve(n);
        int b = 1024; //1024 or 4096 or 8192
        int i=0;
        while (i < (int)v.size()) {
            sample.push_back(v[i]);
            i+=b;
        }

        return sample;
    }

}


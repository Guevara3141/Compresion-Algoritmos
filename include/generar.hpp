#pragma once

#include <vector>
#include <algorithm>
#include <random>

namespace Generar {
    using namespace std;

    static random_device rd;
    static mt19937 generador(rd());

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

    template<typename T>
    void vectorUniformeSorted(vector<T> &v, const int &n) {
        v.clear();
        v.reserve(n);

        double lambda = 0.05;
        exponential_distribution<double> distribucion(lambda);
        double curr_val = 0.0;

        for (int i=0; i<n; ++i) {
            curr_val += distribucion(generador);
            
            if constexpr (is_integral_v<T>) 
                v.push_back(static_cast<T>(round(curr_val)));
            else
                v.push_back(static_cast<T>(curr_val));
        }
    }

    template<typename T>
    void vectorGauss(vector<T> &v, const int &n, const double &dev) {
        v.clear();
        v.reserve(n);

        normal_distribution<double> distribucion(0, dev);
        for (int i=0; i<n; ++i) {   
            double val = distribucion(generador);

            //types like int32 will be rounded up
            if constexpr (is_integral_v<T>) 
                v.push_back(static_cast<T>(round(val)));
            else
                v.push_back(static_cast<T>(val));
        }

        sort(v.begin(), v.end());
    }


    //no esta perfectamente distribuida como la funcion original, pero si se amontona todo al medio, muy parecido a gauss original
    template<typename T>
    void vectorGaussSorted(vector<T> &v, const int &n, const double &dev) {
        v.clear();
        v.reserve(n);
        const double pi = acos(-1.0);
        const double mean = 0.0;
        uniform_real_distribution<double>jitter(0.5, 1.5);
        double p_accumulator=0.0;
        double base_step=1.0/(n+100.0);

        for (int i=0; i<n; ++i) {   
            p_accumulator += base_step*jitter(generador);
            double p = clamp(p_accumulator, 1e-9, 1 - 1e-9);

            double r = sqrt(-2 * log(min(p, 1-p)));
            double val = r * cos(pi*(1-p));
            val = val*dev + mean;
            //types like int32 will be rounded up
            if constexpr (is_integral_v<T>) 
                v.push_back(static_cast<T>(round(val)));
            else
                v.push_back(static_cast<T>(val));
        }
    }


    template<typename T>
    void vectorGap(const vector<T> &v, vector<T> &gap, const int &n) {
        gap.clear();
        gap.reserve(n);
        gap.push_back(v[0]);

        for (int i = 1; i < n; ++i)
            gap.push_back(v[i] - v[i-1]); 
    }

    template<typename T>
    void vectorSample(const vector<T> &v, vector<T> &sample, const int &n){ 
        sample.clear();
        sample.reserve(n);
        int b = 1024; //1024 or 4096 or 8192
        int i=0;
        while (i < (int)v.size()) {
            sample.push_back(v[i]);
            i+=b;
        }
    }

}
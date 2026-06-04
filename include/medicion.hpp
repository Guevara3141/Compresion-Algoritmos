#pragma once
#include <cstring>
#include <vector>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include "generar.hpp"
#include "compresion.hpp"
#include "busqueda.hpp"

namespace Medicion {
    using namespace std;

    static random_device rd;
    static mt19937 gen(rd());

    template <typename T>
    void medicion(const vector<int> &sizes, const vector<double> &deviations, const vector<int> &searchSize) {

        if (filesystem::exists("mediciones.csv")) filesystem::remove("mediciones.csv");
        ofstream csvFile("mediciones.csv");
        
        if (csvFile.is_open()) {
            csvFile << "Size,Distribution,Deviation,Case,ConstructionTime_ms,SearchAmount,SearchTime_ms,TotalBytes\n";

            int b = 1024; // Salto estático para el sample

            for (int size : sizes) {
                println("Procesando size: {}...", size);

                // ==========================================
                // 1. DISTRIBUCIÓN LINEAL
                // ==========================================
                vector<T> vec, gap, sample;
                
                // Construcción Caso 1 (Lineal)
                auto t1 = chrono::high_resolution_clock::now();
                Generar::vectorUniformeSorted<T>(vec, size);
                auto t2 = chrono::high_resolution_clock::now();
                auto const_c1 = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                size_t bytes_c1 = vec.size() * sizeof(T);

                // Construcción Caso 2 (Gap + Sample)
                t1 = chrono::high_resolution_clock::now();
                Generar::vectorGap(vec, gap, size);
                Generar::vectorSample(vec, sample, size);
                t2 = chrono::high_resolution_clock::now();
                auto const_c2 = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                size_t bytes_c2 = gap.size() * sizeof(T) + sample.size() * sizeof(T);

                // Construcción Caso 3 (Elias-Fano)
                t1 = chrono::high_resolution_clock::now();
                Compresion::EliasFano ef = Compresion::eliasFanoCodif(vec);
                t2 = chrono::high_resolution_clock::now();
                auto const_c3 = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                // Cálculo de bits a bytes para Elias-Fano (vector<bool> usa 1 bit por elemento real) + Sample
                size_t bytes_c3 = (ef.low.size() + ef.high.size()) / 8 + sample.size() * sizeof(T);

                for (int search : searchSize) {
                    // Generar targets aleatorios reales que existan en el arreglo (para simular hits)
                    uniform_int_distribution<int> dist_idx(0, size - 1);
                    vector<T> targets(search);
                    for(int i=0; i<search; ++i) targets[i] = vec[dist_idx(gen)];

                    // Búsquedas Caso 1
                    auto ts1 = chrono::high_resolution_clock::now();
                    for (int i = 0; i < search; ++i) 
                        binary_search(vec.begin(), vec.end(), targets[i]);
                    auto ts2 = chrono::high_resolution_clock::now();
                    auto search_c1 = chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1).count();
                    csvFile << size << ",Lineal,0,Case1," << const_c1 << "," << search << "," << search_c1 << "," << bytes_c1 << "\n";

                    // Búsquedas Caso 2
                    ts1 = chrono::high_resolution_clock::now();
                    for (int i = 0; i < search; ++i) {
                        int L, R;
                        if (Busqueda::sample_range(sample, b, targets[i], size, L, R))
                            Busqueda::GC_decode(gap, sample, targets[i], b, L, R);
                    }
                    ts2 = chrono::high_resolution_clock::now();
                    auto search_c2 = chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1).count();
                    csvFile << size << ",Lineal,0,Case2," << const_c2 << "," << search << "," << search_c2 << "," << bytes_c2 << "\n";

                    // Búsquedas Caso 3
                    ts1 = chrono::high_resolution_clock::now();
                    for (int i = 0; i < search; ++i) {
                        int L, R;
                        if (Busqueda::sample_range(sample, b, targets[i], size, L, R))
                            Busqueda::EF_decode(ef, targets[i], L, R);
                    }
                    ts2 = chrono::high_resolution_clock::now();
                    auto search_c3 = chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1).count();
                    csvFile << size << ",Lineal,0,Case3," << const_c3 << "," << search << "," << search_c3 << "," << bytes_c3 << "\n";
                }

                // ==========================================
                // 2. DISTRIBUCIONES NORMALES (GAUSS)
                // ==========================================
                for (double dev : deviations) {
                    vector<T> gauss_vec, gauss_gap, gauss_sample;

                    // Caso 1
                    t1 = chrono::high_resolution_clock::now();
                    Generar::vectorGaussSorted(gauss_vec, size, dev);
                    t2 = chrono::high_resolution_clock::now();
                    const_c1 = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                    bytes_c1 = gauss_vec.size() * sizeof(T);

                    // Caso 2
                    t1 = chrono::high_resolution_clock::now();
                    Generar::vectorGap(gauss_vec, gauss_gap, size);
                    Generar::vectorSample(gauss_vec, gauss_sample, size);
                    t2 = chrono::high_resolution_clock::now();
                    const_c2 = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                    bytes_c2 = gauss_gap.size() * sizeof(T) + gauss_sample.size() * sizeof(T);

                    // Caso 3
                    t1 = chrono::high_resolution_clock::now();
                    Compresion::EliasFano gauss_ef = Compresion::eliasFanoCodif(gauss_vec);
                    t2 = chrono::high_resolution_clock::now();
                    const_c3 = chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
                    bytes_c3 = (gauss_ef.low.size() + gauss_ef.high.size()) / 8 + gauss_sample.size() * sizeof(T);

                    for (int search : searchSize) {
                        uniform_int_distribution<int> dist_idx(0, size - 1);
                        vector<T> targets(search);
                        for(int i=0; i<search; ++i) targets[i] = gauss_vec[dist_idx(gen)];

                        // Búsquedas Caso 1
                        auto ts1 = chrono::high_resolution_clock::now();
                        for (int i = 0; i < search; ++i) 
                            binary_search(gauss_vec.begin(), gauss_vec.end(), targets[i]);
                        auto ts2 = chrono::high_resolution_clock::now();
                        auto search_c1 = chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1).count();
                        csvFile << size << ",Normal," << dev << ",Case1," << const_c1 << "," << search << "," << search_c1 << "," << bytes_c1 << "\n";

                        // Búsquedas Caso 2
                        ts1 = chrono::high_resolution_clock::now();
                        for (int i = 0; i < search; ++i) {
                            int L, R;
                            if (Busqueda::sample_range(gauss_sample, b, targets[i], size, L, R))
                                Busqueda::GC_decode(gauss_gap, gauss_sample, targets[i], b, L, R);
                        }
                        ts2 = chrono::high_resolution_clock::now();
                        auto search_c2 = chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1).count();
                        csvFile << size << ",Normal," << dev << ",Case2," << const_c2 << "," << search << "," << search_c2 << "," << bytes_c2 << "\n";

                        // Búsquedas Caso 3
                        ts1 = chrono::high_resolution_clock::now();
                        for (int i = 0; i < search; ++i) {
                            int L, R;
                            if (Busqueda::sample_range(gauss_sample, b, targets[i], size, L, R))
                                Busqueda::EF_decode(gauss_ef, targets[i], L, R);
                        }
                        ts2 = chrono::high_resolution_clock::now();
                        auto search_c3 = chrono::duration_cast<std::chrono::milliseconds>(ts2 - ts1).count();
                        csvFile << size << ",Normal," << dev << ",Case3," << const_c3 << "," << search << "," << search_c3 << "," << bytes_c3 << "\n";
                    }
                }
            }
        }
        csvFile.close();
    }
}
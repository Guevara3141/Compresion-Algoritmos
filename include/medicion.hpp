#pragma once
#include <cstring>
#include <vector>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include "generar.hpp"

namespace Medicion {
    using namespace std;

    static random_device rd;
    static mt19937 gen(rd());

    template <typename T>
    void medicion(const vector<int> &sizes, const vector<double> &deviations, const vector<int> &searchSize) {

        vector<T>vec;
        vector<T>gauss;

        if (filesystem::exists("mediciones.csv")) filesystem::remove("mediciones.csv");
        ofstream csvFile("mediciones.csv");
        if (csvFile.is_open()) {
            csvFile << "Size, Distribution, Deviation, Case, ConstructionTime_ms, SearchAmount, SearchTime_ms, ElementBytes\n" << endl;

            for (int size : sizes) {
                //initial vector
                auto tvec1 = chrono::high_resolution_clock::now();
                Generar::vectorUniformeSorted<T>(vec, size);
                auto tvec2 = chrono::high_resolution_clock::now();
                auto tvec = std::chrono::duration_cast<std::chrono::milliseconds>(tvec2-tvec1);
                size_t vec_elem = vec.size() * sizeof(T);
                for (int search : searchSize) {

                    uniform_int_distribution<T> distribucion(vec.front(), vec.back());

                    auto tsearch1 = chrono::high_resolution_clock::now();
                    for (int i=0; i<search; ++i) 
                        binary_search(vec.begin(), vec.end(), distribucion(gen));
                    auto tsearch2 = chrono::high_resolution_clock::now();
                    auto tsearch = chrono::duration_cast<std::chrono::milliseconds>(tsearch2 - tsearch1);
                    csvFile << size << ", Lineal, 0, Case1, " << tvec.count() << ", " << search << ", " << tsearch.count() << ", " << vec_elem << endl;
                }

                //Gauss distribution vectors
                for (double dev : deviations) {
                    auto tgauss1 = chrono::high_resolution_clock::now();
                    Generar::vectorGaussSorted(gauss, size, dev);
                    auto tgauss2 = chrono::high_resolution_clock::now();
                    auto tgauss = chrono::duration_cast<std::chrono::milliseconds>(tgauss2 - tgauss1);
                    size_t gauss_elem = gauss.size() * sizeof(T);

                    for (int search : searchSize) {
                        uniform_int_distribution<T> distribution(gauss.front(), gauss.back());
                        auto tsearch1_gauss = chrono::high_resolution_clock::now();
                        for (int i=0; i<search; ++i) 
                            binary_search(gauss.begin(), gauss.end(), distribution(gen));
                        auto tsearch2_gauss = chrono::high_resolution_clock::now();
                        auto tsearch_gauss = chrono::duration_cast<std::chrono::milliseconds>(tsearch2_gauss - tsearch1_gauss);
                        csvFile << size << ", Normal, " << dev << ", Case1, " << tgauss.count() << ", " << search << ", " << tsearch_gauss.count() << ", " << gauss_elem << endl;
                    }
                }

            }
        }
        csvFile.close();
    }
}
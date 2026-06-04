#pragma once
#include <chrono>
#include "busqueda.hpp"
#include "compresion.hpp"
#include "generar.hpp"
#include <iostream>
#include <print>

namespace Case {
    using namespace std;
    template <typename T>
    void cases(const vector<T> &A) {
        int n = A.size();
        
        // Preparar estructuras auxiliares
        vector<T> gap;
        Generar::vectorGap(A, gap, n);
        
        vector<T> sample;
        Generar::vectorSample(A, sample, n);
        int b = 1024; // El valor de salto fijo en generar.hpp

        // Comprimir con Elias-Fano
        Compresion::EliasFano ef = Compresion::eliasFanoCodif(A);

        int case_num, number;
        print("Ingrese caso (1, 2, 3 o 0 para VOLVER): "); cin >> case_num;
        
        if (case_num == 0) return; // Abortar ejecución de inmediato
        
        print("Ingrese numero a buscar: "); cin >> number;

        bool cont = true;
        while (cont) {
            if (case_num == 1) {
                auto tsearch1 = chrono::high_resolution_clock::now();
                auto it = lower_bound(A.begin(), A.end(), number);
                auto tsearch2 = chrono::high_resolution_clock::now();
                auto tsearch = std::chrono::duration_cast<std::chrono::microseconds>(tsearch2 - tsearch1);
                
                if (it == A.end() || *it != number)
                    println("Numero no fue encontrado."); 
                else
                    println("Numero encontrado en posicion {}, en {} us", it-A.begin()+1, tsearch.count());
            }
            else if (case_num == 2) {
                auto tsearch1 = chrono::high_resolution_clock::now();
                int L, R;
                bool found = false;
                if (Busqueda::sample_range(sample, b, number, n, L, R)) {
                    found = Busqueda::GC_decode(gap, sample, number, b, L, R);
                }
                auto tsearch2 = chrono::high_resolution_clock::now();
                auto tsearch = std::chrono::duration_cast<std::chrono::microseconds>(tsearch2 - tsearch1);
                
                if (found) println("Numero encontrado con Gap-Coding en {} us", tsearch.count());
                else println("Numero no fue encontrado.");
            }
            else if (case_num == 3) {
                auto tsearch1 = chrono::high_resolution_clock::now();
                int L, R;
                bool found = false;
                if (Busqueda::sample_range(sample, b, number, n, L, R)) {
                    found = Busqueda::EF_decode(ef, number, L, R);
                }
                auto tsearch2 = chrono::high_resolution_clock::now();
                auto tsearch = std::chrono::duration_cast<std::chrono::microseconds>(tsearch2 - tsearch1);
                
                if (found) println("Numero encontrado con Elias-Fano en {} us", tsearch.count());
                else println("Numero no fue encontrado.");
            }

            string req;
            print("Desea continuar? (s:1, n:0) : "); cin >> req;
            if (req == "n" || req == "0") cont = false;
            else {
                print("Ingrese caso (1, 2, 3 o 0 para VOLVER): "); cin>>case_num;
                if (case_num == 0) return;
                print("Ingrese numero a buscar: "); cin>>number;
            }
        }
    } 
}
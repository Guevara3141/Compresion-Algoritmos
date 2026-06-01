#pragma once
#include <chrono>
#include "busqueda.hpp"
#include <iostream>
#include <print>


namespace Case {
    using namespace std;
    template <typename T>
    void cases(const vector<T> &A) {
        int case_num, number;
        print("Ingrese caso: "); cin >> case_num;
        print("Ingrese numero a buscar: "); cin >> number;
        cout << number << endl;

        bool cont = true;
        while (cont) {

            if (case_num == 1) {
                auto tsearch1 = chrono::high_resolution_clock::now();
                auto it = lower_bound(A.begin(), A.end(), number);
                auto tsearch2 = chrono::high_resolution_clock::now();
                auto tsearch = std::chrono::duration_cast<std::chrono::milliseconds>(tsearch2 - tsearch1);
                if (it == A.end() || *it != number)
                    println("Numero no fue encontrado."); 
                else {
                    println("Numero encontrado en posicion {}, en {}", it-A.begin()+1, tsearch);
                }

            }


            string req;
            print("Desea continuar? (s:1, n:0) : "); cin >> req;
            if (req == "n") cont = false;
            else {
                print("Ingrese caso: "); cin>>case_num;
                print("Ingrese numbero a buscar: "); cin>>number;
            }
        }
        

    } 
}

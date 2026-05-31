#include <cstring>
#include <vector>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <print>
#include "include/generar.hpp"
#include "include/medicion.hpp"

using namespace std;


int main(int argc, char** argv) {

    if (argc < 2 || argc > 4) 
        return (println("Usage: ./main <input_file> <output_file>"), 1);
    if (argc == 2 && strncmp(argv[1], "--benchmark", 11) == 0) {
        println("Benchmark mode");
        println("args: {}", argv[1]);

        
        vector<int>sizes = {10000000, 100000000};
        //may get stuck with 100000000
        vector<double>dev = {1.0, 5.0, 15.0, 30.0};
        vector<int>searchSize = {1000, 10000, 100000};

        Medicion::medicion<int32_t>(sizes, dev, searchSize);
        
        

        /*
        //Initial vector

        vector<int32_t>vec;
        vector<int32_t>vec_gap;
        vector<int32_t>vec_sample;
        vector<int32_t>gauss;
        vector<int32_t>gauss_gap;
        vector<int32_t>gauss_sample;

        auto tgenerar1 = chrono::high_resolution_clock::now();

        Generar::vectorUniformeSorted<int32_t>(vec, 1e7);
        auto tgenerar2 = chrono::high_resolution_clock::now();
	    auto tgenerar = std::chrono::duration_cast<std::chrono::milliseconds>(tgenerar2 - tgenerar1);
        size_t vec_elem_bytes = vec.size() * sizeof(decltype(vec)::value_type);

	
        //gap vector
        auto tgap1 = chrono::high_resolution_clock::now();
        Generar::vectorGap<int32_t>(vec, vec_gap, 1e7);
        auto tgap2 = chrono::high_resolution_clock::now();
        auto tgap = std::chrono::duration_cast<std::chrono::milliseconds>(tgap2 - tgap1);
        size_t gap_elem_bytes = vec_gap.size() * sizeof(decltype(vec_gap)::value_type);

        //sample vector
        auto tsample1 = chrono::high_resolution_clock::now();
        Generar::vectorSample<int32_t>(vec, vec_sample, 1e7);
        auto tsample2 = chrono::high_resolution_clock::now();
        auto tsample = std::chrono::duration_cast<std::chrono::milliseconds>(tsample2 - tsample1);
        size_t sample_elem_bytes = vec_sample.size() * sizeof(decltype(vec_sample)::value_type);


        //Gauss vector
        auto generar1 = chrono::high_resolution_clock::now();

        //numbers centered around 50, with spread of 15
        Generar::vectorGauss<int32_t>(gauss, 1e7, 5);
        //data type for gauss must be float, double, or long double

        auto generar2 = chrono::high_resolution_clock::now();
	    auto generar = std::chrono::duration_cast<std::chrono::milliseconds>(generar2 - generar1);
        size_t gauss_elem_bytes = gauss.size() * sizeof(decltype(gauss)::value_type);

	
        //gap vector
        auto gap1 = chrono::high_resolution_clock::now();
        Generar::vectorGap<int32_t>(gauss, gauss_gap, 1e7);
        auto gap2 = chrono::high_resolution_clock::now();
        auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(gap2 - gap1);
        size_t gauss_gap_elem_bytes = gauss_gap.size() * sizeof(decltype(gauss_gap)::value_type);

        //sample vector
        auto sample1 = chrono::high_resolution_clock::now();
        Generar::vectorSample<int32_t>(gauss, gauss_sample, 1e7);
        auto sample2 = chrono::high_resolution_clock::now();
        auto sample = std::chrono::duration_cast<std::chrono::milliseconds>(sample2 - sample1);
        size_t gauss_sample_elem_bytes = gauss_sample.size() * sizeof(decltype(gauss_sample)::value_type);

        //write to lineal vector csv file
        if (filesystem::exists("lineal_vector.csv")) filesystem::remove("lineal_vector.csv");
        ofstream lineal_vector("lineal_vector.csv");
        if (lineal_vector.is_open()) {
            lineal_vector << "name, time, space_mb\n";
            
            lineal_vector << "vector, " << tgenerar << ", " << vec_elem_bytes/1e+6
                          << "\ngap, " << tgap << ", " << gap_elem_bytes/1e+6
                          << "\nsample, " << tsample << ", " << sample_elem_bytes/1e+6 << endl;
        }
        lineal_vector.close();

        //write to gauss vector
        if (filesystem::exists("gauss_vector.csv")) filesystem::remove("gauss_vector.csv");
        ofstream gauss_vector("gauss_vector.csv");
        if (gauss_vector.is_open()) {
            gauss_vector << "name, time, space_mb\n";
            gauss_vector << "vector, " << generar << ", " << gauss_elem_bytes/1e+6
                          << "\ngap, " << gap << ", " << gauss_gap_elem_bytes/1e+6
                          << "\nsample, " << sample << ", " << gauss_sample_elem_bytes/1e+6 << endl;
        }
        
        

        println("Time taken to generate and sort vector: {} ms", tgenerar / 1ms);
        print("First 10 elements of the sorted vector: ");

        size_t n = min(10, (int)vec.size());
        for (size_t i = 0; i < n; ++i) {
            print("{} ", vec[i]);
        }
        println();println();



        println("Time taken to generate gap vector: {} ms", tgap / 1ms);
        print("First 10 elements of the gap vector: ");
        n = min(10, (int)vec_gap.size());
        for (size_t i = 0; i < n; ++i) {
            print("{} ", vec_gap[i]);
        }
        println();println();

        
        println("Time taken to generate sample vector: {} ms", tsample / 1ms);
        print("First 10 elements of the sample vector: ");
        n = min(10, (int)vec_sample.size());
        for (size_t i = 0; i < n; ++i) {
            print("{} ", vec_sample[i]);
        }
        println();println();

        //gauss

        println("Time taken to generate and sort vector: {} ms", generar / 1ms);
        print("First 10 elements of the sorted vector: ");

        n = min(10, (int)gauss.size());
        for (int i = 0; i < 1e7; ++i) {
            print("{} ", gauss[i]);
        }
        println();println();



        println("Time taken to generate gap vector: {} ms", gap / 1ms);
        print("First 10 elements of the gap vector: ");
        n = min(10, (int)gauss_gap.size());
        for (size_t i = 0; i < n; ++i) {
            print("{} ", gauss_gap[i]);
        }
        println();println();

        
        println("Time taken to generate sample vector: {} ms", sample / 1ms);
        print("First 10 elements of the sample vector: ");
        n = min(10, (int)gauss_sample.size());
        for (size_t i = 0; i < n; ++i) {
            print("{} ", gauss_sample[i]);
        }
        println();println();
        */
        
    } else if (argc == 4) {
        println("Complete mode");
        println("args: {}, {}", argv[1], argv[2]);
    }
    return 0;
}

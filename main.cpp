#include <cstring>
#include <vector>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <print>
#include "include/generar.hpp"

using namespace std;


int main(int argc, char** argv) {

    if (argc < 2 || argc > 4) 
        return (println("Usage: ./main <input_file> <output_file>"), 1);
    if (argc == 2 && strncmp(argv[1], "--benchmark", 11) == 0) {
        println("Benchmark mode");
        println("args: {}", argv[1]);

        //Initial vector
        auto tgenerar1 = chrono::high_resolution_clock::now();

        auto vec = Generar::vectorUniforme<int32_t>(1e7);
	    sort(vec.begin(), vec.end());

        auto tgenerar2 = chrono::high_resolution_clock::now();
	    auto tgenerar = std::chrono::duration_cast<std::chrono::milliseconds>(tgenerar2 - tgenerar1);
        size_t vec_elem_bytes = vec.size() * sizeof(decltype(vec)::value_type);

	
        //gap vector
        auto tgap1 = chrono::high_resolution_clock::now();
        auto vec_gap = Generar::vectorGap<int32_t>(vec, 1e7);
        auto tgap2 = chrono::high_resolution_clock::now();
        auto tgap = std::chrono::duration_cast<std::chrono::milliseconds>(tgap2 - tgap1);
        size_t gap_elem_bytes = vec_gap.size() * sizeof(decltype(vec_gap)::value_type);

        //sample vector
        auto tsample1 = chrono::high_resolution_clock::now();
        auto vec_sample = Generar::vectorSample<int32_t>(vec, 1e7);
        auto tsample2 = chrono::high_resolution_clock::now();
        auto tsample = std::chrono::duration_cast<std::chrono::milliseconds>(tsample2 - tsample1);
        size_t sample_elem_bytes = vec_sample.size() * sizeof(decltype(vec_sample)::value_type);


        //Gauss vector
        auto generar1 = chrono::high_resolution_clock::now();

        //numbers centered around 50, with spread of 15
        auto gauss = Generar::vectorGauss<int32_t>(1e7, 50.0, 15.0);
	    sort(gauss.begin(), gauss.end());
        //data type for gauss must be float, double, or long double

        auto generar2 = chrono::high_resolution_clock::now();
	    auto generar = std::chrono::duration_cast<std::chrono::milliseconds>(generar2 - generar1);
        size_t gauss_elem_bytes = gauss.size() * sizeof(decltype(gauss)::value_type);

	
        //gap vector
        auto gap1 = chrono::high_resolution_clock::now();
        auto gauss_gap = Generar::vectorGap<int32_t>(gauss, 1e7);
        auto gap2 = chrono::high_resolution_clock::now();
        auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(gap2 - gap1);
        size_t gauss_gap_elem_bytes = gauss_gap.size() * sizeof(decltype(gauss_gap)::value_type);

        //sample vector
        auto sample1 = chrono::high_resolution_clock::now();
        auto gauss_sample = Generar::vectorSample<int32_t>(gauss, 1e7);
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
        for (size_t i = 0; i < n; ++i) {
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

    } else if (argc == 4) {
        println("Complete mode");
        println("args: {}, {}", argv[1], argv[2]);
    }
    return 0;
}

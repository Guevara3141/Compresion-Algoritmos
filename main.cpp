#include <cstring>
#include <vector>
#include <chrono>
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
	
        //gap vector
        auto tgap1 = chrono::high_resolution_clock::now();
        auto vec_gap = Generar::vectorGap<int32_t>(vec, 1e7);
        auto tgap2 = chrono::high_resolution_clock::now();
        auto tgap = std::chrono::duration_cast<std::chrono::milliseconds>(tgap2 - tgap1);

        //sample vector
        auto tsample1 = chrono::high_resolution_clock::now();
        auto vec_sample = Generar::vectorSample<int32_t>(vec, 1e7);
        auto tsample2 = chrono::high_resolution_clock::now();
        auto tsample = std::chrono::duration_cast<std::chrono::milliseconds>(tsample2 - tsample1);
        

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
        

    } else if (argc == 4) {
        println("Complete mode");
        println("args: {}, {}", argv[1], argv[2]);
    }
    return 0;
}

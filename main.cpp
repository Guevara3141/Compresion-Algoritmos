#include <bits/stdc++.h>
#include "include/generar.hpp"

using namespace std;

int main(int argc, char** argv) {

    if (argc < 2 || argc > 4) 
        return (println("Usage: ./main <input_file> <output_file>"), 1);
    if (argc == 2 && strncmp(argv[1], "--benchmark", 11) == 0) {
        println("Benchmark mode");
        println("args: {}", argv[1]);

        auto tgenerar1 = chrono::high_resolution_clock::now();
        auto vec = Generar::vectorUniforme<int32_t>(1e7);

		sort(vec.begin(), vec.end());
        auto tgenerar2 = chrono::high_resolution_clock::now();

		auto tgenerar = std::chrono::duration_cast<std::chrono::milliseconds>(tgenerar2 - tgenerar1);
		        
        println("Time taken to generate and sort vector: {} ms", tgenerar / 1ms);

        println("First 10 elements of the sorted vector: {}",
            vector<int32_t>(vec.begin(), vec.begin() + min(10, (int)vec.size()))
        );


    } else if (argc == 4) {
        println("Complete mode");
        println("args: {}, {}", argv[1], argv[2]);
    }
    return 0;
}

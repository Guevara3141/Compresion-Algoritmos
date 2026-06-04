CXX = g++
CXXFLAGS = -std=c++23 -O3 -Wall -Wno-unused-result

HEADERS = include/generar.hpp include/compresion.hpp include/busqueda.hpp include/medicion.hpp include/cases.hpp


main: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o main main.cpp

.PHONY: run clean

# directamente al benchmark
run: main
	./main --benchmark 

# limpiar
clean:
	rm -f main

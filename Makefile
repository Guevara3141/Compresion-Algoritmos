main: main.cpp include/generar.hpp
	g++ -std=c++23 -o main main.cpp include/generar.hpp -Wno-unused-result

.PHONY: run
run: main
	./main --benchmark 

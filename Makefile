main: main.cpp include/generar.hpp include/compresion.hpp include/busqueda.hpp include/medicion.hpp
	g++-14 -std=c++23 -o main main.cpp include/generar.hpp include/compresion.hpp include/busqueda.hpp include/medicion.hpp -Wno-unused-result

.PHONY: run clean
run: main
	./main --benchmark 

clean:
	rm -f main
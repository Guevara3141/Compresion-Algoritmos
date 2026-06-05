# INFO145: Diseño y Análisis de Algoritmos.

Proyecto Semestral de Técnicas de Representación y Compresión en Arreglos Ordenados

## Integrantes
-- Aaron Mancilla
-- Felipe Guevara
-- Agustin Estrada
-- Benjamin Gonzalez

## Descripción

Este proyecto implementa y compara tres estrategias para representar y buscar en arreglos ordenados de gran tamaño:

- **Caso 1 – Representación Explícita:** Arreglo ordenado con búsqueda binaria estándar. Sirve como línea base de comparación.
- **Caso 2 – Gap-Coding + Sample:** El arreglo se reemplaza por sus diferencias consecutivas (gaps). Se usa un índice de muestreo (sample) para acotar el rango de búsqueda y luego decodificar secuencialmente.
- **Caso 3 – Elias-Fano:** Los valores del arreglo se comprimen con la técnica Elias-Fano, separando bits altos (codificados en unario) y bits bajos (almacenados directamente). La búsqueda usa el mismo sample del Caso 2 para acotar el rango y luego decodifica sobre la estructura comprimida.

Se miden tiempos de construcción, tiempos de búsqueda y espacio utilizado para distribuciones **lineal** y **normal (gaussiana)** con distintas desviaciones estándar, a escala de hasta 10^8 elementos.

## Estructura del Repositorio

```
.
├── main.cpp                  # Punto de entrada: parsing de argumentos y flujo principal
├── Makefile                  # Compilación del proyecto
├── README.md                 # Este archivo
├── generar_csv_test.py       # Script auxiliar Python para generar datos de prueba
└── include/
    ├── generar.hpp           # Generación de vectores (uniforme, gaussiano, gap, sample)
    ├── compresion.hpp        # Codificación Elias-Fano
    ├── busqueda.hpp          # Algoritmos de búsqueda (binaria sobre sample, GC-decode, EF-decode)
    ├── medicion.hpp          # Benchmarking: mide tiempos y espacio para todos los casos
    └── cases.hpp             # Modo interactivo: permite buscar un número en cualquier caso
```

## Rango de Números Aceptados

El programa trabaja con enteros de **32 bits con signo** (`int32_t`), por lo tanto el rango de valores válidos es:

```
-2,147,483,648  a  2,147,483,647
```

Para el **modo archivo** (`-i`), el archivo CSV debe contener números enteros dentro de ese rango, separados por comas (con o sin saltos de línea). Ver formato en la sección de entradas más abajo.

## Cómo Compilar

Requiere un compilador con soporte **C++23** (GCC 13+ recomendado).

```bash
make
```

Esto genera el ejecutable `./main`.

Para limpiar el ejecutable compilado:

```bash
make clean
```

## Cómo Ejecutar

El programa soporta dos modos de ejecución:

### Modo Benchmark

Genera datos automáticamente, construye las estructuras y mide tiempos y espacio para todos los casos y tamaños. Los resultados se guardan en `mediciones.csv`.

```bash
./main --benchmark
```
O alternativamente, utilizando el atajo: 
```bash
make run
```
Este compila e inmediatamente ejecuta el --benchmark.

### Modo Archivo (Interactivo)

Recibe un archivo `.csv` con números enteros, construye las tres estructuras y permite al usuario buscar valores de forma interactiva, seleccionando qué caso usar.

```bash
./main -i ruta/del/archivo.csv
```

Una vez cargado el archivo, el programa pedirá:
1. El caso a usar (1, 2 o 3).
2. El número a buscar.
3. Si desea realizar otra búsqueda.

Ingrese `0` en la selección de caso para salir.

## Formato de Entrada (Modo Archivo)

El archivo CSV debe contener números enteros separados por comas, con o sin saltos de línea:

```
12,45,78,102,200
201,350,400
```

O en una sola línea:

```
12,45,78,102,200,201,350,400
```

Los números **deben estar ordenados de menor a mayor**. Ver `datos_gigantes.csv` (generado por `generar_csv_test.py`) como ejemplo.

## Archivos de Salida

### Modo Benchmark → `mediciones.csv`

Contiene una fila por cada combinación de tamaño, distribución y caso:

```
Size,Distribution,Deviation,Case,ConstructionTime_ms,SearchAmount,SearchTime_ms,TotalBytes
```

| Columna | Descripción |
|---|---|
| `Size` | Cantidad de elementos del arreglo |
| `Distribution` | `Lineal` o `Normal` |
| `Deviation` | Desviación estándar (0 si es lineal) |
| `Case` | `Case1`, `Case2` o `Case3` |
| `ConstructionTime_ms` | Tiempo de construcción de la estructura en ms |
| `SearchAmount` | Número de búsquedas realizadas |
| `SearchTime_ms` | Tiempo total de búsquedas en ms |
| `TotalBytes` | Espacio total usado por la estructura en bytes |

### Modo Archivo → Salida por consola

El programa indica si el valor fue encontrado, en qué posición (Caso 1) y el tiempo de búsqueda en microsegundos.

## Ejemplo de Ejecución (Benchmark)

```
Procesando size: 1000000...
Procesando size: 10000000...
...
archivo creado en mediciones.csv
```

## Ejemplo de Ejecución (Modo Archivo)

```bash
./main -i datos_gigantes.csv

Ingrese caso (1, 2, 3 o 0 para VOLVER): 1
Ingrese numero a buscar: 5042
Numero encontrado en posicion 412, en 3 us

Desea continuar? (s:1, n:0): 0
```

## Generación de Datos de Prueba

Para generar un archivo CSV con 1.000.000 de números aleatorios ordenados:

```bash
python3 generar_csv_test.py
```

Esto crea `datos_gigantes.csv` que puede usarse con el modo `-i`.
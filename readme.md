# INFO145: Algoritmos de compresion

Este readme es un ejemplo

## Nombres

- Benjamín Parra
- Carolina Obreque

## Descripción

[resumen corto] Aqui se mide el tiempo de ordenamiento.

./include tiene los includes
main contiene el flujo de inicializacion y parsing
salida.csv se genera al ejecutar modo --benchmark y contiene un resumen de los resultados

## Como compilar

Solo ejecutar

```bash
make
```

## Como ejecutar

./main --benchmark ...

./main -i \<archivo\>

### Entradas y salida

El archivo de entrada debe ser asi:

\<NUMERO\>,\<NUMERO\>,\<NUMERO\>,\<NUMERO\>

O bien separado por , mas saltos de linea.

- Ver entrada.csv como ejemplo

La salida tiene el siguiente formato:

```md
\<tamaño\>,\<tiempo\>,\<espacio\>,\<distribución\>,\<caso\>
\<tamaño\>,\<tiempo\>,\<espacio\>,\<distribución\>,\<caso\>
```

### Ejemplo de ejecucion

```md
Para 2^28 valores, promedio:
-----------------
fase creacion: 30 ms, espacio: 5mb
10000 busquedas caso 1: 30ms, espacio: 8mb
10000 busquedas caso 2: 70ms, espacio: 4mb
10000 busquedas caso 3: 200ms, espacio: 2mb

Creacion caso 2: 20ms
Creacion caso 3: 30ms

radio de mejora (espacio): 
Caso 2 vs caso 1: 50%
Caso 3 vs caso 1: 75%

radio de mejora (tiempo):
Caso 2 vs caso 1: [tiempo]
Caso 3 vs caso 1: [insertar tiempo]

archivo creado en salida.csv
```

import random

def generar_datos_masivos(nombre_archivo, cantidad):
    print(f"Generando {cantidad} números ordenados...")
    valor_actual = 0
    
    with open(nombre_archivo, 'w') as f:
        for i in range(1, cantidad + 1):

            valor_actual += random.randint(1, 10) 
            f.write(str(valor_actual))
            
            if i < cantidad:

                if i % 1000 == 0:
                    f.write(",\n")
                else:
                    f.write(",")
                    
    print(f"Archivo '{nombre_archivo}' generado exitosamente.")

# Generar 1 millón de datos
generar_datos_masivos("datos_gigantes.csv", 1000000)
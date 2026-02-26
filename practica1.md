# ArqCompPractica1

# Estudio del comportamiento de caché en Finisterrae III

**Autores:** [Alan Barreiro y Damián Del Río]  
**Sistema:** CESGA Finisterrae III  
**Procesador:** Intel Xeon Ice Lake 8352Y

---

## Introducción

En este experimento se analiza cómo influye el acceso a memoria en el rendimiento de un programa sencillo en C.  
El programa realiza sumas sobre un vector, pero accediendo a los elementos con un salto fijo definido por el parámetro $D$.

El objetivo es observar cómo cambia el número de ciclos por acceso cuando:

- Cambia el tamaño del vector
- Cambia el salto
- Cambia el tipo de acceso

Las medidas se obtuvieron utilizando el contador de ciclos del procesador (`rdtsc`) y compilando sin optimizaciones.

---

## Características de la caché

Los parámetros de caché se obtuvieron directamente del sistema:

Configuración:

- Tamaño de línea: 64 bytes
- Caché L1: 48 KB
- Caché L2: 1.25 MB

Número de líneas:

- L1: 768 líneas
- L2: 20480 líneas

---

## Resultados con double

Se utilizó un vector de `double` (8 bytes) accedido mediante un vector de índices.

| D \\ L | 384 | 1152 | 10240 | 163840 |
|-------|-----|------|-------|--------|
| 2 | 7.14 | 7.17 | 7.13 | 7.17 |
| 128 | 7.19 | 7.75 | 7.58 | 16.04 |
| 1024 | 9.94 | 8.14 | 17.97 | 17.92 |

### Interpretación

Para valores pequeños de $D$, los accesos permanecen dentro de la misma línea de caché durante varias iteraciones.  
Esto permite aprovechar los datos cargados previamente.

Cuando el salto aumenta, cada acceso corresponde a una línea distinta.  
En este caso, cada lectura implica traer datos nuevos desde niveles más lentos de memoria.

Esto explica el aumento progresivo en los ciclos por acceso.

---
# Resultados con Integers (4 bytes)

Aquí usamos el archivo `int.csv`. Como el `int` ocupa 4 bytes, en una línea de 64 bytes caben 16 elementos. Esto se nota en que los ciclos son ligeramente más bajos que en el `double` cuando el salto ($D$) es pequeño.

| D \ L | 384 (L1) | 1152 (L1-L2) | 10240 (L2) | 163840 (RAM) |
|------|----------|--------------|------------|--------------|
| 2    | 6.88     | 6.87         | 6.91       | 6.91         |
| 8    | 6.92     | 6.94         | 6.95       | 6.95         |
| 128  | 7.37     | 7.63         | 7.64       | 14.52        |
| 1024 | 9.02     | 8.04         | 16.94      | 17.51        |

**Nota:** En $D=2$ y $D=8$, el tiempo es casi idéntico. Esto es porque con un `int`, un salto de 8 posiciones sigue siendo solo media línea de caché (32 bytes), así que la localidad espacial sigue siendo excelente.

---

## Comparación con enteros

Se repitió el experimento utilizando `int` en lugar de `double`.

Dado que un entero ocupa la mitad, cada línea de caché contiene más elementos.

Esto permite reutilizar datos durante más tiempo cuando el salto es pequeño.

Sin embargo, cuando el salto es grande, el beneficio desaparece porque cada acceso requiere igualmente una nueva línea de caché.

---

# Resultados con Acceso Directo (Double)

Estos datos salen de `directo.csv`. Aquí no usamos el vector `ind[]`, sino que el programa calcula la dirección directamente. Al quitar la carga de memoria del índice, rascamos unos decimales en casi todas las medidas.

| D \ L | 384 (L1) | 1152 (L1-L2) | 10240 (L2) | 163840 (RAM) |
|------|----------|--------------|------------|--------------|
| 2    | 7.05     | 7.07         | 7.10       | 7.15         |
| 8    | 7.00     | 7.14         | 7.13       | 7.22         |
| 128  | 7.25     | 7.69         | 7.55       | 16.03        |
| 1024 | 9.77     | 7.97         | 17.18      | 17.21        |


## Acceso directo frente a indirecto

Se compararon dos formas de acceder a los datos:

Indirecto: A[ind[i]]

Directo: A[i*D]


Resultados aproximados:

| Tipo | Ciclos |
|------|--------|
| Indirecto | 7 - 18 |
| Directo | 6 - 17 |

El acceso indirecto requiere una lectura adicional del vector de índices, lo que introduce más tráfico de memoria.

Esto se traduce en un pequeño aumento del tiempo por acceso.

---

## Prefetching

El procesador intenta anticipar accesos a memoria cuando detecta patrones regulares.

Esto reduce la latencia cuando el salto es constante.

Sin embargo, cuando la distancia entre accesos es muy grande, la anticipación deja de ser efectiva.

En ese caso pueden aparecer accesos a memoria principal.

---

## Conclusiones

El experimento muestra que el rendimiento depende en gran medida del patrón de acceso a memoria.

Los accesos secuenciales permiten aprovechar mejor la caché, mientras que los accesos dispersos generan más fallos.

También observamos que el tipo de acceso influye en menor medida que el patrón de memoria.

En general, mantener una buena localidad espacial es clave para obtener buen rendimiento.

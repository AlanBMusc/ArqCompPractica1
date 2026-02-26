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

## Comparación con enteros

Se repitió el experimento utilizando `int` en lugar de `double`.

Dado que un entero ocupa la mitad, cada línea de caché contiene más elementos.

Esto permite reutilizar datos durante más tiempo cuando el salto es pequeño.

Sin embargo, cuando el salto es grande, el beneficio desaparece porque cada acceso requiere igualmente una nueva línea de caché.

---

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

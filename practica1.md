# ArqCompPractica1

# Estudio del comportamiento de caché en Finisterrae III

**Autores:** [Alan Barreiro y Damián Del Río]  
**Sistema:** CESGA Finisterrae III  
**Procesador:** Intel Xeon Ice Lake 8352Y

---
## Introducción

En este experimento se analiza cómo influye el acceso a memoria en el rendimiento de un programa sencillo en C.

El programa realiza sumas sobre un vector, accediendo a los elementos con un salto fijo definido por el parámetro $D$.

El objetivo es observar cómo cambia el número de ciclos por acceso cuando:

- Cambia el tamaño del vector  
- Cambia el salto (stride)  
- Cambia el tipo de acceso (Directo vs Indirecto)  

Las medidas se obtuvieron utilizando el contador de ciclos del procesador (`rdtsc`) y compilando sin optimizaciones (`-O0`).

---

## Metodología y Parámetros

### 1. Deducción de los cálculos

Para obtener el coste medio por acceso, el programa realiza una reducción (suma) de $R$ elementos.  
La medición se realiza sobre 10 repeticiones de este bucle para estabilizar los resultados.

El cálculo de ciclos por acceso se define como:

$$
\text{Ciclos por acceso} =
\frac{\text{Ciclos totales de las 10 repeticiones}}{10 \times R}
$$

---

### 2. Selección de los saltos ($D$)

Los valores de $D$ (2, 8, 128, 512, 1024) se eligieron para analizar el comportamiento respecto a la línea de caché (64 bytes):

- **D = 2**  
  Alta localidad espacial. En `double` (8 bytes), accedemos a un dato cada 16 bytes.  
  Aprovechamos 4 datos por cada línea cargada.

- **D = 8**  
  Punto crítico para `double`. Como $8 \times 8 = 64$ bytes, cada acceso cae en una línea distinta.  
  Aquí desaparece la localidad espacial.

- **D > 8**  
  Saltos grandes que permiten comprobar si el Hardware Prefetcher puede seguir el patrón.

---

### 3. Origen de las líneas ($L$)

Los valores de $L$ se seleccionaron para observar la transición entre niveles de memoria:

- **384** → Mitad de la L1 (768 líneas)  
- **1152** → Supera la L1 pero cabe en L2  
- **10240 / 15360** → Zona intermedia de la L2  
- **163840** → Supera ampliamente la L2 y fuerza accesos a RAM  

---

### 4. Relación entre L y R

El número de elementos a sumar ($R$) se calcula dinámicamente:

Si $D < \text{elementos por línea}$:
- Varios accesos caen en la misma línea.
- Para llenar $L$ líneas necesitamos más accesos.

Si $D \geq 8$ (para double):
- Cada acceso cae en una línea distinta.
- Por tanto:

$$
R = L
$$

No existe reutilización de líneas de caché.

---

## Características de la caché

Los parámetros se obtuvieron del sistema:

/sys/devices/system/cpu/cpu0/cache/
---

## Resultados con double

Se utilizó un vector de `double` (8 bytes) accedido mediante un vector de índices.

| D \\ L | 384 | 1152 | 10240 | 15360 | 40960 | 81920 | 163840 |
|-------|-----|------|-------|-------|-------|-------|--------|
| 2     | 7.09 | 7.10 | 7.11 | 7.12 | 7.12 | 7.17 | 7.16 |
| 8     | 7.14 | 7.16 | 7.17 | 7.26 | 7.29 | 7.20 | 7.20 |
| 128   | 7.19 | 7.75 | 7.58 | 7.59 | 7.70 | 8.84 | 16.04 |
| 512   | 7.78 | 8.04 | 12.18 | 13.91 | 17.55 | 17.81 | 18.02 |
| 1024  | 9.94 | 8.14 | 17.97 | 18.11 | 18.06 | 18.15 | 18.21 |

### Interpretación

Para valores pequeños de $D$, los accesos permanecen dentro de la misma línea de caché durante varias iteraciones.  
Esto permite aprovechar los datos cargados previamente.

Cuando el salto aumenta, cada acceso corresponde a una línea distinta.  
En este caso, cada lectura implica traer datos nuevos desde niveles más lentos de memoria.

Esto explica el aumento progresivo en los ciclos por acceso.

---
# Resultados con Integers (4 bytes)

Aquí usamos el archivo `int.csv`. Como el `int` ocupa 4 bytes, en una línea de 64 bytes caben 16 elementos. Esto se nota en que los ciclos son ligeramente más bajos que en el `double` cuando el salto ($D$) es pequeño.

| D \\ L | 384 | 1152 | 10240 | 15360 | 40960 | 81920 | 163840 |
|-------|-----|------|-------|-------|-------|-------|--------|
| 2     | 7.58 | 6.87 | 6.91 | 6.90 | 6.94 | 6.91 | 6.91 |
| 8     | 7.03 | 6.94 | 6.95 | 6.96 | 6.98 | 6.97 | 6.96 |
| 128   | 7.37 | 7.63 | 7.64 | 7.64 | 7.67 | 8.12 | 14.52 |
| 512   | 7.67 | 7.82 | 11.23 | 12.85 | 16.88 | 17.15 | 17.44 |
| 1024  | 9.02 | 8.04 | 16.94 | 17.06 | 17.22 | 17.45 | 17.51 |

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

| D \\ L | 384 | 1152 | 10240 | 15360 | 40960 | 81920 | 163840 |
|-------|-----|------|-------|-------|-------|-------|--------|
| 2     | 7.05 | 7.07 | 7.10 | 7.11 | 7.25 | 7.15 | 7.16 |
| 8     | 7.01 | 7.14 | 7.13 | 7.20 | 7.24 | 7.15 | 7.15 |
| 128   | 7.25 | 7.69 | 7.55 | 7.56 | 7.62 | 8.79 | 16.03 |
| 512   | 7.71 | 7.91 | 11.85 | 13.52 | 16.90 | 17.12 | 17.38 |
| 1024  | 9.77 | 7.97 | 17.18 | 17.20 | 17.20 | 17.22 | 17.21 ||


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

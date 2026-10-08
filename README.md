# Algoritmo Genético: minimización de la función esfera

Implementación en C++ de un Algoritmo Genético (AG) con representación binaria para resolver:

```
Minimizar f(x) = x1² + x2²
-10 ≤ x1, x2 ≤ 10
Precisión: 6 decimales
Óptimo conocido: f(0, 0) = 0
```

**Autor:** Angel Gabriel Aguilar Hernandez · zs24013324@estudiantes.uv.mx
Licenciatura en Ingeniería de Software, Universidad Veracruzana

## Compilación y ejecución

Requiere un compilador compatible con C++17 (`clang++` o `g++`).

```bash
clang++ -std=c++17 main.cpp -o genetic_algorithm
./genetic_algorithm
```

El programa realiza 30 ejecuciones independientes, imprime la tabla de resultados en la terminal y la guarda en `results.csv`.

## Representación

- Cada variable se codifica con **25 bits**: ⌈log₂(20 000 001)⌉ = 25, necesarios para cubrir el rango [-10, 10] con precisión de 10⁻⁶.
- El cromosoma mide **50 bits**: los primeros 25 corresponden a x1 y los siguientes 25 a x2.
- Decodificación: `x = -10 + E · 20 / (2²⁵ − 1)`, donde E es el entero que representan los bits.

## Operadores

| Etapa | Implementación |
|---|---|
| Población inicial | Bits aleatorios uniformes |
| Evaluación | Aptitud = f(x1, x2); menor es mejor |
| Selección | Torneo de tamaño 3 |
| Cruza | Un punto |
| Mutación | Inversión de bit |
| Elitismo | El mejor individuo pasa intacto a la siguiente generación |
| Reemplazo | Generacional |

## Parámetros

| Parámetro | Valor |
|---|---|
| Tamaño de población | 100 |
| Generaciones | 200 |
| Probabilidad de cruza | 0.9 |
| Probabilidad de mutación | 1/L = 0.02 |
| Tamaño de torneo | 3 |
| Élites | 1 |
| Ejecuciones | 30 |

Todos los parámetros están definidos como constantes al inicio de `main.cpp`.

## Reproducibilidad

Se usa el generador `std::mt19937`. La ejecución *i* utiliza la semilla `BASE_SEED + i` (1001 a 1030), y la semilla se imprime en la tabla de resultados. Con la misma semilla y el mismo compilador se obtiene exactamente el mismo resultado.

## Archivos

| Archivo | Contenido |
|---|---|
| `main.cpp` | Código fuente del algoritmo |
| `results.csv` | Resultados de las 30 ejecuciones |

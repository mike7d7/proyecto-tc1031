# Proyecto: Osu-MM
[Osu!](https://osu.ppy.sh/) es un juego de ritmo inspirado en el juego [Osu! Tatakae! Ouendan](https://en.wikipedia.org/wiki/Osu!_Tatakae!_Ouendan) para la Nintendo DS. Osu!, al ser un juego de ritmo competitivo, cuenta con información acerca de las diferentes partidas jugadas por los jugadores. En base a la dificultad y calidad de cada partida, se le otorga al jugador un número de [puntos de rendimiento](https://osu.ppy.sh/wiki/en/Performance_points).

Estos puntos de rendimientos se vuelven un reflejo de la habilidad de cada jugador, por lo que el programa los utiliza para emparejar dos jugadores de habilidad similar para un juego multijugador justo (sistema [skill-based match making](https://en.wikipedia.org/wiki/Skill-based_matchmaking)).

## Librerías externas
En la carpeta `external-libs/` se encuentran códigos externos que se utilizan para facilitar el desarrollo del programa.
Las librerías utilizadas y su propósito son las siguientes:
- [rapidcsv](https://github.com/d99kris/rapidcsv/tree/cbd8a0a937b249cc07e2db3bfa9cd2cc1689708f): Utilizada para leer los valores de los archivos CSV de forma sencilla.

## Descripción del avance 1
El primer avance consiste en lo siguiente:
- Creación de clase `User`, que representa a un jugador.
- Función `main`, que genera la interfaz de usuario y utiliza la clase `User` para guardar la información de cada usuario, cargada de los archivos CSV.
- Funcionalidad de mostrar lista de jugadores.
- Funcionalidad de mostrar información detallada de un jugador al escribir su `user_id`.
- Funcionalidad de encontrar y mostrar un oponente con habilidad similar para un jugador.
- Algoritmo de ordenamiento basado en [RadixSort](https://en.wikipedia.org/wiki/Radix_sort) para el ordenamiento de los usuarios en base a sus puntos de rendimiento (necesario para encontrar oponentes).

## Instrucciones para compilar el avance de proyecto
Primero, clonar el proyecto y entrar a su carpeta (en una terminal).
Ejecutar el siguiente comando en la terminal:

`g++ -std=c++11 main.cpp "src/user.cpp" -o osu-mm` 

## Instrucciones para ejecutar el avance de proyecto
Después de compilar el proyecto, ejecuta el siguiente comando en la terminal:

`./osu-mm` 

## Descripción de las entradas del avance de proyecto
El programa requiere archivos en formato CSV en el mismo formato que la base de datos de osu!. En la carpeta `csv/` de este repositorio se encuentran los archivos de ejemplo. Los datos utilizados provienen de [esta entrada](https://data.ppy.sh/2026_09_01_performance_osu_random_10000.tar.bz2) en [data.ppy.sh](https://data.ppy.sh/). Los datos se exportaron a archivos CSV para facilitar su uso en el programa. Los datos originales se encuentran en forma de scripts de SQL.

## Descripción de las salidas del avance de proyecto
Las salidas dependen de la opción seleccionada en el menú principal:
- Opción 1: Muestra 40 jugadores con su respectivo `user_id`.
- Opción 2: Muestra información adicional (puntos de rendimiento y tiempo de juego) de un jugador dado su `user_id`. 
- Opción 3: Encuentra un oponente con una habilidad similar al usuario dado y muestra la información del oponente.

En caso de que el usuario ingrese valores inválidos (ej. ingresa -1 en el `user_id`) se muestrará un mensaje de error.

## Desarrollo de competencias
### SICT0301: Evalúa los componentes
#### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa.
En la línea 78 del archivo `main.cpp` se encuentra mi función de ordenamiento `sort_users_by_skill()`, la cual se basa en RadixSort. Esta función tiene una complejidad temporal de `O(nk)`, donde `n` es el número de elementos en el arreglo y `k` es el número de digitos usados para ordenar, como en este caso, se ordenan exclusivamente valores de tipo `float` (de 32 bits) y el algoritmo los procesa en 4 grupos de 8 bits, la variable `k` tiene un valor de 4. Cómo vimos en clase, esta complejidad es lineal y se simplifica a `O(n)`. Esta complejidad lineal se debe a que el iterador del bucle más externo va de 0 a 32 de 8 en 8. Dentro de este bucle, se encuentran dos bucles que iteran sobre cada elemento del arreglo a ordenar, sin embargo estos bucles no están anidados, por lo que se evita una complejidad cuadrática.

Como el algoritmo debe cumplir todos los ciclos sin importar el orden inicial del arreglo, esto hace que su complejidad sea igual en los tres casos.
- Mejor caso: `O(n)`
- Caso promedio: `O(n)`
- Peor caso: `O(n)`

En complejidad espacial, este algoritmo tiene complejidad `O(n)` (lineal). Esto se debe a que se crea un arreglo (llamado `buffer`) del mismo tamaño que el arreglo a ordenar. Los arreglos adicionales utilizados para almacenar las radixes tienen un tamaño constante, por lo que se mantiene la complejidad lineal.

### SICT0302: Toma decisiones
#### Selecciona un algoritmo de ordenamiento adecuado al problema y lo usa correctamente.
El algoritmo de ordenamiento utilizado en el proyecto es RadixSort. La elección de este algoritmo de debió principalmente a su complejidad temporal lineal. Esta complejidad indica que es un algoritmo más rápido que otros como QuickSort (Promedio: `O(nlog(n))`), MergeSort (Promedio: `O(nlog(n))`), Bubble/Insertion sort (Promedio: `O(n^2)`). Esto se debe a que es un algoritmo de ordenamiento sin comparaciones, por lo que logra su complejidad temporal lineal. 

Como el problema se basa en ordenar 10,000 valores de tipo `float`, su complejidad espacial lineal no resulta en un problema, por lo que es un "sacrificio" válido para aprovechar su menor complejidad temporal.

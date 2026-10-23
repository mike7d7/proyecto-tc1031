# Proyecto: Osu-MM
[Osu!](https://osu.ppy.sh/) es un juego de ritmo inspirado en el juego [Osu! Tatakae! Ouendan](https://en.wikipedia.org/wiki/Osu!_Tatakae!_Ouendan) para la Nintendo DS. Osu!, al ser un juego de ritmo competitivo, cuenta con información acerca de las diferentes partidas jugadas por los jugadores. En base a la dificultad y calidad de cada partida, se le otorga al jugador un número de [puntos de rendimiento](https://osu.ppy.sh/wiki/en/Performance_points).

Estos puntos de rendimientos se vuelven un reflejo de la habilidad de cada jugador, por lo que el programa los utiliza para emparejar dos jugadores de habilidad similar para un juego multijugador justo (sistema [skill-based match making](https://en.wikipedia.org/wiki/Skill-based_matchmaking)).

## Librerías externas
En la carpeta `external-libs/` se encuentran códigos externos que se utilizan para facilitar el desarrollo del programa.
Las librerías utilizadas y su propósito son las siguientes:
- [rapidcsv](https://github.com/d99kris/rapidcsv/tree/cbd8a0a937b249cc07e2db3bfa9cd2cc1689708f): Utilizada para leer los valores de los archivos CSV de forma sencilla.
- [Dear ImGui](https://github.com/ocornut/imgui): Librería para la interfaz gráfica (GUI). Se agregó como un submódulo de git en la carpeta `external-libs/imgui`.
    - [SDL3](https://wiki.libsdl.org/SDL3/FrontPage): Backend/Renderizador elegido para la interfaz gráfica.
    - [Ejemplo ImGui-SDL3GPU](https://github.com/ocornut/imgui/tree/master/examples/example_sdl3_sdlgpu3): Sirvió como base para el código que conecta a Dear ImGui con SDL3 al igual que para el `Makefile`.

## Descripción del avance 1
El primer avance consiste en lo siguiente:
- Creación de clase `User`, que representa a un jugador.
- Función `main`, que genera la interfaz de usuario y utiliza la clase `User` para guardar la información de cada usuario, cargada de los archivos CSV.
- Funcionalidad de mostrar lista de jugadores.
- Funcionalidad de mostrar información detallada de un jugador al escribir su `user_id`.
- Funcionalidad de encontrar y mostrar un oponente con habilidad similar para un jugador.
- Algoritmo de ordenamiento basado en [RadixSort](https://en.wikipedia.org/wiki/Radix_sort) para el ordenamiento de los usuarios en base a sus puntos de rendimiento (necesario para encontrar oponentes).

## Descripción del avance 2
- Creación de clase `OsuMM`, la cual se usa para manejar funciones variadas que afectan a la lista de usuarios (ej. ordenarlos).
- Ahora se utiliza `std::unordered_map` como el contenedor con todos los usuarios (previamente se utilizaba un `vector`).

### Cambios sobre el primer avance
- Ahora se utiliza una interfaz gráfica de usuario (GUI) hecha con [Dear ImGui](https://github.com/ocornut/imgui).

## Instrucciones para compilar el avance de proyecto
Se asume que ya se tiene la librería de `SDL3` instalada.

Primero, clonar el proyecto y entrar a su carpeta (en una terminal).
Ejecutar los siguientes comandos en la terminal:

`git submodule update --init --recursive`

`make install-local`

## Instrucciones para ejecutar el avance de proyecto
Después de compilar el proyecto, ejecuta el siguiente comando en la terminal:

`./bin/osu-mm` 

## Descripción de las entradas del avance de proyecto
El programa requiere archivos en formato CSV en el mismo formato que la base de datos de osu!. En la carpeta `csv/` de este repositorio se encuentran los archivos de ejemplo. Los datos utilizados provienen de [esta entrada](https://data.ppy.sh/2026_09_01_performance_osu_random_10000.tar.bz2) en [data.ppy.sh](https://data.ppy.sh/). Los datos se exportaron a archivos CSV para facilitar su uso en el programa. Los datos originales se encuentran en forma de scripts de SQL.

## Descripción de las salidas del avance de proyecto
Las salidas dependen de la pestaña seleccionada en la ventana principal:
- "Lista de jugadores": Muestra una tabla con 40 jugadores con su respectivo `user_id`, se pueden mostrar los siguientes 40 jugadores con un botón de `Siguiente página`.
- "Información de jugador": Muestra información adicional (puntos de rendimiento y tiempo de juego) de un jugador dado su `user_id`. 
- "Encontrar oponente": Encuentra un oponente con una habilidad similar al usuario dado y muestra la información de ambos.

En caso de que el usuario ingrese valores inválidos (ej. ingresa -1 en el `user_id`) se mostrará un mensaje de error.

## Desarrollo de competencias
### SICT0301: Evalúa los componentes
#### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa.
En la línea 78 del archivo `main.cpp` se encuentra mi función de ordenamiento `sort_users_by_skill()`, la cual se basa en RadixSort. Esta función tiene una complejidad temporal de `O(nk)`, donde `n` es el número de elementos en el arreglo y `k` es el número de digitos usados para ordenar, como en este caso, se ordenan exclusivamente valores de tipo `float` (de 32 bits) y el algoritmo los procesa en 4 grupos de 8 bits, la variable `k` tiene un valor de 4. Cómo vimos en clase, esta complejidad es lineal y se simplifica a `O(n)`. Esta complejidad lineal se debe a que el iterador del bucle más externo va de 0 a 32 de 8 en 8. Dentro de este bucle, se encuentran dos bucles que iteran sobre cada elemento del arreglo a ordenar, sin embargo estos bucles no están anidados, por lo que se evita una complejidad cuadrática.

Como el algoritmo debe cumplir todos los ciclos sin importar el orden inicial del arreglo, esto hace que su complejidad sea igual en los tres casos.
- Mejor caso: `O(n)`
- Caso promedio: `O(n)`
- Peor caso: `O(n)`

En complejidad espacial, este algoritmo tiene complejidad `O(n)` (lineal). Esto se debe a que se crea un arreglo (llamado `buffer`) del mismo tamaño que el arreglo a ordenar. Los arreglos adicionales utilizados para almacenar las radixes tienen un tamaño constante, por lo que se mantiene la complejidad lineal.

#### Hace un análisis de complejidad correcto y completo de todas las estructuras de datos y cada uno de sus usos en el programa.
En el programa utilizo dos estructuras de datos:
- Vectores (`<vector>`)
- HashMap (`<unordered_map>`)
Estas estructuras cuentan con las siguientes complejidades temporales:

**Mejor caso:**
| Operación | Vector | HashMap |
|---|---|---|
| Acceso con índice | O(1) | N/A |
| Búsqueda | O(1) | O(1) |
| Inserción | O(1) | O(1) |
| Borrado | O(1) | O(1) |

En el vector, el mejor caso en búsqueda sucede cuando el valor buscado es el primer elemento. Para la inserción sucede cuando se no se tienen que mover otros elementos ni cambiar su tamaño. En el borrado sucede cuando no se tienen que recorrer los demás elementos (ej. el último elemento).

**Promedio:**
| Operación | Vector | HashMap |
|---|---|---|
| Acceso con índice | O(1) | N/A |
| Búsqueda | O(n) | O(1) |
| Inserción | O(n) | O(1) |
| Borrado | O(n) | O(1) |

**Peor caso:**
| Operación | Vector | HashMap |
|---|---|---|
| Acceso con índice | O(1) | N/A |
| Búsqueda | O(n) | O(n) |
| Inserción | O(n) | O(n) |
| Borrado | O(n) | O(n) |

En el vector esto sucede cuando el elemento que se necesita es el último, por lo que se tiene que iterar sobre todos los elementos. En el HashMap sucede cuando existen colisiones entre las llaves por lo que se termina iterando sobre múltiples elementos, y en el peor caso, sobre todos los elementos.

La complejidad espacial de ambas estructuras es `O(n)` debido a que su tamaño escala con el número de elementos que contengan.

### SICT0302: Toma decisiones
#### Selecciona un algoritmo de ordenamiento adecuado al problema y lo usa correctamente.
El algoritmo de ordenamiento utilizado en el proyecto es RadixSort. La elección de este algoritmo de debió principalmente a su complejidad temporal lineal. Esta complejidad indica que es un algoritmo más rápido que otros como QuickSort (Promedio: `O(nlog(n))`), MergeSort (Promedio: `O(nlog(n))`), Bubble/Insertion sort (Promedio: `O(n^2)`). Esto se debe a que es un algoritmo de ordenamiento sin comparaciones, por lo que logra su complejidad temporal lineal. 

Como el problema se basa en ordenar 10,000 valores de tipo `float`, su complejidad espacial lineal no resulta en un problema, por lo que es un "sacrificio" válido para aprovechar su menor complejidad temporal.

#### Selecciona una estructura de datos adecuada al problema y la usa correctamente.
La estructura de datos principal que utilicé en el programa es el HashMap (`unordered_map`) con el fin de guardar a todos los usuarios. Mi razón para utilizar esta estructura es que en el programa, la acción que más se realiza es consultar la información de un usuario en base a su ID. Si guardo todos los usuarios en un vector, tendría que iterar sobre todos los usuarios hasta encontrar uno que tenga el mismo ID que estoy buscando, esto daría una complejidad temporal de `O(n)`. En cambio, con el HashMap, el caso promedio tiene una complejidad constante `O(1)` ya que puedo utilizar su ID como una llave para acceder al elemento sin iterar sobre los demás elementos.

### SICT0303: Implementa acciones científicas
#### Implementa mecanismos para consultar información de las estructras correctos.
El programa consulta y muestra la información general de múltiples usuarios en la pestaña de "Lista de jugadores". En la pestaña de "Información de jugador" se busca un jugador en base a su ID y se muestra su información específica, esto también sucede en la pestaña de "Encontrar oponente", en donde se muestra la información del jugador especificado, al igual que la información del oponente el cual se encuentra buscando otro jugador con un valor similar de puntos de rendimiento.

#### Implementa mecanismos de lectura de archivos para cargar datos a las estructuras de manera correcta.
El programa carga al iniciar (línea 143 en `main.cpp`) los datos de los usuarios desde archivos csv (descritos a detalle en las entradas del poyecto), los cuales están basados en la base de datos real de osu!. 

Los datos se guardan en objetos de la clase `User`, la cual representa a un jugador junto a sus datos importantes (nombre de usuario, puntos de rendiemiento, tiempo de juego), los cuales son guardados como atributos. El conjunto de todos los `User`s con sus respectivos datos son guardados en el atributo `users` de la clase `OsuMM`.

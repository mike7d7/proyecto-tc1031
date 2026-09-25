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

## Instrucciones para compilar el avance de proyecto
Primero, clonar el proyecto y entrar a su carpeta (en una terminal).
Ejecutar el siguiente comando en la terminal:

`g++ -std=c++11 main.cpp "src/user.cpp" -o osu-mm` 

## Instrucciones para ejecutar el avance de proyecto
Después de compilar el proyecto, ejecuta el siguiente comando en la terminal:

`./osu-mm` 

## Descripción de las entradas del avance de proyecto
Escribe aquí la descripción de las entradas del proyecto, por ejemplo, si de entrada se requieren varios archivos, hay que indicar el formato de cada uno de ellos, y proporcionar un ejemplo de los datos de cada archivo.


## Descripción de las salidas del avance de proyecto
Escribe aquí la descripción de los resultados de la ejecución de tu programa.

## Desarrollo de competencias

### SICT0301: Evalúa los componentes
#### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa.
Escribe aquí tus argumentos sobre por qué consideras que has desarrrollado esta competencia y dónde se puede observar el desarrollo que mencionas.

### SICT0302: Toma decisiones
#### Selecciona un algoritmo de ordenamiento adecuado al problema y lo usa correctamente.
Escribe aquí tus argumentos sobre por qué consideras que has desarrrollado esta competencia y dónde se puede observar el desarrollo que mencionas.



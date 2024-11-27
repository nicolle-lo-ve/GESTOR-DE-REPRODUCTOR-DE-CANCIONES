# Gestión de Canciones - README

Este proyecto implementa un sistema para la gestión y manipulación de canciones utilizando diversas estructuras de datos. A continuación se describen las estructuras clave utilizadas en el código, sus ventajas y desventajas, así como su uso principal.

## Estructuras de Datos

### 1. Estructura `Cancion`
- **Descripción**: Representa una canción y contiene atributos como el nombre del artista, el título, popularidad, año de lanzamiento, género y otros atributos musicales.
- **Uso**: Almacena la información de cada canción cargada desde un archivo CSV.

### 2. Lista Enlazada (`NodoCancion`)
- **Descripción**: Cada nodo contiene un objeto `Cancion` y un puntero al siguiente nodo.
- **Uso**: Facilita la inserción y eliminación dinámica de canciones en la lista de reproducción. La cabeza de la lista es un puntero que apunta al primer nodo.

### 3. Vector (`vector<Cancion>`)
- **Descripción**: Utilizado en la clase `ListaReproduccion` para almacenar todas las canciones en un vector.
- **Uso**: Permite acceso rápido a las canciones y es eficiente para operaciones como añadir nuevas canciones al final.

### 4. Mapa Desordenado (`unordered_map<string, Cancion*>`)
- **Descripción**: Mapea el nombre de cada canción a su objeto `Cancion`.
- **Uso**: Proporciona acceso rápido a las canciones por su nombre, mejorando la eficiencia en operaciones como eliminación o búsqueda.

### 5. Árbol B Multinivel (`NodoArbolB` y `ArbolBMultinivel`)
- **Descripción**: Organiza las canciones en un árbol B, donde cada nodo puede contener múltiples canciones y punteros a nodos hijos.
- **Uso**: Permite mantener las canciones ordenadas por popularidad y realizar búsquedas eficientes. Facilita la inserción y división de nodos cuando se excede el número máximo de elementos.

## Comparación entre Estructuras

| Estructura         | Ventajas                                         | Desventajas                                    | Uso Principal                              |
|--------------------|--------------------------------------------------|------------------------------------------------|--------------------------------------------|
| `Cancion`          | Almacena información detallada sobre cada canción | No es una estructura por sí sola               | Representar datos individuales             |
| `NodoCancion`      | Inserción/eliminación eficiente                   | Acceso secuencial; no eficiente para búsquedas | Manejo dinámico de listas de reproducción  |
| `Vector`           | Acceso rápido y fácil gestión de memoria         | Redimensionamiento costoso si se requiere     | Almacenar colecciones dinámicas            |
| `Unordered Map`    | Acceso rápido a elementos por clave              | Mayor uso de memoria comparado con listas      | Búsquedas rápidas por nombre               |
| `Árbol B Multinivel` | Búsquedas rápidas; mantiene datos ordenados      | Más compleja; requiere más memoria             | Almacenamiento eficiente y búsqueda rápida |

## Conclusión

El código hace uso efectivo de diversas estructuras de datos para optimizar la gestión de una lista de reproducción musical. La combinación de listas enlazadas, vectores y árboles B permite realizar operaciones eficientes en términos de tiempo y espacio, mejorando así la experiencia del usuario al interactuar con las canciones.

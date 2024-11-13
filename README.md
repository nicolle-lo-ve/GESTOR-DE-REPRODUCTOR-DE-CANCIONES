# GESTOR-DE-REPRODUCTOR-DE-CANCIONES

Este código implementa una lista de reproducción de canciones utilizando una estructura de datos de árbol B. Permite cargar canciones desde un archivo CSV, agregar, eliminar y mover canciones, y listar las canciones de diferentes formas (por popularidad, por año). La separación de responsabilidades entre las clases `Cancion`, `Nodo`, `BTree` y `ListaReproduccion` proporciona una arquitectura modular y escalable. El menú interactivo en la función `main()` facilita la interacción del usuario con la aplicación.

## Sección de Includes y Namespaces

El código comienza con la inclusión de los encabezados necesarios: 
- `iostream`
- `vector`
- `fstream`
- `sstream`
- `string`
- `algorithm`
- `random`
- `stdexcept`

Se utiliza el espacio de nombres `std` para evitar tener que escribir `std::` antes de cada nombre de función o tipo.

## Definición de la clase `Cancion`

Esta clase representa una sola canción en la lista de reproducción. Contiene los siguientes atributos:

- `artist_name`: Nombre del artista.
- `track_name`: Título de la canción.
- `track_id`: Identificador único de la canción.
- `popularity`: Popularidad de la canción.
- `anio`: Año de lanzamiento de la canción.
- `genre`: Género musical de la canción.
- `danceability`, `energy`, `key`, `loudness`, `mode`, `speechiness`, `acousticness`, `instrumentalness`, `liveness`, `valence`, `tempo`, `duration_ms`: Características de audio de la canción.

### Constructores

- **Constructor por defecto**: Inicializa todos los atributos a valores predeterminados.
- **Constructor con parámetros**: Permite inicializar todos los atributos de la canción.

### Sobrecarga de Operadores

Implementa los operadores `>` y `==` para comparar canciones por su título y ID, respectivamente.

## Definición de la clase `Nodo`

Esta clase representa un nodo en el árbol B que se utiliza para almacenar las canciones. Contiene los siguientes atributos:

- `canciones`: Vector que almacena las canciones del nodo.
- `hijos`: Vector de punteros a nodos hijos.
- `tamano_maximo`: Tamaño máximo del vector de canciones que puede contener el nodo.
- `es_hoja`: Bandera que indica si el nodo es una hoja o no.

### Métodos

- **Constructor**: Inicializa el tamaño máximo del nodo.
- **Destructor**: Libera la memoria de los nodos hijos.
- **Métodos**:
  - `insertar_no_lleno()`: Inserta una nueva canción en el nodo sin exceder el tamaño máximo.
  - `dividir_hijo()`: Divide un nodo hijo en dos nodos cuando se excede el tamaño máximo.

## Definición de la clase `BTree`

Esta clase representa el árbol B que almacena todas las canciones. Contiene los siguientes atributos:

- `raiz`: Puntero al nodo raíz del árbol B.
- `tamano_maximo`: Tamaño máximo de los nodos del árbol B.

### Métodos

- **Constructor**: Inicializa la raíz del árbol B con un nodo vacío y el tamaño máximo de los nodos.
- **Destructor**: Libera la memoria de la raíz del árbol B.
- **Métodos**:
  - `insertar()`: Inserta una nueva canción en el árbol B, dividiendo nodos si es necesario.
  - `eliminar()`: Elimina una canción del árbol B (la implementación completa no está incluida).
  - `mover_cancion()`: Mueve una canción a una nueva posición en el árbol B.
  - `listar()`: Devuelve un vector con todas las canciones del árbol B.
  - `listar_por_popularidad()`: Devuelve un vector con las canciones ordenadas por popularidad.
  - `obtener_por_anio()`: Devuelve un vector con las canciones filtradas por año.
  - `_listar()`: Función auxiliar recursiva utilizada por `listar()` para recorrer el árbol B y obtener todas las canciones.

## Definición de la clase `ListaReproduccion`

Esta clase envuelve a la clase `BTree` y proporciona una interfaz de alto nivel para interactuar con la lista de reproducción. Contiene un atributo `bTree` que es una instancia de la clase `BTree`.

### Métodos

- `agregar_cancion()`: Inserta una nueva canción en la lista de reproducción.
- `listar_canciones()`: Devuelve un vector con todas las canciones de la lista de reproducción.
- `listar_por_popularidad()`: Devuelve un vector con las canciones ordenadas por popularidad.
- `obtener_por_anio()`: Devuelve un vector con las canciones filtradas por año.

## Función `cargar_csv()`

Esta función se encarga de cargar las canciones desde un archivo CSV y almacenarlas en una instancia de `ListaReproduccion`. 

### Funcionamiento

   1. Abre el archivo CSV especificado y verifica que se haya abierto correctamente.
   2. Lee la cabecera del archivo CSV y luego procesa cada línea del archivo.
   3. Para cada línea, crea una instancia de `Cancion` con los datos extraídos y la agrega a la `ListaReproduccion`.
   4. Maneja excepciones en caso de que haya problemas al procesar alguna línea.
   5. Finalmente, devuelve un vector con todas las canciones cargadas.

## Implementación de los Métodos de `BTree`

- **insertar()**: Verifica si la raíz del árbol está llena. Si es así, crea un nuevo nodo raíz y divide el nodo raíz original. Luego, llama al método `insertar_no_lleno()` del nodo raíz para insertar la nueva canción.
- **eliminar()**: Implementa el esqueleto básico para eliminar una canción del árbol B. Recorre el vector de canciones en el nodo raíz y, si encuentra la canción, la elimina. La implementación completa no está incluida.
- **mover_cancion()**: Busca la canción con el ID especificado, la elimina y luego la inserta en la nueva posición.
- **_listar()**: Función auxiliar recursiva que recorre el árbol B y agrega todas las canciones a un vector.
- **listar()**: Llama a la función `_listar()` pasando la raíz del árbol B y un vector vacío, y devuelve el vector resultante con todas las canciones.
- **listar_por_popularidad()**: Llama al método `listar()` para obtener todas las canciones, las ordena por popularidad y devuelve el vector resultante.
- **obtener_por_anio()**: Llama al método `listar()` para obtener todas las canciones, las filtra por año y devuelve un vector con las canciones que coinciden.

## Función `main()`

Implementa un bucle principal que muestra un menú de opciones al usuario. Las opciones incluyen:

   1. Cargar canciones desde un archivo CSV.
   2. Listar todas las canciones.
   3. Listar canciones por popularidad.
   4. Buscar canciones por año.
   5. Agregar una canción manualmente.
   6. Eliminar una canción.
   7. Mover una canción.
   8. Salir del programa.

### Manejo de Errores

Cada opción del menú llama a los métodos correspondientes de la clase `ListaReproduccion` para realizar las operaciones solicitadas. Se maneja excepciones en caso de que ocurran errores durante la ejecución.

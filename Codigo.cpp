#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <locale>
#include <cctype>

using namespace std;

class TrieNode {
public:
    unordered_map<char, unique_ptr<TrieNode>> hijos;
    vector<string> track_ids;
    bool fin_palabra = false;

    void insertar(const string& palabra, const string& track_id) {
        TrieNode* nodo_actual = this;
        for (char c : palabra) {
            c = tolower(c);
            if (!nodo_actual->hijos[c]) {
                nodo_actual->hijos[c] = make_unique<TrieNode>();
            }
            nodo_actual = nodo_actual->hijos[c].get();
        }
        nodo_actual->fin_palabra = true;
        nodo_actual->track_ids.push_back(track_id);
    }

    vector<string> buscar_prefijo(const string& prefijo) {
        TrieNode* nodo_actual = this;
        for (char c : prefijo) {
            c = tolower(c);
            if (!nodo_actual->hijos[c]) return {};
            nodo_actual = nodo_actual->hijos[c].get();
        }
        return recolectar_ids(nodo_actual);
    }

private:
    vector<string> recolectar_ids(TrieNode* nodo) {
        vector<string> resultados;
        if (nodo->fin_palabra) {
            resultados.insert(
                resultados.end(), 
                nodo->track_ids.begin(), 
                nodo->track_ids.end()
            );
        }
        for (auto& par : nodo->hijos) {
            auto ids_hijo = recolectar_ids(par.second.get());
            resultados.insert(
                resultados.end(), 
                ids_hijo.begin(), 
                ids_hijo.end()
            );
        }
        return resultados;
    }
};

// Estructura de Canción modificada para incluir time_signature
class Cancion {
public:
    string artist_name;
    string track_name;
    string track_id;
    int popularity;
    int anio;
    string genre;
    float danceability;
    float energy;
    int key;
    float loudness;
    int mode;
    float speechiness;
    float acousticness;
    float instrumentalness;
    float liveness;
    float valence;
    float tempo;
    int duration_ms;
    int time_signature; 

    // Constructor predeterminado
    Cancion() = default;

    // Constructor completo
    Cancion(string artist_name, string track_name, string track_id, int popularity, 
            int anio, string genre, float danceability, float energy, int key, 
            float loudness, int mode, float speechiness, float acousticness, 
            float instrumentalness, float liveness, float valence, 
            float tempo, int duration_ms, int time_signature = 4) 
        : artist_name(move(artist_name)), track_name(move(track_name)), 
          track_id(move(track_id)), popularity(popularity), anio(anio), 
          genre(move(genre)), danceability(danceability), energy(energy), 
          key(key), loudness(loudness), mode(mode), speechiness(speechiness), 
          acousticness(acousticness), instrumentalness(instrumentalness), 
          liveness(liveness), valence(valence), tempo(tempo), 
          duration_ms(duration_ms), time_signature(time_signature) {}

    // Operadores de comparación
    bool operator>(const Cancion& otra) const {
        return track_name > otra.track_name;
    }

    bool operator==(const Cancion& otra) const {
        return track_id == otra.track_id;
    }
};

// Clase Nodo optimizada
class Nodo {
public:
    vector<Cancion> canciones;
    vector<unique_ptr<Nodo>> hijos;
    const int tamano_maximo;
    bool es_hoja;

    explicit Nodo(int tam_max) : tamano_maximo(tam_max), es_hoja(true) {
        canciones.reserve(tam_max);
        hijos.reserve(tam_max + 1);
    }

    void insertar_no_lleno(const Cancion& cancion) {
        int i = static_cast<int>(canciones.size()) - 1;

        if (es_hoja) {
            while (i >= 0 && canciones[i] > cancion) {
                i--;
            }
            canciones.insert(canciones.begin() + i + 1, cancion);
        } else {
            while (i >= 0 && canciones[i] > cancion) {
                i--;
            }
            i++;
            if (hijos[i]->canciones.size() == tamano_maximo) {
                dividir_hijo(i);
                if (cancion > canciones[i]) {
                    i++;
                }
            }
            hijos[i]->insertar_no_lleno(cancion);
        }
    }

    void dividir_hijo(int indice) {
        auto& hijo = hijos[indice];
        auto nuevo_hijo = make_unique<Nodo>(tamano_maximo);
        nuevo_hijo->es_hoja = hijo->es_hoja;

        // Mover la mitad de las canciones al nuevo hijo
        int mitad = tamano_maximo / 2;
        nuevo_hijo->canciones.assign(
            make_move_iterator(hijo->canciones.begin() + mitad),
            make_move_iterator(hijo->canciones.end())
        );
        hijo->canciones.resize(mitad);

        // Si no es hoja, mover también los hijos correspondientes
        if (!hijo->es_hoja) {
            nuevo_hijo->hijos.assign(
                make_move_iterator(hijo->hijos.begin() + mitad),
                make_move_iterator(hijo->hijos.end())
            );
            hijo->hijos.resize(mitad);
        }

        // Insertar la canción del medio en el nodo actual
        canciones.insert(canciones.begin() + indice, move(hijo->canciones.back()));
        hijo->canciones.pop_back();

        // Insertar el nuevo hijo
        hijos.insert(hijos.begin() + indice + 1, move(nuevo_hijo));
    }

    optional<Cancion> buscar(const string& track_id) const {
        auto it = find_if(canciones.begin(), canciones.end(),
            [&track_id](const Cancion& c) { return c.track_id == track_id; });
        
        if (it != canciones.end()) {
            return *it;
        }

        if (!es_hoja) {
            for (const auto& hijo : hijos) {
                auto resultado = hijo->buscar(track_id);
                if (resultado.has_value()) {
                    return resultado;
                }
            }
        }

        return nullopt;
    }

    bool eliminar(const string& track_id) {
        auto it = find_if(canciones.begin(), canciones.end(),
            [&track_id](const Cancion& c) { return c.track_id == track_id; });
        
        if (it != canciones.end()) {
            canciones.erase(it);
            return true;
        }

        if (!es_hoja) {
            for (auto& hijo : hijos) {
                if (hijo->eliminar(track_id)) {
                    return true;
                }
            }
        }

        return false;
    }
};

vector<Cancion> cargar_csv_por_prefijo(const string& file_path, const string& prefijo, bool por_artista) {
    vector<Cancion> canciones;
    ifstream file(file_path, ios::binary);
   
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + file_path);
    }

    string linea;
    getline(file, linea); // Saltar encabezado

    // Crear una localización para la conversión a minúsculas
    std::locale loc;

    while (getline(file, linea)) {
        vector<string> campos;
        size_t pos = 0;
        size_t next_pos = 0;
        while ((next_pos = linea.find(',', pos)) != string::npos) {
            campos.push_back(linea.substr(pos, next_pos - pos));
            pos = next_pos + 1;
        }
        campos.push_back(linea.substr(pos));

        // Verificar si comienza con el prefijo
        string texto_busqueda = por_artista ? campos[1] : campos[2];
       
        // Convertir a minúsculas para búsqueda insensible a mayúsculas
        transform(texto_busqueda.begin(), texto_busqueda.end(), texto_busqueda.begin(),
                  [&loc](char c) { return std::tolower(c, loc); });
        string prefijo_lower = prefijo;
        transform(prefijo_lower.begin(), prefijo_lower.end(), prefijo_lower.begin(),
                  [&loc](char c) { return std::tolower(c, loc); });

        // Solo agregar si comienza con el prefijo
        if (texto_busqueda.substr(0, prefijo_lower.length()) == prefijo_lower) {
            try {
                auto safe_stoi = [](const string& s) {
                    return s.empty() ? 0 : stoi(s);
                };

                auto safe_stof = [](const string& s) {
                    return s.empty() ? 0.0f : stof(s);
                };

                canciones.emplace_back(
                    move(campos[1]), // artist_name
                    move(campos[2]), // track_name
                    move(campos[3]), // track_id
                    safe_stoi(campos[4]), // popularity
                    safe_stoi(campos[5]), // anio
                    move(campos[6]), // genre
                    safe_stof(campos[7]), // danceability
                    safe_stof(campos[8]), // energy
                    safe_stoi(campos[9]), // key
                    safe_stof(campos[10]), // loudness
                    safe_stoi(campos[11]), // mode
                    safe_stof(campos[12]), // speechiness
                    safe_stof(campos[13]), // acousticness
                    safe_stof(campos[14]), // instrumentalness
                    safe_stof(campos[15]), // liveness
                    safe_stof(campos[16]), // valence
                    safe_stof(campos[17]), // tempo
                    safe_stoi(campos[18]), // duration_ms
                    4 // default time_signature
                );
            } catch (const exception& e) {
                // Ignorar errores de conversión
            }
        }
    }

    return canciones;
}

// Clase BTree optimizada
class BTree {
public:
    unique_ptr<Nodo> raiz;
    const int tamano_maximo;
    unordered_map<string, size_t> indice_por_id;

    explicit BTree(int tam_max) : tamano_maximo(tam_max) {
        raiz = make_unique<Nodo>(tam_max);
    }

    void insertar(const Cancion& cancion) {
        if (raiz->canciones.size() == tamano_maximo) {
            auto nuevo_raiz = make_unique<Nodo>(tamano_maximo);
            nuevo_raiz->es_hoja = false;
            nuevo_raiz->hijos.push_back(move(raiz));
            raiz = move(nuevo_raiz);
            raiz->dividir_hijo(0);
        }
        raiz->insertar_no_lleno(cancion);
        indice_por_id[cancion.track_id] = indice_por_id.size();
    }

    bool eliminar(const string& track_id) {
        bool resultado = raiz->eliminar(track_id);
        if (resultado) {
            indice_por_id.erase(track_id);
        }
        return resultado;
    }

    optional<Cancion> buscar(const string& track_id) const {
        return raiz->buscar(track_id);
    }

    void mover_cancion(const string& track_id, size_t nueva_posicion) {
        auto cancion_opt = buscar(track_id);
        if (!cancion_opt) {
            throw runtime_error("Canción no encontrada");
        }

        if (nueva_posicion >= indice_por_id.size()) {
            throw runtime_error("Posición inválida");
        }

        eliminar(track_id);
        insertar(cancion_opt.value());
        // Actualizar índices
        size_t idx = 0;
        for (const auto& cancion : listar()) {
            indice_por_id[cancion.track_id] = idx++;
        }
    }

    vector<Cancion> listar() const {
        vector<Cancion> resultado;
        resultado.reserve(indice_por_id.size());
        _listar(raiz.get(), resultado);
        return resultado;
    }

    vector<Cancion> listar_por_popularidad(bool ascendente = true) const {
        auto canciones = listar();
        sort(canciones.begin(), canciones.end(),
            [ascendente](const Cancion& a, const Cancion& b) {
                return ascendente ? 
                    a.popularity < b.popularity : 
                    a.popularity > b.popularity;
            });
        return canciones;
    }

    vector<Cancion> obtener_por_anio(int anio) const {
        vector<Cancion> resultado;
        auto todas = listar();
        copy_if(todas.begin(), todas.end(), back_inserter(resultado),
            [anio](const Cancion& c) { return c.anio == anio; });
        return resultado;
    }

    vector<Cancion> listar_por_duracion(bool ascendente = true) const {
        auto canciones = listar();
        sort(canciones.begin(), canciones.end(),
            [ascendente](const Cancion& a, const Cancion& b) {
                return ascendente ? 
                    a.duration_ms < b.duration_ms : 
                    a.duration_ms > b.duration_ms;
            });
        return canciones;
    }

private:
    void _listar(const Nodo* nodo, vector<Cancion>& resultado) const {
        if (!nodo) return;

        for (size_t i = 0; i < nodo->canciones.size(); i++) {
            if (!nodo->es_hoja && i < nodo->hijos.size()) {
                _listar(nodo->hijos[i].get(), resultado);
            }
            resultado.push_back(nodo->canciones[i]);
        }

        if (!nodo->es_hoja && !nodo->hijos.empty()) {
            _listar(nodo->hijos.back().get(), resultado);
        }
    }
};

// Clase ListaReproduccion combinada
class ListaReproduccion {
public:
    BTree bTree;
    TrieNode trie_artistas;
    TrieNode trie_canciones;
    size_t total_canciones;
    vector<Cancion> csv_canciones;

    explicit ListaReproduccion(int tamano_maximo = 3) 
        : bTree(tamano_maximo), total_canciones(0) {}

    // New method to find songs in the loaded CSV data
    vector<Cancion> buscar_canciones_por_prefijo_en_csv(const string& prefijo, bool por_artista = false) {
        string file_path = "spotify_data.csv";
       
        try {
            return cargar_csv_por_prefijo(file_path, prefijo, por_artista);
        } catch (const runtime_error& e) {
            cerr << e.what() << '\n';
            return {};
        }
    }

    void agregar_cancion(const Cancion& cancion) {
        bTree.insertar(cancion);
        trie_artistas.insertar(cancion.artist_name, cancion.track_id);
        trie_canciones.insertar(cancion.track_name, cancion.track_id);
        total_canciones++;
    }

    vector<Cancion> listar_canciones() const {
        return bTree.listar();
    }

    vector<Cancion> listar_por_popularidad(bool ascendente = true) const {
        return bTree.listar_por_popularidad(ascendente);
    }

    vector<Cancion> obtener_por_anio(int anio) const {
        return bTree.obtener_por_anio(anio);
    }

    bool eliminar_cancion(const string& track_id) {
        if (bTree.eliminar(track_id)) {
            total_canciones--;
            return true;
        }
        return false;
    }

    void reproducir_aleatoria() const {
        auto canciones = listar_canciones();
        if (canciones.empty()) {
            cout << "La lista de reproducción está vacía." << endl;
            return;
        }
        
        srand(static_cast<unsigned>(time(nullptr)));
        int indice_aleatorio = rand() % canciones.size();
        const Cancion& cancion = canciones[indice_aleatorio];
        
        cout << "Reproduciendo: " << cancion.track_name 
             << " - " << cancion.artist_name << endl;
    }
    
        // En la clase ListaReproduccion

    vector<Cancion> buscar_canciones_por_trie(const string& prefijo, bool por_artista = false) {
        vector<Cancion> resultados_playlist;
        TrieNode& trie = por_artista ? trie_artistas : trie_canciones;

        // Obtener IDs de canciones usando el Trie
        vector<string> track_ids = trie.buscar_prefijo(prefijo);

        for (const auto& track_id : track_ids) {
            auto cancion = bTree.buscar(track_id);
            if (cancion) {
                // Verificar si la cadena comienza exactamente con el prefijo
                bool cumple_prefijo = por_artista ?
                    (cancion->artist_name.substr(0, prefijo.length()) == prefijo) :
                    (cancion->track_name.substr(0, prefijo.length()) == prefijo);

                if (cumple_prefijo) {
                    resultados_playlist.push_back(*cancion);
                }
            }
        }

        return resultados_playlist;
    }

    bool eliminar_cancion_por_nombre(const string& nombre, bool por_artista = false) {
        auto canciones = buscar_canciones_por_trie(nombre, por_artista);
        
        if (canciones.empty()) {
            cout << "No se encontraron canciones.\n";
            return false;
        }
        
        // Si hay múltiples canciones, mostrar opciones
        if (canciones.size() > 1) {
            cout << "Se encontraron múltiples canciones:\n";
            for (size_t i = 0; i < canciones.size(); ++i) {
                cout << i + 1 << ". " 
                     << canciones[i].track_name 
                     << " - " << canciones[i].artist_name 
                     << " (ID: " << canciones[i].track_id << ")\n";
            }
            
            size_t seleccion;
            cout << "Seleccione el número de la canción a eliminar: ";
            cin >> seleccion;
            
            if (seleccion < 1 || seleccion > canciones.size()) {
                cout << "Selección inválida.\n";
                return false;
            }
            
            return eliminar_cancion(canciones[seleccion - 1].track_id);
        }
        
        // Si solo hay una canción
        return eliminar_cancion(canciones[0].track_id);
    }
    
    void mover_cancion_por_nombre(const string& nombre, size_t nueva_posicion, bool por_artista = false) {
        auto canciones = buscar_canciones_por_trie(nombre, por_artista);
        
        if (canciones.empty()) {
            cout << "No se encontraron canciones.\n";
            return;
        }
        
        // Si hay múltiples canciones, mostrar opciones
        if (canciones.size() > 1) {
            cout << "Se encontraron múltiples canciones:\n";
            for (size_t i = 0; i < canciones.size(); ++i) {
                cout << i + 1 << ". " 
                     << canciones[i].track_name 
                     << " - " << canciones[i].artist_name 
                     << " (ID: " << canciones[i].track_id << ")\n";
            }
            
            size_t seleccion;
            cout << "Seleccione el número de la canción a mover: ";
            cin >> seleccion;
            
            if (seleccion < 1 || seleccion > canciones.size()) {
                cout << "Selección inválida.\n";
                return;
            }
            
            try {
                bTree.mover_cancion(canciones[seleccion - 1].track_id, nueva_posicion);
                cout << "Canción movida.\n";
            } catch (const exception& e) {
                cout << "Error: " << e.what() << "\n";
            }
            return;
        }
        
        // Si solo hay una canción
        try {
            bTree.mover_cancion(canciones[0].track_id, nueva_posicion);
            cout << "Canción movida.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
    
    struct Pagina {
        vector<Cancion> canciones;
        size_t total_canciones;
        size_t pagina_actual;
        size_t total_paginas;
    };

    Pagina listar_canciones_paginado(size_t pagina = 1, size_t canciones_por_pagina = 200) const {
        auto todas_canciones = bTree.listar();
        return paginar(todas_canciones, pagina, canciones_por_pagina);
    }

    Pagina listar_por_popularidad_paginado(bool ascendente = true, size_t pagina = 1, size_t canciones_por_pagina = 200) const {
        auto canciones = bTree.listar_por_popularidad(ascendente);
        return paginar(canciones, pagina, canciones_por_pagina);
    }

    Pagina obtener_por_anio_paginado(int anio, size_t pagina = 1, size_t canciones_por_pagina = 200) const {
        auto canciones = bTree.obtener_por_anio(anio);
        return paginar(canciones, pagina, canciones_por_pagina);
    }

    Pagina listar_por_duracion_paginado(bool ascendente = true, size_t pagina = 1, size_t canciones_por_pagina = 200) const {
        auto canciones = bTree.listar_por_duracion(ascendente);
        return paginar(canciones, pagina, canciones_por_pagina);
    }

  private:

    Pagina paginar(const vector<Cancion>& canciones, size_t pagina, size_t canciones_por_pagina) const {
        size_t total_canciones = canciones.size();
        size_t total_paginas = (total_canciones + canciones_por_pagina - 1) / canciones_por_pagina;

        // Validar página
        pagina = min(max(pagina, static_cast<size_t>(1)), total_paginas);

        // Calcular rango de canciones para la página
        size_t inicio = (pagina - 1) * canciones_por_pagina;
        size_t fin = min(inicio + canciones_por_pagina, total_canciones);

        vector<Cancion> canciones_pagina(
            canciones.begin() + inicio, 
            canciones.begin() + fin
        );

        return {
            move(canciones_pagina),
            total_canciones,
            pagina,
            total_paginas
        };
    }
};

// Optimización de carga de CSV
vector<Cancion> cargar_csv(const string& file_path) {
    vector<Cancion> canciones;
    
    // Abrir el archivo con ifstream en modo binario para mayor eficiencia
    ifstream file(file_path, ios::binary | ios::ate);
    
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + file_path);
    }

    // Obtener tamaño del archivo de manera más eficiente
    streamsize file_size = file.tellg();
    file.seekg(0, ios::beg);

    // Reserva de memoria con tamaño más preciso
    canciones.reserve(file_size / 250);  // Estimación más ajustada

    // Buffer para lectura de líneas
    string linea;
    linea.reserve(300);  // Reservar memoria para cada línea

    // Saltar encabezado
    getline(file, linea);

    // Usar vectores de strings con reserva para parsing
    vector<string> campos;
    campos.reserve(20);

    // Contador de progreso y medición de tiempo
    size_t contador = 0;
    auto inicio = chrono::high_resolution_clock::now();

    // Parsing más rápido con parsing manual
    while (getline(file, linea)) {
        campos.clear();
        
        // Parsing de CSV manual más rápido que stringstream
        size_t pos = 0;
        size_t next_pos = 0;
        while ((next_pos = linea.find(',', pos)) != string::npos) {
            campos.push_back(linea.substr(pos, next_pos - pos));
            pos = next_pos + 1;
        }
        // Añadir último campo
        campos.push_back(linea.substr(pos));

        // Parseo robusto con conversiones seguras
        try {
            // Funciones lambda para conversiones seguras
            auto safe_stoi = [](const string& s) { 
                return s.empty() ? 0 : stoi(s); 
            };

            auto safe_stof = [](const string& s) { 
                return s.empty() ? 0.0f : stof(s); 
            };

            // Construcción de Cancion con move semántics
            canciones.emplace_back(
                move(campos[1]),    // artist_name
                move(campos[2]),    // track_name
                move(campos[3]),    // track_id
                safe_stoi(campos[4]),   // popularity
                safe_stoi(campos[5]),   // anio
                move(campos[6]),    // genre
                safe_stof(campos[7]),   // danceability
                safe_stof(campos[8]),   // energy
                safe_stoi(campos[9]),   // key
                safe_stof(campos[10]),  // loudness
                safe_stoi(campos[11]),  // mode
                safe_stof(campos[12]),  // speechiness
                safe_stof(campos[13]),  // acousticness
                safe_stof(campos[14]),  // instrumentalness
                safe_stof(campos[15]),  // liveness
                safe_stof(campos[16]),  // valence
                safe_stof(campos[17]),  // tempo
                safe_stoi(campos[18]),  // duration_ms
                4                       // default time_signature
            );

            contador++;
        } catch (const exception& e) {
        
        }

        // Cada 100,000 registros, muestra progreso
        if (contador % 100000 == 0) {
            auto actual = chrono::high_resolution_clock::now();
            auto duracion = chrono::duration_cast<chrono::milliseconds>(actual - inicio);
            cout << "Procesados " << contador << " registros. Tiempo: " 
                 << duracion.count() << " ms\n";
        }
    }

    // Reducir capacidad al tamaño exacto
    canciones.shrink_to_fit();

    // Mostrar estadísticas finales
    auto fin = chrono::high_resolution_clock::now();
    auto duracion_total = chrono::duration_cast<chrono::milliseconds>(fin - inicio);
    
    cout << "Carga completa. Total canciones: " << canciones.size() 
         << ". Tiempo total: " << duracion_total.count() << " ms\n";

    return canciones;
}

size_t mostrar_menu_navegacion(size_t pagina, size_t total_paginas, bool& navegando) {
    cout << "\nOpciones:\n";
    cout << "1. Página siguiente\n";
    cout << "2. Página anterior\n";
    cout << "3. Ir a página específica\n";
    cout << "4. Salir\n";

    int opcion;
    cin >> opcion;

    switch (opcion) {
        case 1:
            return min(pagina + 1, total_paginas);
        case 2:
            return max(pagina - 1, static_cast<size_t>(1));
        case 3:
            cout << "Ingrese el número de página (1-" << total_paginas << "): ";
            cin >> pagina;
            return min(max(pagina, static_cast<size_t>(1)), total_paginas);
        case 4:
            navegando = false;
            return pagina;
        default:
            return pagina;
    }
}

int main() {
    try {
        ListaReproduccion playlist;
        bool running = true;
        bool csv_cargado = false;

        while (running) {
            cout << "\n--- Menú Principal ---\n";
            cout << "1. Cargar canciones desde CSV\n";
            cout << "2. Listar todas las canciones\n";
            cout << "3. Listar por...\n";
            cout << "4. Buscar canciones por año\n";
            cout << "5. Agregar una canción desde CSV\n";
            cout << "6. Eliminar una canción\n";
            cout << "7. Mover una canción\n";
            cout << "8. Reproducir canción aleatoria\n";
            cout << "9. Buscar canciones por prefijo\n";
            cout << "10. Salir\n";
            cout << "Seleccione una opción: ";

            int opcion;
            cin >> opcion;

            switch (opcion) {
                case 1: { // Cargar canciones desde CSV
                    string file_path = "spotify_data.csv";
                    
                    try {
                        auto canciones = cargar_csv(file_path);
                        for (auto& cancion : canciones) {
                            playlist.agregar_cancion(cancion);
                        }
                        cout << "Canciones cargadas exitosamente.\n";
                    } catch (const runtime_error& e) {
                        cerr << e.what() << '\n';
                    }
                    csv_cargado = true;
                    break;
                }
                case 2: { // Listar todas las canciones con paginación
                    size_t pagina = 1;
                    bool navegando = true;
                    while (navegando) {
                        auto resultado = playlist.listar_canciones_paginado(pagina);

                        cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                             << " (Total canciones: " << resultado.total_canciones << ")\n";

                        for (auto& cancion : resultado.canciones) {
                            cout << cancion.track_name << " - " << cancion.artist_name << " (" << cancion.anio << ")\n";
                        }

                        cout << "\nOpciones:\n";
                        cout << "1. Página siguiente\n";
                        cout << "2. Página anterior\n";
                        cout << "3. Ir a página específica\n";
                        cout << "4. Salir\n";

                        int opcion;
                        cin >> opcion;

                        switch (opcion) {
                            case 1:
                                pagina = min(pagina + 1, resultado.total_paginas);
                                break;
                            case 2:
                                pagina = max(pagina - 1, static_cast<size_t>(1));
                                break;
                            case 3:
                                cout << "Ingrese el número de página (1-" << resultado.total_paginas << "): ";
                                cin >> pagina;
                                pagina = min(max(pagina, static_cast<size_t>(1)), resultado.total_paginas);
                                break;
                            case 4:
                                navegando = false;
                                break;
                        }
                    }
                    break;
                }           
                case 3: { // Sub-menú de ordenamiento
                    bool ordenando = true;
                    while (ordenando) {
                        cout << "\n--- Menú de Ordenamiento ---\n";
                        cout << "1. Ordenar por Popularidad (Descendente)\n";
                        cout << "2. Ordenar por Popularidad (Ascendente)\n";
                        cout << "3. Ordenar por Duración (Descendente)\n";
                        cout << "4. Ordenar por Duración (Ascendente)\n";
                        cout << "5. Volver al menú principal\n";
                        cout << "Seleccione una opción: ";

                        int opcion_ordenamiento;
                        cin >> opcion_ordenamiento;

                        size_t pagina = 1;
                        bool navegando = true;

                        switch (opcion_ordenamiento) {
                            case 1: { // Popularidad Descendente
                                while (navegando) {
                                    auto resultado = playlist.listar_por_popularidad_paginado(false, pagina);

                                    cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                                         << " (Total canciones: " << resultado.total_canciones << ")\n";

                                    for (auto& cancion : resultado.canciones) {
                                        cout << cancion.track_name << " - Popularidad: " << cancion.popularity << "\n";
                                    }

                                    pagina = mostrar_menu_navegacion(pagina, resultado.total_paginas, navegando);
                                }
                                break;
                            }
                            case 2: { // Popularidad Ascendente
                                while (navegando) {
                                    auto resultado = playlist.listar_por_popularidad_paginado(true, pagina);

                                    cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                                         << " (Total canciones: " << resultado.total_canciones << ")\n";

                                    for (auto& cancion : resultado.canciones) {
                                        cout << cancion.track_name << " - Popularidad: " << cancion.popularity << "\n";
                                    }

                                    pagina = mostrar_menu_navegacion(pagina, resultado.total_paginas, navegando);
                                }
                                break;
                            }
                            case 3: { // Duración Descendente
                                while (navegando) {
                                    auto resultado = playlist.listar_por_duracion_paginado(false, pagina);

                                    cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                                         << " (Total canciones: " << resultado.total_canciones << ")\n";

                                    for (auto& cancion : resultado.canciones) {
                                        cout << cancion.track_name << " - Duración: " 
                                             << (cancion.duration_ms / 1000 / 60) << "m " 
                                             << (cancion.duration_ms / 1000 % 60) << "s\n";
                                    }

                                    pagina = mostrar_menu_navegacion(pagina, resultado.total_paginas, navegando);
                                }
                                break;
                            }
                            case 4: { // Duración Ascendente
                                while (navegando) {
                                    auto resultado = playlist.listar_por_duracion_paginado(true, pagina);

                                    cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                                         << " (Total canciones: " << resultado.total_canciones << ")\n";

                                    for (auto& cancion : resultado.canciones) {
                                        cout << cancion.track_name << " - Duración: " 
                                             << (cancion.duration_ms / 1000 / 60) << "m " 
                                             << (cancion.duration_ms / 1000 % 60) << "s\n";
                                    }

                                    pagina = mostrar_menu_navegacion(pagina, resultado.total_paginas, navegando);
                                }
                                break;
                            }
                            case 5:
                                ordenando = false;
                                break;
                            default:
                                cout << "Opción inválida.\n";
                        }
                    }
                    break;
                }           
                case 4: { // Buscar canciones por año con paginación
                    int anio;
                    cout << "Ingrese el año: ";
                    cin >> anio;

                    size_t pagina = 1;
                    bool navegando = true;
                    while (navegando) {
                        auto resultado = playlist.obtener_por_anio_paginado(anio, pagina);

                        cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                             << " (Total canciones del año " << anio << ": " << resultado.total_canciones << ")\n";

                        for (auto& cancion : resultado.canciones) {
                            cout << cancion.track_name << " - " << cancion.artist_name << "\n";
                        }

                        cout << "\nOpciones:\n";
                        cout << "1. Página siguiente\n";
                        cout << "2. Página anterior\n";
                        cout << "3. Ir a página específica\n";
                        cout << "4. Salir\n";

                        int opcion;
                        cin >> opcion;

                        switch (opcion) {
                            case 1:
                                pagina = min(pagina + 1, resultado.total_paginas);
                                break;
                            case 2:
                                pagina = max(pagina - 1, static_cast<size_t>(1));
                                break;
                            case 3:
                                cout << "Ingrese el número de página (1-" << resultado.total_paginas << "): ";
                                cin >> pagina;
                                pagina = min(max(pagina, static_cast<size_t>(1)), resultado.total_paginas);
                                break;
                            case 4:
                                navegando = false;
                                break;
                        }
                    }
                    break;
                }
                case 5: { // Agregar una canción desde CSV
                    string prefijo;
                    int tipo_busqueda;

                    cout << "Buscar canción por:\n1. Nombre de canción\n2. Nombre de artista\nElija una opción: ";
                    cin >> tipo_busqueda;

                    cout << "Ingrese el prefijo de búsqueda: ";
                    cin >> prefijo;

                    vector<Cancion> resultados = tipo_busqueda == 1 ?
                        playlist.buscar_canciones_por_prefijo_en_csv(prefijo, false) :
                        playlist.buscar_canciones_por_prefijo_en_csv(prefijo, true);

                    if (resultados.empty()) {
                        cout << "No se encontraron canciones.\n";
                        break;
                    }

                    // Mostrar resultados
                    for (size_t i = 0; i < resultados.size(); ++i) {
                        cout << i + 1 << ". "
                             << resultados[i].track_name
                             << " - " << resultados[i].artist_name
                             << " (" << resultados[i].anio << ")\n";
                    }

                    size_t seleccion;
                    cout << "Seleccione el número de la canción a agregar: ";
                    cin >> seleccion;

                    if (seleccion < 1 || seleccion > resultados.size()) {
                        cout << "Selección inválida.\n";
                        break;
                    }

                    // Agregar la canción seleccionada a la playlist
                    playlist.agregar_cancion(resultados[seleccion - 1]);
                    cout << "Canción agregada exitosamente.\n";
                    break;
                }               
                case 6: { // Eliminar una canción
                    int tipo_busqueda;
                    string nombre;
                    cout << "Eliminar por:\n1. Nombre de canción\n2. Nombre de artista\nElija una opción: ";
                    cin >> tipo_busqueda;
                    cout << "Ingrese el nombre: ";
                    cin.ignore();
                    getline(cin, nombre);

                    if (tipo_busqueda == 1) {
                        playlist.eliminar_cancion_por_nombre(nombre, false);
                    } else if (tipo_busqueda == 2) {
                        playlist.eliminar_cancion_por_nombre(nombre, true);
                    }
                    break;
                }

                case 7: { // Mover una canción
                    int tipo_busqueda;
                    string nombre;
                    size_t nueva_posicion;

                    cout << "Mover por:\n1. Nombre de canción\n2. Nombre de artista\nElija una opción: ";
                    cin >> tipo_busqueda;

                    cout << "Ingrese el nombre: ";
                    cin.ignore();
                    getline(cin, nombre);

                    cout << "Ingrese la nueva posición (0 para la primera): ";
                    cin >> nueva_posicion;

                    if (tipo_busqueda == 1) {
                        playlist.mover_cancion_por_nombre(nombre, nueva_posicion, false);
                    } else if (tipo_busqueda == 2) {
                        playlist.mover_cancion_por_nombre(nombre, nueva_posicion, true);
                    }
                break;
                }  
                case 8: { // Reproducir canción aleatoria
                    playlist.reproducir_aleatoria();
                    break;
                }
                case 9: { // Buscar canciones por prefijo
                    string prefijo;
                    int tipo_busqueda;
                    cout << "Ingrese el prefijo a buscar: ";
                    cin >> prefijo;
                    cout << "Buscar por:\n1. Artista\n2. Canción\nElija una opción: ";
                    cin >> tipo_busqueda;

                    vector<Cancion> resultados_playlist = playlist.buscar_canciones_por_trie(prefijo, tipo_busqueda == 1);
                    vector<Cancion> resultados_csv = tipo_busqueda == 1 ?
                        playlist.buscar_canciones_por_prefijo_en_csv(prefijo, true) :
                        playlist.buscar_canciones_por_prefijo_en_csv(prefijo, false);

                    cout << "Resultados en la playlist:\n";
                    for (const auto& cancion : resultados_playlist) {
                        cout << cancion.track_name << " - " << cancion.artist_name << " (En playlist)\n";
                    }

                    cout << "\nResultados en CSV:\n";
                    for (const auto& cancion : resultados_csv) {
                        cout << cancion.track_name << " - " << cancion.artist_name << " (En CSV)\n";
                    }
                    break;
                }
                case 10: { // Salir
                    running = false;
                    cout << "Saliendo del programa...\n";
                    break;
                }
            }
        }
    } catch (exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

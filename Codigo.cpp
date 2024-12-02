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
    int time_signature; // Nuevo campo del segundo código

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

    explicit ListaReproduccion(int tamano_maximo = 3) 
        : bTree(tamano_maximo), total_canciones(0) {}

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
    
    // Nueva función para buscar canciones por prefijo de artista o canción
    vector<Cancion> buscar_por_prefijo(const string& prefijo, bool buscar_artista = true) {
    vector<Cancion> resultados;
    vector<string> track_ids = buscar_artista ? 
        trie_artistas.buscar_prefijo(prefijo) : 
        trie_canciones.buscar_prefijo(prefijo);
    
    // Add output to help users understand search results
    cout << "Encontrados " << track_ids.size() << " resultados para el prefijo \"" 
         << prefijo << "\"" << endl;
    
    for (const auto& track_id : track_ids) {
        auto cancion = bTree.buscar(track_id);
        if (cancion) {
            resultados.push_back(*cancion);
        }
    }
    return resultados;
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

// Función para cargar CSV con mejoras
vector<Cancion> cargar_csv(const string& file_path) {
    vector<Cancion> canciones;
    ifstream file(file_path);
    
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + file_path);
    }

    // Determine file size for more accurate pre-allocation
    file.seekg(0, ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, ios::beg);

    // Pre-allocate memory based on estimated number of records
    // Estimate: Average record size is about 300 bytes
    canciones.reserve(min(file_size / 300, static_cast<size_t>(100000)));

    string linea;
    string header;
    getline(file, header); // Skip header

    // Use more efficient string parsing
    while (getline(file, linea)) {
        try {
            istringstream ss(linea);
            vector<string> campos(20);
            string campo;
            int indice = 0;

            while (getline(ss, campo, ',')) {
                // Simple trim function to remove quotes
                if (!campo.empty() && campo.front() == '"' && campo.back() == '"') {
                    campo = campo.substr(1, campo.length() - 2);
                }
                campos[indice++] = move(campo);
                
                // Break if we've reached expected number of fields
                if (indice >= 19) break;
            }

            // Robust parsing with default values
            campos.resize(19); // Ensure full size
            canciones.emplace_back(
                campos[1], campos[2], campos[3], 
                campos[4].empty() ? 0 : stoi(campos[4]),
                campos[5].empty() ? 0 : stoi(campos[5]),
                campos[6], 
                campos[7].empty() ? 0.0f : stof(campos[7]),
                campos[8].empty() ? 0.0f : stof(campos[8]),
                campos[9].empty() ? 0 : stoi(campos[9]),
                campos[10].empty() ? 0.0f : stof(campos[10]),
                campos[11].empty() ? 0 : stoi(campos[11]),
                campos[12].empty() ? 0.0f : stof(campos[12]),
                campos[13].empty() ? 0.0f : stof(campos[13]),
                campos[14].empty() ? 0.0f : stof(campos[14]),
                campos[15].empty() ? 0.0f : stof(campos[15]),
                campos[16].empty() ? 0.0f : stof(campos[16]),
                campos[17].empty() ? 0.0f : stof(campos[17]),
                campos[18].empty() ? 0 : stoi(campos[18]),
                4 // default time_signature
            );
        } catch (const exception& e) {
            
        }
    }

    // Optional: shrink to fit to reduce memory overhead
    canciones.shrink_to_fit();

    cout << "Loaded " << canciones.size() << " songs successfully.\n";
    return canciones;
}

int main() {
    try {
        ListaReproduccion playlist;
        bool running = true;

        while (running) {
            cout << "\n--- Menú Principal ---\n";
            cout << "1. Cargar canciones desde CSV\n";
            cout << "2. Listar todas las canciones\n";
            cout << "3. Listar canciones por popularidad\n";
            cout << "4. Buscar canciones por año\n";
            cout << "5. Agregar una canción manualmente\n";
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
                case 3: { // Listar canciones por popularidad con paginación
        size_t pagina = 1;
        bool navegando = true;
        while (navegando) {
            auto resultado = playlist.listar_por_popularidad_paginado(false, pagina);
            
            cout << "\nPágina " << resultado.pagina_actual << " de " << resultado.total_paginas 
                 << " (Total canciones: " << resultado.total_canciones << ")\n";
            
            for (auto& cancion : resultado.canciones) {
                cout << cancion.track_name << " - Popularidad: " << cancion.popularity << "\n";
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
                case 5: { // Agregar una canción manualmente
                    string artist_name, track_name, track_id, genre;
                    int popularity, anio, key, mode, duration_ms, time_signature;
                    float danceability, energy, loudness, speechiness, acousticness, 
                          instrumentalness, liveness, valence, tempo;

                    cout << "Ingrese los detalles de la canción:\n";
                    cin.ignore();
                    
                    cout << "Artista: ";
                    getline(cin, artist_name);
                    cout << "Nombre de la canción: ";
                    getline(cin, track_name);
                    cout << "ID de la canción: ";
                    getline(cin, track_id);
                    cout << "Popularidad (0-100): ";
                    cin >> popularity;
                    cout << "Año: ";
                    cin >> anio;
                    cin.ignore();
                    cout << "Género: ";
                    getline(cin, genre);
                    cout << "Danceability (0-1): ";
                    cin >> danceability;
                    cout << "Energy (0-1): ";
                    cin >> energy;
                    cout << "Key (0-11): ";
                    cin >> key;
                    cout << "Loudness (-60 to 0): ";
                    cin >> loudness;
                    cout << "Mode (0-1): ";
                    cin >> mode;
                    cout << "Speechiness (0-1): ";
                    cin >> speechiness;
                    cout << "Acousticness (0-1): ";
                    cin >> acousticness;
                    cout << "Instrumentalness (0-1): ";
                    cin >> instrumentalness;
                    cout << "Liveness (0-1): ";
                    cin >> liveness;
                    cout << "Valence (0-1): ";
                    cin >> valence;
                    cout << "Tempo (BPM): ";
                    cin >> tempo;
                    cout << "Duración (ms): ";
                    cin >> duration_ms;
                    cout << "Time Signature (default 4): ";
                    cin >> time_signature;

                    Cancion nueva_cancion(artist_name, track_name, track_id, popularity, 
                                          anio, genre, danceability, energy, key, 
                                          loudness, mode, speechiness, acousticness, 
                                          instrumentalness, liveness, valence, 
                                          tempo, duration_ms, time_signature);
                    playlist.agregar_cancion(nueva_cancion);
                    cout << "Canción agregada exitosamente.\n";
                    break;
                }
                case 6: { // Eliminar una canción
                    string track_id;
                    cout << "Ingrese el ID de la canción a eliminar: ";
                    cin >> track_id;
                    if (playlist.eliminar_cancion(track_id)) {
                        cout << "Canción eliminada.\n";
                    } else {
                        cout << "Canción no encontrada.\n";
                    }
                    break;
                }
                case 7: { // Mover una canción
                    string track_id;
                    int nueva_posicion;
                    cout << "Ingrese el ID de la canción a mover: ";
                    cin >> track_id;
                    cout << "Ingrese la nueva posición (0 para la primera): ";
                    cin >> nueva_posicion;
                    try {
                        playlist.bTree.mover_cancion(track_id, nueva_posicion);
                        cout << "Canción movida.\n";
                    } catch (const exception& e) {
                        cout << "Error: " << e.what() << "\n";
                    }
                    break;
                }
                case 8: { // Reproducir canción aleatoria
                    playlist.reproducir_aleatoria();
                    break;
                }
                case 9: { // Buscar canciones por prefijo de artista o canción
                    string prefijo;
                    int tipo_busqueda;
                    cout << "Ingrese el prefijo a buscar: ";
                    cin >> prefijo;
                    cout << "Buscar por:\n1. Artista\n2. Canción\nElija una opción: ";
                    cin >> tipo_busqueda;
    
                    vector<Cancion> resultados = tipo_busqueda == 1 ?
                    playlist.buscar_por_prefijo(prefijo, true) :
                    playlist.buscar_por_prefijo(prefijo, false);
    
                    cout << "Resultados:\n";
                    for (const auto& cancion : resultados) {
                        cout << cancion.track_name << " - " << cancion.artist_name << "\n";
                    }
                    break;
                }
                case 10: { // Salir
                    running = false;
                    cout << "Saliendo del programa...\n";
                    break;

                }
                default: {
                    cout << "Opción no válida. Intente de nuevo.\n";
                    break;
                }
            }
        }
    } catch (exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
Pagina listar_canciones_paginado(size_t pagina = 1, size_t canciones_por_pagina = 200) const {
   auto todas_canciones = bTree.listar();
  return paginar(todas_canciones, pagina, canciones_por_pagina);
}

    return 0;
}

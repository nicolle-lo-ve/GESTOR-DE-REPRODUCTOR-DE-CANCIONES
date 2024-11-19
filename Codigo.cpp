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

using namespace std;

// Clase Cancion optimizada
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

    // Constructor predeterminado necesario para algunos contenedores
    Cancion() = default;

    // Constructor con parámetros usando inicialización de miembros
    Cancion(string artist_name, string track_name, string track_id, int popularity, 
            int anio, string genre, float danceability, float energy, int key, 
            float loudness, int mode, float speechiness, float acousticness, 
            float instrumentalness, float liveness, float valence, 
            float tempo, int duration_ms) 
        : artist_name(move(artist_name)), track_name(move(track_name)), 
          track_id(move(track_id)), popularity(popularity), anio(anio), 
          genre(move(genre)), danceability(danceability), energy(energy), 
          key(key), loudness(loudness), mode(mode), speechiness(speechiness), 
          acousticness(acousticness), instrumentalness(instrumentalness), 
          liveness(liveness), valence(valence), tempo(tempo), 
          duration_ms(duration_ms) {}

    // Operadores de comparación optimizados
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

// Clase ListaReproduccion optimizada
class ListaReproduccion {
public:
    BTree bTree;
    size_t total_canciones;

    explicit ListaReproduccion(int tamano_maximo = 3) 
        : bTree(tamano_maximo), total_canciones(0) {}

    void agregar_cancion(const Cancion& cancion) {
        bTree.insertar(cancion);
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
};

// Función optimizada para cargar CSV
vector<Cancion> cargar_csv(const string& file_path) {
    vector<Cancion> canciones;
    ifstream file(file_path);
    
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + file_path);
    }

    string linea;
    getline(file, linea); // Leer la cabecera
    canciones.reserve(1000); // Reservar espacio aproximado

    while (getline(file, linea)) {
        istringstream ss(linea);
        vector<string> datos;
        datos.reserve(19);
        string campo_actual;
        bool dentro_comillas = false;

        for (char c : linea) {
            if (c == '"') {
                dentro_comillas = !dentro_comillas;
            } else if (c == ',' && !dentro_comillas) {
                datos.push_back(move(campo_actual));
                campo_actual.clear();
            } else {
                campo_actual += c;
            }
        }
        datos.push_back(move(campo_actual));

        try {
            canciones.emplace_back(
                move(datos[1]), move(datos[2]), move(datos[3]), stoi(datos[4]), 
                stoi(datos[5]), move(datos[6]), stof(datos[7]), stof(datos[8]), 
                stoi(datos[9]), stof(datos[10]), stoi(datos[11]), stof(datos[12]), 
                stof(datos[13]), stof(datos[14]), stof(datos[15]), stof(datos[16]), 
                stof(datos[17]), stoi(datos[18])
            );
        } catch (const exception& e) {
            cerr << "Error al procesar línea: " << linea << "\n";
            cerr << "Error: " << e.what() << "\n";
            continue;
        }
    }

    return canciones;
}

int main() {
    try {
        ListaReproduccion playlist;
        bool running = true;

        while (running) {
            cout << "\nMenu:\n";
            cout << "1. Cargar canciones desde CSV\n";
            cout << "2. Listar todas las canciones\n";
            cout << "3. Listar canciones por popularidad\n";
            cout << "4. Buscar canciones por año\n";
            cout << "5. Agregar una canción manualmente\n";
            cout << "6. Eliminar una canción\n"; // Opción para eliminar
            cout << "7. Mover una canción\n"; // Opción para mover
            cout << "8. Salir\n";
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
                case 2: { // Listar todas las canciones
                    cout << "\nCanciones:\n";
                    for (auto& cancion : playlist.listar_canciones()) {
                        cout << cancion.track_name << " - " << cancion.artist_name << " (" << cancion.anio << ")\n";
                    }
                    break;
                }
                case 3: { // Listar canciones por popularidad
                    cout << "\nCanciones ordenadas por popularidad (descendente):\n";
                    for (auto& cancion : playlist.listar_por_popularidad(false)) {
                        cout << cancion.track_name << " - Popularidad: " << cancion.popularity << "\n";
                    }
                    break;
                }
                case 4: { // Buscar canciones por año
                    int anio;
                    cout << "Ingrese el año: ";
                    cin >> anio;
                    cout << "\nCanciones del año " << anio << ":\n";
                    for (auto& cancion : playlist.obtener_por_anio(anio)) {
                        cout << cancion.track_name << " - " << cancion.artist_name << "\n";
                    }
                    break;
                }
                case 5: { // Agregar una canción manualmente
                    string artist_name, track_name, track_id, genre;
                    int popularity, anio, key, mode, duration_ms;
                    float danceability, energy, loudness, speechiness, acousticness, instrumentalness, liveness, valence, tempo;

                    cout << "Ingrese los detalles de la canción:\n";
                    cout << "Artista: ";
                    cin.ignore(); // Limpiar el buffer
                    getline(cin, artist_name);
                    cout << "Nombre de la canción: ";
                    getline(cin, track_name);
                    cout << "ID de la canción: ";
                    getline(cin, track_id);
                    cout << "Popularidad: ";
                    cin >> popularity;
                    cout << "Año: ";
                    cin >> anio;
                    cout << "Género: ";
                    cin.ignore();
                    getline(cin, genre);
                    cout << "Danceability: ";
                    cin >> danceability;
                    cout << "Energy: ";
                    cin >> energy;
                    cout << "Key: ";
                    cin >> key;
                    cout << "Loudness: ";
                    cin >> loudness;
                    cout << "Mode: ";
                    cin >> mode;
                    cout << "Speechiness: ";
                    cin >> speechiness;
                    cout << "Acousticness: ";
                    cin >> acousticness;
                    cout << "Instrumentalness: ";
                    cin >> instrumentalness;
                    cout << "Liveness: ";
                    cin >> liveness;
                    cout << "Valence: ";
                    cin >> valence;
                    cout << "Tempo: ";
                    cin >> tempo;
                    cout << "Duración (ms): ";
                    cin >> duration_ms;

                    Cancion nueva_cancion(artist_name, track_name, track_id, popularity, anio, genre,
                                           danceability, energy, key, loudness, mode, speechiness,
                                           acousticness, instrumentalness, liveness, valence, tempo, duration_ms);
                    playlist.agregar_cancion(nueva_cancion);
                    cout << "Canción agregada exitosamente.\n";
                    break;
                }
                case 6: { // Eliminar una canción
                    string track_id;
                    cout << "Ingrese el ID de la canción a eliminar: ";
                    cin >> track_id;
                    playlist.bTree.eliminar(track_id); // Llama al método de eliminación
                    cout << "Canción eliminada.\n";
                    break;
                }
                case 7: { // Mover una canción
                    string track_id;
                    int nueva_posicion;
                    cout << "Ingrese el ID de la canción a mover: ";
                    cin >> track_id;
                    cout << "Ingrese la nueva posición (0 para la primera): ";
                    cin >> nueva_posicion;
                    playlist.bTree.mover_cancion(track_id, nueva_posicion); // Llama al método de mover
                    cout << "Canción movida.\n";
                    break;
                }
                case 8: { // Salir
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

    return 0;
}

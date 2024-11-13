#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include <stdexcept>

using namespace std;

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
    // Constructor por defecto
        Cancion() : artist_name(""), track_name(""), track_id(""), popularity(0), 
                     anio(0), genre(""), danceability(0.0), energy(0.0), key(0), 
                     loudness(0.0), mode(0), speechiness(0.0), acousticness(0.0), 
                     instrumentalness(0.0), liveness(0.0), valence(0.0), 
                     tempo(0.0), duration_ms(0) {}

    // Constructor con parámetros
    Cancion(string artist_name, string track_name, string track_id, int popularity, 
            int anio, string genre, float danceability, float energy, int key, 
            float loudness, int mode, float speechiness, float acousticness, 
            float instrumentalness, float liveness, float valence, 
            float tempo, int duration_ms) 
        : artist_name(artist_name), track_name(track_name), track_id(track_id), 
          popularity(popularity), anio(anio), genre(genre), 
          danceability(danceability), energy(energy), key(key), 
          loudness(loudness), mode(mode), speechiness(speechiness), 
          acousticness(acousticness), instrumentalness(instrumentalness), 
          liveness(liveness), valence(valence), tempo(tempo), duration_ms(duration_ms) {}

    bool operator > (Cancion otra) {
        return track_name > otra.track_name;
    }

    bool operator==(Cancion otra) {
        return track_id == otra.track_id;
    }
};

class Nodo {
public:
    vector<Cancion> canciones;
    vector<Nodo*> hijos;
    int tamano_maximo;
    bool es_hoja;

    explicit Nodo(int tamano_maximo) : tamano_maximo(tamano_maximo), es_hoja(true) {}
    
    ~Nodo() {
        for (auto hijo : hijos) {
            delete hijo;
        }
    }

    void insertar_no_lleno(Cancion cancion);
    void dividir_hijo(int indice);
};

class BTree {
public:
    Nodo* raiz;
    int tamano_maximo;

    explicit BTree(int tamano_maximo) : tamano_maximo(tamano_maximo) {
        raiz = new Nodo(tamano_maximo);
    }

    ~BTree() {
        delete raiz;
    }

    void insertar(Cancion cancion);
    void eliminar(string track_id);
    void mover_cancion(string track_id, int nueva_posicion);
    vector<Cancion> listar();
    vector<Cancion> listar_por_popularidad(bool ascendente = true);
    vector<Cancion> obtener_por_anio(int anio);

private:
    void _listar(Nodo* nodo, vector<Cancion>& resultado);
};

class ListaReproduccion {
public:
    BTree bTree;

    explicit ListaReproduccion(int tamano_maximo = 3) : bTree(tamano_maximo) {}

    void agregar_cancion(Cancion cancion) {
        bTree.insertar(cancion);
    }

    vector<Cancion> listar_canciones() {
        return bTree.listar();
    }

    vector<Cancion> listar_por_popularidad(bool ascendente = true) {
        return bTree.listar_por_popularidad(ascendente);
    }

    vector<Cancion> obtener_por_anio(int anio) {
        return bTree.obtener_por_anio(anio);
    }

private:
};

vector<Cancion> cargar_csv(string file_path) {
    ListaReproduccion lista_reproduccion;
    
    ifstream file(file_path);
    
    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + file_path);
    }

    string linea;
    
    getline(file, linea); // Leer la cabecera

    while (getline(file, linea)) {
        istringstream ss(linea);
        vector<string> datos;
        string campo;

        bool dentro_comillas = false;
        string campo_actual;

        for (char c : linea) {
            if (c == '"') {
                dentro_comillas = !dentro_comillas;
            } else if (c == ',' && !dentro_comillas) {
                datos.push_back(campo_actual);
                campo_actual.clear();
            } else {
                campo_actual += c;
            }
        }
        
        datos.push_back(campo_actual);

        try {
            Cancion cancion(
                datos[1], datos[2], datos[3], stoi(datos[4]), 
                stoi(datos[5]), datos[6], stof(datos[7]), stof(datos[8]), stoi(datos[9]), 
                stof(datos[10]), stoi(datos[11]), stof(datos[12]), stof(datos[13]), 
                stof(datos[14]), stof(datos[15]), stof(datos[16]), stof(datos[17]), stoi(datos[18])
            );
            lista_reproduccion.agregar_cancion(cancion);
        } catch (exception& e) {
            cerr << "Error al procesar línea: " << linea << "\n";
            cerr << "Error: " << e.what() << "\n";
            continue;
        }
    }

   return lista_reproduccion.listar_canciones();
}

// Implementación de BTree::insertar
void BTree::insertar(Cancion cancion) {
   if (raiz->canciones.size() == tamano_maximo) {
       Nodo* nuevo_nodo = new Nodo(tamano_maximo);
       nuevo_nodo->es_hoja = false;
       nuevo_nodo->hijos.push_back(raiz);
       raiz = nuevo_nodo;
       raiz->dividir_hijo(0);
   }
   raiz->insertar_no_lleno(cancion);
}

// Implementación de BTree::eliminar
void BTree::eliminar(string track_id) {
    // Este es un esqueleto básico. Necesitarás implementar la lógica para buscar y eliminar.
    // Para simplificar, aquí solo se muestra cómo podrías empezar.
    for (auto& cancion : raiz->canciones) {
        if (cancion.track_id == track_id) {
            // Elimina la canción del nodo
            raiz->canciones.erase(remove(raiz->canciones.begin(), raiz->canciones.end(), cancion), raiz->canciones.end());
            return;
        }
    }
    // Si no se encuentra en el nodo raíz, deberías buscar en los hijos.
}

// Implementación de BTree::mover_cancion
void BTree::mover_cancion(string track_id, int nueva_posicion) {
    Cancion cancion_a_mover;
    bool encontrada = false;

    // Busca la canción
    for (const auto& cancion : listar()) {
        if (cancion.track_id == track_id) {
            cancion_a_mover = cancion;
            encontrada = true;
            break;
        }
    }

    if (!encontrada) {
        cerr << "Canción no encontrada.\n";
        return;
    }

    // Elimina la canción
    eliminar(track_id);

    // Inserta la canción en la nueva posición
    // Aquí necesitarás una lógica para insertar en la posición correcta.
    insertar(cancion_a_mover);
}

// Implementación de Nodo::insertar_no_lleno
void Nodo::insertar_no_lleno(Cancion cancion) {
   int i = canciones.size() - 1;

   if (es_hoja) {
       while (i >= 0 && canciones[i] > cancion) {
           i--;
       }
       canciones.insert(canciones.begin() + i + 1, cancion);
   } else {
       while (i >= 0 && canciones[i] > cancion) {
           i--;
       }
       hijos[i + 1]->insertar_no_lleno(cancion);
   }
}

// Implementación de Nodo::dividir_hijo
void Nodo::dividir_hijo(int indice) {
   Nodo* hijo = hijos[indice];
   Nodo* nuevo_hijo = new Nodo(hijo->tamano_maximo);
   nuevo_hijo->es_hoja = hijo->es_hoja;

   for (int j = 0; j < tamano_maximo / 2; j++) {
       nuevo_hijo->canciones.push_back(hijo->canciones[j + tamano_maximo / 2]);
   }

   if (!hijo->es_hoja) {
       for (int j = 0; j <= tamano_maximo / 2; j++) {
           nuevo_hijo->hijos.push_back(hijo->hijos[j + tamano_maximo / 2]);
       }
   }

   canciones.insert(canciones.begin() + indice, hijo->canciones[tamano_maximo / 2 - 1]);
   hijos.insert(hijos.begin() + indice + 1, nuevo_hijo);

   hijo->canciones.resize(tamano_maximo / 2 - 1);
}

// Implementación de BTree::_listar
void BTree::_listar(Nodo* nodo, vector<Cancion>& resultado) {
   for (size_t i = 0; i < nodo->canciones.size(); i++) {
       if (!nodo->es_hoja) {
           _listar(nodo->hijos[i], resultado);
       }
       resultado.push_back(nodo->canciones[i]);
   }

   if (!nodo->es_hoja) {
       _listar(nodo->hijos[nodo->canciones.size()], resultado);
   }
}

// Implementación de BTree::listar
vector<Cancion> BTree::listar() {
   vector<Cancion> resultado;
   _listar(raiz, resultado);
   return resultado;
}

// Implementación de BTree::listar_por_popularidad
vector<Cancion> BTree::listar_por_popularidad(bool ascendente) {
   vector<Cancion> canciones = listar();

   sort(canciones.begin(), canciones.end(), [ascendente](const Cancion& a, const Cancion& b) {
       return ascendente ? a.popularity < b.popularity : a.popularity > b.popularity;
   });

   return canciones;
}

// Implementación de BTree::obtener_por_anio
vector<Cancion> BTree::obtener_por_anio(int anio) {
   vector<Cancion> todas_canciones = listar();
   vector<Cancion> filtradas;

   for (const auto& cancion : todas_canciones) {
       if (cancion.anio == anio) {
           filtradas.push_back(cancion);
       }
   }

   return filtradas;
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
                    string file_path;
                    cout << "Ingrese la ruta del archivo CSV: ";
                    cin >> file_path;
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

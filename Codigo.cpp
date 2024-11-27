#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// Estructura que representa una canción
struct Cancion {
    string artist_name;
    string track_name;
    string track_id;
    int popularity;
    int year;
    string genre;
    float danceability;
    float energy;
    int key;
    float loudness;
    bool mode;
    float speechiness;
    float acousticness;
    float instrumentalness;
    float liveness;
    float valence;
    float tempo;
    int duration_ms;
    int time_signature;

    Cancion(const string& artist, const string& track, const string& id, int pop, int yr,
            const string& gen, float dance, float en, int k, float loud,
            bool md, float speech, float acoust, float instrum,
            float live, float val, float tmp, int dur, int time_sig)
        : artist_name(artist), track_name(track), track_id(id), popularity(pop),
          year(yr), genre(gen), danceability(dance), energy(en), key(k),
          loudness(loud), mode(md), speechiness(speech), acousticness(acoust),
          instrumentalness(instrum), liveness(live), valence(val),
          tempo(tmp), duration_ms(dur), time_signature(time_sig) {}
};

// Clase para gestionar la lista de reproducción
class ListaReproduccion {
private:
    vector<Cancion> canciones; // Usar vector para acceso rápido
    unordered_map<string, Cancion*> cancionMap; // Mapa para acceso rápido por nombre

public:
    void insertarCancion(const Cancion& cancion) {
        canciones.push_back(cancion);
        cancionMap[cancion.track_name] = &canciones.back(); // Mapa para acceso rápido
    }

    void mostrarCanciones() const {
        cout << "--- Canciones ---\n";
        for (const auto& cancion : canciones) {
            cout << "Artista: " << cancion.artist_name << ", Canción: " << cancion.track_name << endl;
        }
    }

    bool eliminarCancionPorNombre(const string& nombre) {
        auto it = cancionMap.find(nombre);
        if (it != cancionMap.end()) {
            // Eliminar canción del vector
            auto index = distance(canciones.begin(), find_if(canciones.begin(), canciones.end(),
                [&](const Cancion& c) { return c.track_name == nombre; }));
            canciones.erase(canciones.begin() + index);
            cancionMap.erase(it);
            cout << "La canción \"" << nombre << "\" ha sido eliminada." << endl;
            return true;
        }
        cout << "La canción \"" << nombre << "\" no fue encontrada." << endl;
        return false;
    }

    void reproducirAleatoria() const {
        if (canciones.empty()) {
            cout << "La lista de reproducción está vacía." << endl;
            return;
        }
        srand(static_cast<unsigned>(time(nullptr)));
        int indiceAleatorio = rand() % canciones.size();
        const Cancion& cancion = canciones[indiceAleatorio];
        cout << "Reproduciendo: " << cancion.track_name << " - " << cancion.artist_name << endl;
    }
};

// Función para cargar el CSV
void cargarCSV(const string& nombreArchivo, ListaReproduccion& lista) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo " << nombreArchivo << endl;
        return;
    }

    string linea;
    getline(archivo, linea); // Leer encabezados
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string artist, track, id, genre;
        int popularity, year, key, duration_ms, time_signature,n;
        float danceability, energy, loudness, speechiness,
              acousticness, instrumentalness, liveness,
              valence, tempo;
        bool mode;

        ss >> n; ss.ignore();
        getline(ss, artist, ',');
        getline(ss, track, ',');
        getline(ss, id, ',');
        ss >> popularity; ss.ignore();
        ss >> year; ss.ignore();
        getline(ss, genre, ',');
        ss >> danceability; ss.ignore();
        ss >> energy; ss.ignore();
        ss >> key; ss.ignore();
        ss >> loudness; ss.ignore();
        ss >> mode; ss.ignore();
        ss >> speechiness; ss.ignore();
        ss >> acousticness; ss.ignore();
        ss >> instrumentalness; ss.ignore();
        ss >> liveness; ss.ignore();
        ss >> valence; ss.ignore();
        ss >> tempo; ss.ignore();
        ss >> duration_ms; ss.ignore();
        ss >> time_signature;

        Cancion cancion(artist, track, id, popularity, year,
                         genre, danceability, energy,
                         key, loudness,
                         mode,
                         speechiness,
                         acousticness,
                         instrumentalness,
                         liveness,
                         valence,
                         tempo,
                         duration_ms,
                         time_signature);

        lista.insertarCancion(cancion);
    }

    archivo.close();
    cout << "Archivo CSV cargado correctamente." << endl;
}

int main() {
    ListaReproduccion lista;

    string archivoCSV = "spotify_data.csv";
    cargarCSV(archivoCSV, lista);

    int opcion;
    do {
        cout << "\n--- Menú ---\n";
        cout << "1. Mostrar todas las canciones\n";
        cout << "2. Eliminar canción por nombre\n";
        cout << "3. Reproducir aleatoria\n";
        cout << "4. Salir\n";

        cin >> opcion;

        switch (opcion) {
            case 1:
                lista.mostrarCanciones();
                break;

            case 2: {
                cout << "Ingrese el nombre de la canción a eliminar: ";
                string nombre;
                cin.ignore(); // Limpiar buffer
                getline(cin, nombre);
                lista.eliminarCancionPorNombre(nombre);
                break;
            }

            case 3:
                lista.reproducirAleatoria();
                break;

            case 4:
                cout << "Saliendo del programa..." << endl;
                break;

            default:
                cout << "Opción no válida. Intente de nuevo." << endl;
                break;
        }

    } while (opcion != 4);

    return 0;
}

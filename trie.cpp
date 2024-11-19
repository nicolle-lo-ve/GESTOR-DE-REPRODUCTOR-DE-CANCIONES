#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <limits>
#include <fstream>
#include <set>

using namespace std;

const int M = 52; // 26 letras del alfabeto

// Function to clean a word by removing non-alphabetic characters
string cleanWord(const string& word) {
    string cleaned;
    for (char c : word) {
        if (isalpha(c)) {
            cleaned += tolower(c);
        }
    }
    return cleaned;
}

// TrieNode class implementation
class TrieNode {
public:
    char data;
    bool is_leaf;
    TrieNode* children[M];

    TrieNode(char c) : data(c), is_leaf(false) {
        for (int i = 0; i < M; i++) {
            children[i] = nullptr;
        }
    }

    ~TrieNode() {
        for (int i = 0; i < M; i++) {
            delete children[i];
        }
    }
};

// Trie class implementation
class Trie {
private:
    TrieNode* root;

    TrieNode* make_trienode(char data) {
        return new TrieNode(data);
    }

    TrieNode* insert_trie(TrieNode* node, const string& word, int index) {
        if (index == word.length()) {
            node->is_leaf = true;
            return node;
        }

        int idx = word[index] - 'a';
        if (!node->children[idx]) {
            node->children[idx] = make_trienode(word[index]);
        }
        return insert_trie(node->children[idx], word, index + 1);
    }

    void print_trie(TrieNode* node, string prefix) const {
        if (node->is_leaf) {
            cout << prefix << endl;
        }

        for (int i = 0; i < M; i++) {
            if (node->children[i]) {
                print_trie(node->children[i], prefix + node->children[i]->data);
            }
        }
    }

    bool delete_trie(TrieNode* node, const string& word, int index) {
        if (!node) return false;

        if (index == word.length()) {
            if (!node->is_leaf) return false;
            node->is_leaf = false;

            for (int i = 0; i < M; i++) {
                if (node->children[i]) return false;
            }
            return true;
        }

        int idx = word[index] - 'a';
        if (delete_trie(node->children[idx], word, index + 1)) {
            delete node->children[idx];
            node->children[idx] = nullptr;
            return !node->is_leaf;
        }
        return false;
    }

    void search_words(TrieNode* node, const string& prefix, vector<string>& results) const {
        if (node->is_leaf) {
            results.push_back(prefix);
        }

        for (int i = 0; i < M; i++) {
            if (node->children[i]) {
                search_words(node->children[i], prefix + node->children[i]->data, results);
            }
        }
    }

public:
    Trie() {
        root = new TrieNode('\0');
    }

    ~Trie() {
        delete root;
    }

    void insert(const string& word) {
        if (word.empty()) {
            cerr << "Error: palabra vacía no se puede insertar." << endl;
            return;
        }

        string cleanedWord = cleanWord(word);
        if (cleanedWord.empty()) return;
        
        insert_trie(root, cleanedWord, 0);
    }

    void print() const {
        print_trie(root, "");
    }

    bool remove(const string& word) {
        return delete_trie(root, cleanWord(word), 0);
    }

    vector<string> search_words(const string& prefix) const {
        vector<string> results;
        string cleanedPrefix = cleanWord(prefix);
        TrieNode* node = root;

        for (char c : cleanedPrefix) {
            int index = c - 'a';
            if (!node->children[index]) {
                return results;
            }
            node = node->children[index];
        }

        search_words(node, cleanedPrefix, results);
        return results;
    }
};

// Utility function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    ifstream inputFile("words.txt");
    ofstream outputFile("salida.txt");
    set<string> uniqueWords;

    if (!inputFile) {
        cerr << "No se pudo abrir el archivo de entrada." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        string word;
        for (char c : line) {
            if (c == ' ' || c == '\n') {
                if (!word.empty()) {
                    string cleanedWord = cleanWord(word);
                    if (!cleanedWord.empty()) {
                        uniqueWords.insert(cleanedWord);
                    }
                    word.clear();
                }
            } else {
                word += c;
            }
        }
        if (!word.empty()) {
            string cleanedWord = cleanWord(word);
            if (!cleanedWord.empty()) {
                uniqueWords.insert(cleanedWord);
            }
        }
    }

    for (const auto& word : uniqueWords) {
        outputFile << word << endl;
    }

    inputFile.close();
    outputFile.close();

    cout << "El archivo ha sido limpiado y guardado como 'salida.txt'." << endl;

    Trie trie;
    for (const auto& word : uniqueWords) {
        trie.insert(word);
    }

    cout << "Trie construido con las palabras del archivo." << endl;

    while (true) {
        cout << "\nMenú:\n";
        cout << "1. Insertar palabra\n";
        cout << "2. Buscar palabras que empiecen con un prefijo\n";
        cout << "3. Eliminar una palabra\n";
        cout << "4. Imprimir Trie\n";
        cout << "5. Salir\n";
        cout << "Elija una opción: ";

        int opcion;
        cin >> opcion;

        if (cin.fail()) {
            clearInputBuffer();
            cout << "Opción no válida. Por favor, intente de nuevo.\n";
            continue;
        }

        if (opcion == 5) break;

        switch (opcion) {
            case 1: {
                string palabra;
                cout << "Ingrese la palabra a insertar: ";
                cin >> palabra;
                trie.insert(palabra);
                cout << "Palabra '" << palabra << "' insertada.\n";
                break;
            }
            case 2: {
                string prefijo;
                cout << "Ingrese el prefijo de búsqueda: ";
                cin >> prefijo;
                auto resultados = trie.search_words(prefijo);
                if (resultados.empty()) {
                    cout << "No se encontraron palabras que empiecen con '" << prefijo << "'\n";
                } else {
                    cout << "Palabras que empiezan con '" << prefijo << "':\n";
                    for (const string& palabra : resultados) {
                        cout << "- " << palabra << "\n";
                    }
                }
                break;
            }
            case 3: {
                string palabra;
                cout << "Ingrese la palabra a eliminar: ";
                cin >> palabra;
                if (trie.remove(palabra)) {
                    cout << "Palabra '" << palabra << "' eliminada.\n";
                } else {
                    cout << "No se encontró la palabra '" << palabra << "'.\n";
                }
                break;
            }
            case 4: {
                cout << "Contenido del Trie:\n";
                trie.print();
                break;
            }
            default:
                cout << "Opción no válida. Intente nuevamente.\n";
        }
    }

    cout << "Programa finalizado.\n";
    return 0;
}

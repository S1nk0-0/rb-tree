// uso: ./rbtree ops.txt salida.json   (lineas: i x / d x / s x)
#include <iostream>
#include "rbtree.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "uso: " << argv[0] << " ops.txt salida.json\n";
        return 1;
    }
    ifstream in(argv[1]);
    if (!in) {
        cerr << "no se pudo abrir " << argv[1] << "\n";
        return 1;
    }
    RBTree t;
    char c;
    int x;
    while (in >> c >> x) {
        if (c == 'i') t.insertar(x);
        else if (c == 'd') t.eliminar(x);
        else if (c == 's') t.buscar(x);
    }
    t.exportar(argv[2]);
    cout << argv[2] << " | altura " << t.altura() << " | valido: " << (t.esValido() ? "si" : "no") << "\n";
}

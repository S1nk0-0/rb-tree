#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include "rbtree.hpp"

int main() {
    // vacio
    {
        RBTree t;
        t.setLog(false);
        assert(!t.buscar(5));
        assert(!t.eliminar(5));
        assert(t.vacio() && t.esValido());
        cout << "ok vacio\n";
    }
    // un elemento y duplicado
    {
        RBTree t;
        t.setLog(false);
        assert(t.insertar(7));
        assert(t.esValido() && t.buscar(7) && t.altura() == 1);
        assert(!t.insertar(7));
        assert(t.eliminar(7));
        assert(t.vacio() && t.esValido());
        cout << "ok un elemento\n";
    }
    // en orden 1..n (peor caso de un BST normal)
    {
        int n = 1000;
        RBTree t;
        t.setLog(false);
        for (int i = 1; i <= n; i++) {
            t.insertar(i);
            assert(t.esValido());
        }
        double lim = 2 * log2(n + 1);
        cout << "ok en orden: altura " << t.altura() << " (limite " << lim << ", BST normal " << n << ")\n";
        assert(t.altura() <= lim);
        for (int i = 1; i <= n; i++) {
            t.eliminar(i);
            assert(t.esValido());
        }
        assert(t.vacio());
        cout << "ok eliminar en orden\n";
    }
    // aleatorio, comparando contra un vector ordenado
    {
        RBTree t;
        t.setLog(false);
        vector<int> ref;
        mt19937 rng(42);
        for (int it = 0; it < 20000; it++) {
            int op = rng() % 3, v = rng() % 500;
            auto pos = lower_bound(ref.begin(), ref.end(), v);
            bool esta = pos != ref.end() && *pos == v;
            if (op == 0) {
                assert(t.insertar(v) == !esta);
                if (!esta) ref.insert(pos, v);
            } else if (op == 1) {
                assert(t.eliminar(v) == esta);
                if (esta) ref.erase(pos);
            } else {
                assert(t.buscar(v) == esta);
            }
            assert(t.esValido());
        }
        vector<int> res;
        t.inorden(res);
        assert(res == ref);
        cout << "ok 20000 ops aleatorias\n";
    }
    cout << "todo ok\n";
}

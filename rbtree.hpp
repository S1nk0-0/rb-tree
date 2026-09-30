#pragma once
#include <algorithm>
#include <climits>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

enum Color { ROJO, NEGRO };

struct Nodo {
    int val;
    Color color;
    Nodo *izq, *der, *padre;
};

class RBTree {
    Nodo* nil;  // hoja centinela
    Nodo* raiz;
    bool log = true;
    vector<string> pasos, ops;
    string opActual;
    int valActual = 0;

public:
    RBTree() {
        nil = new Nodo{0, NEGRO, nullptr, nullptr, nullptr};
        nil->izq = nil->der = nil->padre = nil;
        raiz = nil;
    }
    ~RBTree() { borrarTodo(raiz); delete nil; }

    void setLog(bool b) { log = b; }

    bool buscar(int v) {
        inicio("buscar", v);
        Nodo* n = encontrar(v);
        if (n == nil) {
            paso("no_encontrado", "", "El " + to_string(v) + " no está en el árbol");
            fin("no_encontrado");
            return false;
        }
        paso("encontrado", campo("nodo", n), "Encontrado el " + to_string(v));
        fin("encontrado");
        return true;
    }

    bool insertar(int v) {
        inicio("insertar", v);
        Nodo* p = nil;
        Nodo* act = raiz;
        while (act != nil) {
            p = act;
            if (v == act->val) {
                paso("duplicado", campo("nodo", act), "El " + to_string(v) + " ya existe, no se inserta");
                fin("duplicado");
                return false;
            }
            comparar(v, act);
            act = v < act->val ? act->izq : act->der;
        }
        Nodo* nuevo = new Nodo{v, ROJO, nil, nil, p};
        if (p == nil) raiz = nuevo;
        else if (v < p->val) p->izq = nuevo;
        else p->der = nuevo;

        string donde = p == nil ? "como raíz" : "como hijo de " + str(p);
        paso("insertar_nodo", campo("nodo", nuevo) + ",\"padre\":" + json(p),
             "Se inserta el " + to_string(v) + " en rojo " + donde);
        arreglarInsercion(nuevo);
        fin("ok");
        return true;
    }

    bool eliminar(int v) {
        inicio("eliminar", v);
        Nodo* nodo = encontrar(v);
        if (nodo == nil) {
            paso("no_encontrado", "", "El " + to_string(v) + " no está en el árbol, no hay nada que eliminar");
            fin("no_encontrado");
            return false;
        }
        paso("encontrado", campo("nodo", nodo), "Encontrado el " + to_string(v) + ", se va a eliminar");

        Nodo* reemp = nodo;
        Color colorOrig = reemp->color;
        Nodo* hijo;
        if (nodo->izq == nil) {
            hijo = nodo->der;
            cambiar(nodo, nodo->der);
        } else if (nodo->der == nil) {
            hijo = nodo->izq;
            cambiar(nodo, nodo->izq);
        } else {
            reemp = minimo(nodo->der);
            paso("sucesor", campo("nodo", reemp),
                 "Tiene dos hijos: se reemplaza por su sucesor " + str(reemp) + " (el menor del lado derecho)");
            colorOrig = reemp->color;
            hijo = reemp->der;
            if (reemp->padre == nodo) {
                hijo->padre = reemp;
            } else {
                cambiar(reemp, reemp->der);
                reemp->der = nodo->der;
                reemp->der->padre = reemp;
            }
            cambiar(nodo, reemp);
            reemp->izq = nodo->izq;
            reemp->izq->padre = reemp;
            reemp->color = nodo->color;
        }
        paso("eliminar_nodo", "\"nodo\":" + to_string(v), "Se quita el " + to_string(v) + " del árbol");
        delete nodo;

        if (colorOrig == NEGRO) arreglarEliminacion(hijo);
        fin("ok");
        return true;
    }

    bool esValido() const {
        bool ok = raiz->color == NEGRO;
        revisar(raiz, LLONG_MIN, LLONG_MAX, ok);
        return ok;
    }
    int altura() const { return altura(raiz); }
    bool vacio() const { return raiz == nil; }
    void inorden(vector<int>& res) const { inorden(raiz, res); }

    void exportar(const string& ruta) const {
        ofstream f(ruta);
        f << "{\n  \"estructura\": \"Red-Black Tree\",\n  \"operaciones\": [\n";
        for (size_t i = 0; i < ops.size(); i++)
            f << "    " << ops[i] << (i + 1 < ops.size() ? ",\n" : "\n");
        f << "  ]\n}\n";
    }

private:
    void inicio(const string& op, int v) { opActual = op; valActual = v; pasos.clear(); }

    void fin(const string& res) {
        if (!log) return;
        string s = "{\"op\":\"" + opActual + "\",\"valor\":" + to_string(valActual) +
                   ",\"resultado\":\"" + res + "\",\"pasos\":[";
        for (size_t i = 0; i < pasos.size(); i++) s += (i ? "," : "") + pasos[i];
        ops.push_back(s + "]}");
    }

    void paso(const string& accion, const string& extra, const string& nota) {
        if (!log) return;
        pasos.push_back("{\"accion\":\"" + accion + "\"" + (extra.empty() ? "" : "," + extra) +
                        ",\"nota\":\"" + nota + "\",\"arbol\":" + foto(raiz) + "}");
    }

    void comparar(int v, Nodo* n) {
        bool izq = v < n->val;
        paso("comparar", campo("nodo", n) + ",\"direccion\":\"" + (izq ? "izq" : "der") + "\"",
             to_string(v) + (izq ? " < " : " > ") + str(n) + ", baja a la " + (izq ? "izquierda" : "derecha"));
    }

    void recolorear(vector<Nodo*> ns, const string& nota) {
        string lista;
        for (Nodo* n : ns) {
            if (n == nil) continue;
            if (!lista.empty()) lista += ",";
            lista += "{\"clave\":" + to_string(n->val) + ",\"color\":\"" + (n->color == ROJO ? "R" : "N") + "\"}";
        }
        paso("recolorear", "\"nodos\":[" + lista + "]", nota);
    }

    string foto(Nodo* n) const {
        if (n == nil) return "null";
        return "{\"clave\":" + to_string(n->val) + ",\"color\":\"" + (n->color == ROJO ? "R" : "N") +
               "\",\"izq\":" + foto(n->izq) + ",\"der\":" + foto(n->der) + "}";
    }

    string str(Nodo* n) const { return n == nil ? "NIL" : to_string(n->val); }
    string json(Nodo* n) const { return n == nil ? "null" : to_string(n->val); }
    string campo(const string& k, Nodo* n) const { return "\"" + k + "\":" + json(n); }

    Nodo* encontrar(int v) {
        Nodo* n = raiz;
        while (n != nil && n->val != v) {
            comparar(v, n);
            n = v < n->val ? n->izq : n->der;
        }
        return n;
    }

    Nodo* minimo(Nodo* n) const {
        while (n->izq != nil) n = n->izq;
        return n;
    }

    void rotarIzq(Nodo* a, const string& nota) {
        Nodo* b = a->der;
        a->der = b->izq;
        if (b->izq != nil) b->izq->padre = a;
        b->padre = a->padre;
        if (a->padre == nil) raiz = b;
        else if (a == a->padre->izq) a->padre->izq = b;
        else a->padre->der = b;
        b->izq = a;
        a->padre = b;
        paso("rotar_izq", "\"pivote\":" + json(a) + ",\"sube\":" + json(b), nota);
    }

    void rotarDer(Nodo* a, const string& nota) {
        Nodo* b = a->izq;
        a->izq = b->der;
        if (b->der != nil) b->der->padre = a;
        b->padre = a->padre;
        if (a->padre == nil) raiz = b;
        else if (a == a->padre->der) a->padre->der = b;
        else a->padre->izq = b;
        b->der = a;
        a->padre = b;
        paso("rotar_der", "\"pivote\":" + json(a) + ",\"sube\":" + json(b), nota);
    }

    void arreglarInsercion(Nodo* n) {
        while (n->padre->color == ROJO) {
            Nodo* p = n->padre;
            Nodo* abu = p->padre;
            if (p == abu->izq) {
                Nodo* tio = abu->der;
                if (tio->color == ROJO) {
                    p->color = NEGRO; tio->color = NEGRO; abu->color = ROJO;
                    recolorear({p, tio, abu}, "Caso 1 (tío " + str(tio) + " rojo): padre y tío pasan a negro, el abuelo " + str(abu) + " a rojo");
                    n = abu;
                } else {
                    if (n == p->der) {
                        n = p;
                        rotarIzq(n, "Caso 2 (tío negro, zigzag): rotación a la izquierda en " + str(n) + " para dejarlos en línea");
                        p = n->padre;
                    }
                    p->color = NEGRO; abu->color = ROJO;
                    recolorear({p, abu}, "Caso 3 (tío negro, en línea): el padre " + str(p) + " pasa a negro y el abuelo " + str(abu) + " a rojo");
                    rotarDer(abu, "Caso 3: rotación a la derecha en el abuelo " + str(abu));
                }
            } else {
                Nodo* tio = abu->izq;
                if (tio->color == ROJO) {
                    p->color = NEGRO; tio->color = NEGRO; abu->color = ROJO;
                    recolorear({p, tio, abu}, "Caso 1 (tío " + str(tio) + " rojo): padre y tío pasan a negro, el abuelo " + str(abu) + " a rojo");
                    n = abu;
                } else {
                    if (n == p->izq) {
                        n = p;
                        rotarDer(n, "Caso 2 (tío negro, zigzag): rotación a la derecha en " + str(n) + " para dejarlos en línea");
                        p = n->padre;
                    }
                    p->color = NEGRO; abu->color = ROJO;
                    recolorear({p, abu}, "Caso 3 (tío negro, en línea): el padre " + str(p) + " pasa a negro y el abuelo " + str(abu) + " a rojo");
                    rotarIzq(abu, "Caso 3: rotación a la izquierda en el abuelo " + str(abu));
                }
            }
        }
        if (raiz->color == ROJO) {
            raiz->color = NEGRO;
            recolorear({raiz}, "La raíz siempre tiene que ser negra");
        }
    }

    void cambiar(Nodo* a, Nodo* b) {
        if (a->padre == nil) raiz = b;
        else if (a == a->padre->izq) a->padre->izq = b;
        else a->padre->der = b;
        b->padre = a->padre;
    }

    void arreglarEliminacion(Nodo* n) {
        while (n != raiz && n->color == NEGRO) {
            if (n == n->padre->izq) {
                Nodo* hno = n->padre->der;
                if (hno->color == ROJO) {
                    hno->color = NEGRO; n->padre->color = ROJO;
                    recolorear({hno, n->padre}, "Caso 1 (hermano " + str(hno) + " rojo): el hermano pasa a negro y el padre a rojo");
                    rotarIzq(n->padre, "Caso 1: rotación a la izquierda en el padre " + str(n->padre));
                    hno = n->padre->der;
                }
                if (hno->izq->color == NEGRO && hno->der->color == NEGRO) {
                    hno->color = ROJO;
                    recolorear({hno}, "Caso 2 (hermano " + str(hno) + " negro con hijos negros): el hermano pasa a rojo y el problema sube al padre");
                    n = n->padre;
                } else {
                    if (hno->der->color == NEGRO) {
                        hno->izq->color = NEGRO; hno->color = ROJO;
                        recolorear({hno->izq, hno}, "Caso 3 (hermano negro, sobrino cercano rojo): se intercambian colores");
                        rotarDer(hno, "Caso 3: rotación a la derecha en el hermano " + str(hno));
                        hno = n->padre->der;
                    }
                    hno->color = n->padre->color; n->padre->color = NEGRO; hno->der->color = NEGRO;
                    recolorear({hno, n->padre, hno->der}, "Caso 4 (hermano negro, sobrino lejano rojo): se ajustan colores");
                    rotarIzq(n->padre, "Caso 4: rotación a la izquierda en el padre " + str(n->padre) + ", queda arreglado");
                    n = raiz;
                }
            } else {
                Nodo* hno = n->padre->izq;
                if (hno->color == ROJO) {
                    hno->color = NEGRO; n->padre->color = ROJO;
                    recolorear({hno, n->padre}, "Caso 1 (hermano " + str(hno) + " rojo): el hermano pasa a negro y el padre a rojo");
                    rotarDer(n->padre, "Caso 1: rotación a la derecha en el padre " + str(n->padre));
                    hno = n->padre->izq;
                }
                if (hno->der->color == NEGRO && hno->izq->color == NEGRO) {
                    hno->color = ROJO;
                    recolorear({hno}, "Caso 2 (hermano " + str(hno) + " negro con hijos negros): el hermano pasa a rojo y el problema sube al padre");
                    n = n->padre;
                } else {
                    if (hno->izq->color == NEGRO) {
                        hno->der->color = NEGRO; hno->color = ROJO;
                        recolorear({hno->der, hno}, "Caso 3 (hermano negro, sobrino cercano rojo): se intercambian colores");
                        rotarIzq(hno, "Caso 3: rotación a la izquierda en el hermano " + str(hno));
                        hno = n->padre->izq;
                    }
                    hno->color = n->padre->color; n->padre->color = NEGRO; hno->izq->color = NEGRO;
                    recolorear({hno, n->padre, hno->izq}, "Caso 4 (hermano negro, sobrino lejano rojo): se ajustan colores");
                    rotarDer(n->padre, "Caso 4: rotación a la derecha en el padre " + str(n->padre) + ", queda arreglado");
                    n = raiz;
                }
            }
        }
        if (n->color == ROJO) {
            n->color = NEGRO;
            recolorear({n}, "El " + str(n) + " pasa a negro y se recupera la altura negra");
        }
        n->color = NEGRO;
    }

    int revisar(Nodo* n, long long mn, long long mx, bool& ok) const {
        if (n == nil) return 1;
        if (n->val <= mn || n->val >= mx) ok = false;
        if (n->color == ROJO && (n->izq->color == ROJO || n->der->color == ROJO)) ok = false;
        if (n->izq != nil && n->izq->padre != n) ok = false;
        if (n->der != nil && n->der->padre != n) ok = false;
        int a = revisar(n->izq, mn, n->val, ok);
        int b = revisar(n->der, n->val, mx, ok);
        if (a != b) ok = false;  // altura negra
        return a + (n->color == NEGRO);
    }

    int altura(Nodo* n) const {
        if (n == nil) return 0;
        return 1 + max(altura(n->izq), altura(n->der));
    }

    void inorden(Nodo* n, vector<int>& res) const {
        if (n == nil) return;
        inorden(n->izq, res);
        res.push_back(n->val);
        inorden(n->der, res);
    }

    void borrarTodo(Nodo* n) {
        if (n == nil) return;
        borrarTodo(n->izq);
        borrarTodo(n->der);
        delete n;
    }
};

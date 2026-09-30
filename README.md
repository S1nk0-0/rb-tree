# Red-Black Tree (parte P1)

## Compilar y correr

```bash
g++ -std=c++17 -O2 -o rbtree main.cpp
./rbtree ops/demo.txt salida/demo.json

g++ -std=c++17 -O2 -o tests tests.cpp
./tests
```

El archivo de operaciones tiene un comando por línea: `i 10` inserta el 10, `d 10` lo elimina y `s 10` lo busca.

## Archivos

- `rbtree.hpp`: el árbol, hecho desde cero sin `std::set` ni `std::map`.
- `main.cpp`: lee las operaciones y guarda los pasos en un JSON.
- `tests.cpp`: pruebas de casos borde y 20 000 operaciones aleatorias. Después de cada operación revisa que el árbol cumpla las reglas.
- `ops/`: operaciones de ejemplo. `demo.txt` mezcla inserciones, búsquedas y eliminaciones, `caso_borde_orden.txt` inserta del 1 al 7 en orden y `caso_borde_vacio.txt` prueba el árbol vacío, un solo elemento y duplicados.
- `salida/`: los JSON ya generados para la animación.

## Formato del JSON

```json
{
  "estructura": "Red-Black Tree",
  "operaciones": [
    {
      "op": "insertar",
      "valor": 3,
      "resultado": "ok",
      "pasos": [
        { "accion": "comparar", "nodo": 1, "direccion": "der",
          "nota": "3 > 1, baja a la derecha", "arbol": { ... } }
      ]
    }
  ]
}
```

Cada paso incluye `arbol`, que es el árbol completo tal como quedó después de ese paso. Para animar basta con dibujar ese árbol y mover los nodos que cambiaron respecto al paso anterior.

Un nodo de `arbol` se ve así:

```json
{ "clave": 10, "color": "N", "izq": { ... }, "der": null }
```

`"R"` es rojo, `"N"` es negro y `null` es una hoja vacía.

| accion | Campos extra | Qué mostrar |
|---|---|---|
| `comparar` | `nodo`, `direccion` (`izq`/`der`) | resaltar el nodo y marcar hacia dónde baja |
| `insertar_nodo` | `nodo`, `padre` | aparece el nodo nuevo en rojo |
| `recolorear` | `nodos: [{clave, color}]` | cambiar el color de esos nodos |
| `rotar_izq` / `rotar_der` | `pivote`, `sube` | mover los nodos a su nueva posición |
| `sucesor` | `nodo` | resaltar el nodo que reemplaza al eliminado |
| `eliminar_nodo` | `nodo` | quitar el nodo |
| `encontrado` / `no_encontrado` | `nodo` (si existe) | resaltarlo o mostrar un aviso |
| `duplicado` | `nodo` | avisar que ya existe |

El campo `nota` se puede usar como subtítulo en el video.

## Casos borde

1. Árbol vacío: buscar o eliminar no rompe nada y devuelve que no se encontró.
2. Un solo elemento: la raíz queda negra y, al eliminarla, el árbol vuelve a estar vacío.
3. Duplicados: no se insertan.
4. Inserción en orden, que es el peor caso de un BST normal. Con los números del 1 al 1000, un BST normal llega a altura 1000. El Red-Black queda en 17, debajo del límite teórico de 2·log₂(1001) ≈ 19,9. En el video conviene usar `caso_borde_orden.txt` (1 al 7) porque entra bien en pantalla.

## Complejidad

Sea *n* la cantidad de nodos y *h* la altura.

La altura es O(log n) por lo siguiente. Todos los caminos de la raíz a una hoja tienen la misma cantidad de nodos negros, que llamamos altura negra (*bh*). Como un nodo rojo no puede tener hijos rojos, al menos la mitad de cada camino son nodos negros, así que *h* ≤ 2·*bh*. Un subárbol con altura negra *bh* tiene al menos 2^*bh* − 1 nodos, entonces *n* ≥ 2^(h/2) − 1 y despejando sale h ≤ 2·log₂(n + 1).

| Operación | Mejor caso | Peor caso | Por qué |
|---|---|---|---|
| Buscar | O(1), si está en la raíz | O(log n) | Baja un nivel en cada comparación. |
| Insertar | O(log n) | O(log n) | Llegar a la posición cuesta O(log n). El arreglo recolorea subiendo por el árbol como máximo O(log n) veces y hace a lo mucho 2 rotaciones. |
| Eliminar | O(log n) | O(log n) | Encontrar el nodo y su sucesor cuesta O(log n). El arreglo sube como máximo O(log n) veces y hace a lo mucho 3 rotaciones. |
| Rotación | O(1) | O(1) | Solo cambia unos cuantos punteros. |

El espacio es O(n). Todas las hojas apuntan al mismo nodo `nil`, así que las hojas vacías no ocupan memoria extra.

En un BST sin balanceo, si los datos llegan ordenados el árbol se vuelve una lista y cada operación cuesta O(n). El Red-Black lo evita rebalanceándose con rotaciones y recoloreos.

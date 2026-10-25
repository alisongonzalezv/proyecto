#ifndef SERVICIOINVENTARIO_H
#define SERVICIOINVENTARIO_H

#include "RepositorioProductos.h"
#include <memory>
#include <algorithm>

class ServicioInventario {
private:
    std::shared_ptr<RepositorioProductos> repo;

public:
    explicit ServicioInventario(std::shared_ptr<RepositorioProductos> repo) : repo(repo) {
        repo->cargar();
    }

    bool agregarProducto(const Producto& p) {
        auto& lista = repo->obtenerTodos();
        for (const auto& item : lista) {
            if (item.getCodigo() == p.getCodigo()) return false; // Código duplicado
        }
        lista.push_back(p);
        return repo->guardar();
    }

    bool descontarStock(const QString& idProducto, int cantidad) {
        auto& lista = repo->obtenerTodos();
        for (auto& item : lista) {
            if (item.getId() == idProducto) {
                if (item.getExistencias() < cantidad) return false; // Stock insuficiente
                item.setExistencias(item.getExistencias() - cantidad);
                return repo->guardar();
            }
        }
        return false;
    }

    bool reponerStock(const QString& idProducto, int cantidad) {
        auto& lista = repo->obtenerTodos();
        for (auto& item : lista) {
            if (item.getId() == idProducto) {
                item.setExistencias(item.getExistencias() + cantidad);
                return repo->guardar();
            }
        }
        return false;
    }

    std::vector<Producto> obtenerBajoStock() {
        std::vector<Producto> resultado;
        for (const auto& item : repo->obtenerTodos()) {
            if (item.esActivo() && item.esBajoStock()) {
                resultado.push_back(item);
            }
        }
        return resultado;
    }

    std::vector<Producto> obtenerTodos() {
        return repo->obtenerTodos();
    }
};

#endif // SERVICIOINVENTARIO_H
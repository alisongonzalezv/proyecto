#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <QString>

class Producto {
private:
    QString id;
    QString codigo;
    QString nombre;
    double precio;
    int existencias;
    int stockMinimo;
    bool activo;

public:
    Producto() : precio(0.0), existencias(0), stockMinimo(0), activo(true) {}

    Producto(const QString& id, const QString& codigo, const QString& nombre,
             double precio, int existencias, int stockMinimo, bool activo = true)
        : id(id), codigo(codigo), nombre(nombre), precio(precio),
          existencias(existencias), stockMinimo(stockMinimo), activo(activo) {}

    bool esBajoStock() const { return existencias <= stockMinimo; }

    // Getters
    QString getId() const { return id; }
    QString getCodigo() const { return codigo; }
    QString getNombre() const { return nombre; }
    double getPrecio() const { return precio; }
    int getExistencias() const { return existencias; }
    int getStockMinimo() const { return stockMinimo; }
    bool esActivo() const { return activo; }

    // Setters con validaciones básicas
    void setCodigo(const QString& nuevoCodigo) { codigo = nuevoCodigo; }
    void setNombre(const QString& nuevoNombre) { nombre = nuevoNombre; }
    void setPrecio(double nuevoPrecio) { if (nuevoPrecio >= 0) precio = nuevoPrecio; }
    void setExistencias(int nuevasExistencias) { if (nuevasExistencias >= 0) existencias = nuevasExistencias; }
    void setStockMinimo(int nuevoMinimo) { if (nuevoMinimo >= 0) stockMinimo = nuevoMinimo; }
    void setActivo(bool estado) { activo = estado; }
};

#endif // PRODUCTO_H
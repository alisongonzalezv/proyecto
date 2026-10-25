#ifndef USUARIO_H
#define USUARIO_H

#include <QString>

class Usuario {
protected:
    QString id;
    QString nombre;
    QString nombreUsuario;
    QString hashContrasena;
    bool activo;

public:
    Usuario(const QString& id, const QString& nombre, const QString& nombreUsuario,
            const QString& hashContrasena, bool activo = true)
        : id(id), nombre(nombre), nombreUsuario(nombreUsuario),
          hashContrasena(hashContrasena), activo(activo) {}

    // Constructor simplificado para inicio rápido y gestión flexible
    Usuario(const QString& usuario, const QString& clave)
        : id(usuario), nombre(usuario), nombreUsuario(usuario),
          hashContrasena(clave), activo(true) {}

    virtual ~Usuario() = default;

    QString getId() const { return id; }
    QString getNombre() const { return nombre; }
    QString getNombreUsuario() const { return nombreUsuario; }
    QString getClave() const { return hashContrasena; }
    bool isActivo() const { return activo; }

    void setClave(const QString& nuevaClave) { hashContrasena = nuevaClave; }

    virtual QString rol() const = 0;
};

class Administrador : public Usuario {
public:
    Administrador(const QString& id, const QString& nombre, const QString& nombreUsuario,
                  const QString& hashContrasena, bool activo = true)
        : Usuario(id, nombre, nombreUsuario, hashContrasena, activo) {}

    Administrador(const QString& usuario, const QString& clave)
        : Usuario(usuario, clave) {}

    QString rol() const override { return "Administrador"; }
};

class Cajero : public Usuario {
public:
    Cajero(const QString& id, const QString& nombre, const QString& nombreUsuario,
           const QString& hashContrasena, bool activo = true)
        : Usuario(id, nombre, nombreUsuario, hashContrasena, activo) {}

    Cajero(const QString& usuario, const QString& clave)
        : Usuario(usuario, clave) {}

    QString rol() const override { return "Cajero"; }
};

#endif // USUARIO_H
#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <memory>
#include "Usuario.h"

class VentanaPrincipal : public QMainWindow {
    Q_OBJECT

private:
    std::shared_ptr<Usuario> usuarioActual;
    QStackedWidget* panelCentral;

    QPushButton* btnVentas;
    QPushButton* btnInventario;
    QPushButton* btnHistorial;
    QPushButton* btnUsuarios;
    QPushButton* btnCerrarSesion;

public:
    explicit VentanaPrincipal(std::shared_ptr<Usuario> usuario, QWidget* parent = nullptr);

private:
    void configurarInterfaz();
    void aplicarPermisosRol();
    QPushButton* crearBotonMenu(const QString& texto, bool activo = false);
};

#endif // VENTANAPRINCIPAL_H
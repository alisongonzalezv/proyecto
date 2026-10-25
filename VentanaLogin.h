#ifndef VENTANALOGIN_H
#define VENTANALOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <memory>
#include "Usuario.h"

class VentanaLogin : public QDialog {
    Q_OBJECT

private:
    QLineEdit* txtUsuario;
    QLineEdit* txtClave;
    QPushButton* btnIngresar;

    std::shared_ptr<Usuario> usuarioAutenticado;

public:
    explicit VentanaLogin(QWidget* parent = nullptr);
    std::shared_ptr<Usuario> getUsuarioAutenticado() const;

private:
    void intentarLogin();
};

#endif // VENTANALOGIN_H
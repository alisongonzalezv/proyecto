#ifndef VISTAUSUARIOS_H
#define VISTAUSUARIOS_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QComboBox>
#include <vector>
#include <memory>
#include "Usuario.h"
#include "GestorJSON.h"

class VistaUsuarios : public QWidget {
    Q_OBJECT

private:
    QLineEdit* txtBuscar;
    QTableWidget* tablaUsuarios;
    QPushButton* btnAgregar;
    QPushButton* btnEditar;
    QPushButton* btnEliminar;

    std::vector<std::shared_ptr<Usuario>> listaUsuarios;

public:
    explicit VistaUsuarios(QWidget* parent = nullptr);
    void actualizar();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void configurarInterfaz();
    void cargarDatos();
    void renderizarTabla();
    void filtrarTabla();
    void abrirFormularioUsuario(std::shared_ptr<Usuario> usuarioExistente = nullptr);
    void eliminarUsuarioSeleccionado();
};

#endif // VISTAUSUARIOS_H
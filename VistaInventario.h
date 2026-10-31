#ifndef VISTAINVENTARIO_H
#define VISTAINVENTARIO_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <vector>
#include "VistaVentas.h"
#include "GestorJSON.h"

class VistaInventario : public QWidget {
    Q_OBJECT

private:
    QLineEdit* txtBuscar;
    QComboBox* comboCategoriaFiltro;
    QTableWidget* tablaProductos;

    QPushButton* btnAgregar;
    QPushButton* btnEditar;
    QPushButton* btnEliminar;

    std::vector<ProductoItem> listaProductos;

public:
    explicit VistaInventario(QWidget* parent = nullptr);
    void actualizar(); // Permite recargar la tabla externamente

signals:
    void productosActualizados();

protected:
    void showEvent(QShowEvent* event) override; // Se ejecuta automáticamente al abrir la vista

private:
    void configurarInterfaz();
    void cargarDatos();
    void renderizarTabla();
    void actualizarCategoriasFiltro();

    void abrirFormularioProducto(ProductoItem* productoExistente = nullptr);
    bool guardarProductoEnJSON(const ProductoItem& prod, bool esEdicion);
    void eliminarProductoSeleccionado();
    void filtrarTabla();
};

#endif // VISTAINVENTARIO_H

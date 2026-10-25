#ifndef VISTAHISTORIAL_H
#define VISTAHISTORIAL_H

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
#include <QTextBrowser>
#include <QComboBox>
#include <QDateEdit>
#include <QFileDialog>
#include <QTextStream>
#include <QDate>
#include <vector>
#include "GestorJSON.h"
#include "IGeneradorTicket.h"

class VistaHistorial : public QWidget {
    Q_OBJECT

private:
    QLineEdit* txtBuscar;
    QComboBox* comboRangoFecha;
    QDateEdit* dateInicio;
    QDateEdit* dateFin;
    QTableWidget* tablaVentas;
    QPushButton* btnVerTicket;
    QPushButton* btnExportarCSV;
    QPushButton* btnAnularVenta;

    QLabel* lblTotalVendido;
    QLabel* lblTotalIVA;
    QLabel* lblCantidadVentas;

    std::vector<DatosVentaTicket> listaVentas;

public:
    explicit VistaHistorial(QWidget* parent = nullptr);
    void actualizar();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void configurarInterfaz();
    void cargarDatos();
    void renderizarTabla();
    void filtrarTabla();
    void mostrarTicketSeleccionado();
    void exportarCSV();
    void anularVentaSeleccionada();
};

#endif // VISTAHISTORIAL_H
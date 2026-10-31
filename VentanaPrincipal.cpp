#include "VentanaPrincipal.h"
#include "VistaVentas.h"
#include "VistaInventario.h"
#include "VistaHistorial.h"
#include "VistaUsuarios.h"

VentanaPrincipal::VentanaPrincipal(std::shared_ptr<Usuario> usuario, QWidget* parent)
    : QMainWindow(parent), usuarioActual(usuario) {
    setWindowTitle("UNA Pulpe - Sistema POS");
    resize(1200, 750);

    configurarInterfaz();
    aplicarPermisosRol();
}

void VentanaPrincipal::configurarInterfaz() {
    QWidget* widgetCentral = new QWidget(this);
    QHBoxLayout* layoutRaiz = new QHBoxLayout(widgetCentral);
    layoutRaiz->setContentsMargins(0, 0, 0, 0);
    layoutRaiz->setSpacing(0);

    widgetCentral->setStyleSheet("background-color: #f8f9fa;");

    // BARRA NAVEGACIÓN LATERAL
    QFrame* panelLateral = new QFrame(this);
    panelLateral->setFixedWidth(230);
    panelLateral->setStyleSheet(
        "QFrame { background-color: #ffffff; border-right: 1px solid #e5e7eb; }"
    );

    QVBoxLayout* layoutMenu = new QVBoxLayout(panelLateral);
    layoutMenu->setContentsMargins(20, 25, 20, 25);
    layoutMenu->setSpacing(10);

    QLabel* lblLogoTexto = new QLabel("UNA Pulpe", this);
    lblLogoTexto->setStyleSheet(
        "QLabel { font-size: 22px; font-weight: bold; color: #10b981; border: none; background: transparent; }"
    );
    layoutMenu->addWidget(lblLogoTexto);

    QLabel* lblSub = new QLabel("Punto de Venta", this);
    lblSub->setStyleSheet(
        "QLabel { font-size: 12px; color: #6b7280; border: none; background: transparent; margin-bottom: 10px; }"
    );
    layoutMenu->addWidget(lblSub);

    QLabel* lblRol = new QLabel(usuarioActual->rol().toUpper(), this);
    lblRol->setAlignment(Qt::AlignCenter);
    lblRol->setStyleSheet(
        "background-color: #ecfdf5; color: #10b981; font-weight: bold; "
        "padding: 8px 12px; border-radius: 8px; font-size: 12px; border: none; margin-bottom: 15px;"
    );
    layoutMenu->addWidget(lblRol);

    btnVentas = crearBotonMenu("Ventas", true);
    btnInventario = crearBotonMenu("Inventario");
    btnHistorial = crearBotonMenu("Historial");
    btnUsuarios = crearBotonMenu("Usuarios");

    layoutMenu->addWidget(btnVentas);
    layoutMenu->addWidget(btnInventario);
    layoutMenu->addWidget(btnHistorial);
    layoutMenu->addWidget(btnUsuarios);

    layoutMenu->addStretch();

    btnCerrarSesion = new QPushButton("Cerrar Sesión", this);
    btnCerrarSesion->setStyleSheet(
        "QPushButton { background-color: #fef2f2; color: #ef4444; font-weight: bold; "
        "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        "QPushButton:hover { background-color: #fee2e2; }"
    );
    layoutMenu->addWidget(btnCerrarSesion);

    layoutRaiz->addWidget(panelLateral);

    // PANEL CONTENEDOR DERECHO
    QWidget* areaDerecha = new QWidget(this);
    QVBoxLayout* layoutDerecha = new QVBoxLayout(areaDerecha);
    layoutDerecha->setContentsMargins(25, 20, 25, 20);
    layoutDerecha->setSpacing(20);

    QHBoxLayout* layoutHeader = new QHBoxLayout();

    QLabel* lblSaludo = new QLabel(QString("Bienvenido, %1").arg(usuarioActual->getNombreUsuario()), this);
    lblSaludo->setStyleSheet("font-size: 18px; font-weight: bold; color: #111827;");

    QLabel* lblFecha = new QLabel("Sistema POS Activo", this);
    lblFecha->setStyleSheet("font-size: 13px; color: #6b7280;");

    layoutHeader->addWidget(lblSaludo);
    layoutHeader->addStretch();
    layoutHeader->addWidget(lblFecha);

    layoutDerecha->addLayout(layoutHeader);

    panelCentral = new QStackedWidget(this);
    panelCentral->setStyleSheet("background: transparent;");

    VistaVentas* vistaVentas = new VistaVentas(usuarioActual->getNombreUsuario(), this);
    VistaInventario* vistaInventario = new VistaInventario(this);
    connect(vistaInventario, &VistaInventario::productosActualizados,
            vistaVentas, &VistaVentas::actualizarCatalogo);
    VistaHistorial* vistaHistorial = new VistaHistorial(this);
    VistaUsuarios* vistaUsuarios = new VistaUsuarios(this);

    panelCentral->addWidget(vistaVentas);
    panelCentral->addWidget(vistaInventario);
    panelCentral->addWidget(vistaHistorial);
    panelCentral->addWidget(vistaUsuarios);

    layoutDerecha->addWidget(panelCentral);
    layoutRaiz->addWidget(areaDerecha);

    setCentralWidget(widgetCentral);

    auto resetEstiloBotones = [this]() {
        btnVentas->setStyleSheet(crearBotonMenu("Ventas")->styleSheet());
        btnInventario->setStyleSheet(crearBotonMenu("Inventario")->styleSheet());
        btnHistorial->setStyleSheet(crearBotonMenu("Historial")->styleSheet());
        btnUsuarios->setStyleSheet(crearBotonMenu("Usuarios")->styleSheet());
    };

    connect(btnVentas, &QPushButton::clicked, [=]() {
        panelCentral->setCurrentIndex(0);
        resetEstiloBotones();
        btnVentas->setStyleSheet(
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
            "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        );
    });

    connect(btnInventario, &QPushButton::clicked, [=]() {
        panelCentral->setCurrentIndex(1);
        vistaInventario->actualizar();
        resetEstiloBotones();
        btnInventario->setStyleSheet(
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
            "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        );
    });

    connect(btnHistorial, &QPushButton::clicked, [=]() {
        panelCentral->setCurrentIndex(2);
        vistaHistorial->actualizar();
        resetEstiloBotones();
        btnHistorial->setStyleSheet(
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
            "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        );
    });

    connect(btnUsuarios, &QPushButton::clicked, [=]() {
        panelCentral->setCurrentIndex(3);
        vistaUsuarios->actualizar();
        resetEstiloBotones();
        btnUsuarios->setStyleSheet(
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
            "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        );
    });

    connect(btnCerrarSesion, &QPushButton::clicked, this, &QMainWindow::close);
}

QPushButton* VentanaPrincipal::crearBotonMenu(const QString& texto, bool activo) {
    QPushButton* btn = new QPushButton(texto, this);
    if (activo) {
        btn->setStyleSheet(
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
            "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        );
    } else {
        btn->setStyleSheet(
            "QPushButton { background-color: transparent; color: #4b5563; font-weight: 500; "
            "border: none; border-radius: 10px; padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
            "QPushButton:hover { background-color: #f3f4f6; color: #111827; }"
        );
    }
    return btn;
}

void VentanaPrincipal::aplicarPermisosRol() {
    if (usuarioActual->rol() == "Cajero") {
        btnUsuarios->setEnabled(false);
        btnUsuarios->setStyleSheet(
            "QPushButton { background-color: transparent; color: #d1d5db; border: none; "
            "padding: 12px; font-size: 13px; text-align: left; padding-left: 15px; }"
        );
    }
}

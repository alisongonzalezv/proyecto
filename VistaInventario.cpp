#include "VistaInventario.h"
#include <QShowEvent>

VistaInventario::VistaInventario(QWidget* parent) : QWidget(parent) {
    configurarInterfaz();
    cargarDatos();
}

void VistaInventario::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizar();
}

void VistaInventario::actualizar() {
    cargarDatos();
}

void VistaInventario::configurarInterfaz() {
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);
    layoutPrincipal->setSpacing(15);
    this->setStyleSheet("background-color: #f8f9fa;");

    // BARRA SUPERIOR
    QHBoxLayout* layoutTop = new QHBoxLayout();
    layoutTop->setSpacing(10);

    txtBuscar = new QLineEdit(this);
    txtBuscar->setPlaceholderText("Buscar por nombre o código...");
    txtBuscar->setStyleSheet(
        "QLineEdit { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; "
        "padding: 8px 12px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 1px solid #10b981; }"
    );
    connect(txtBuscar, &QLineEdit::textChanged, this, &VistaInventario::filtrarTabla);

    comboCategoriaFiltro = new QComboBox(this);
    comboCategoriaFiltro->addItems({"Todas las Categorías", "Snacks", "Bebidas", "Comida", "Útiles", "Higiene"});
    comboCategoriaFiltro->setStyleSheet(
        "QComboBox { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; "
        "padding: 6px 12px; font-size: 12px; color: #111827; }"
    );
    connect(comboCategoriaFiltro, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VistaInventario::filtrarTabla);

    btnAgregar = new QPushButton("+ Nuevo Producto", this);
    btnAgregar->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 16px; font-size: 13px; }"
        "QPushButton:hover { background-color: #059669; }"
    );
    connect(btnAgregar, &QPushButton::clicked, [this]() { abrirFormularioProducto(nullptr); });

    btnEditar = new QPushButton(" Editar", this);
    btnEditar->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 16px; font-size: 13px; }"
        "QPushButton:hover { background-color: #2563eb; }"
    );
    connect(btnEditar, &QPushButton::clicked, [this]() {
        int fila = tablaProductos->currentRow();
        if (fila < 0) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Selección requerida");
            msgBox.setText("Por favor, seleccione un producto de la tabla para editar.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
            msgBox.exec();
            return;
        }
        QString idProd = tablaProductos->item(fila, 0)->text();
        for (auto& p : listaProductos) {
            if (p.id == idProd) {
                abrirFormularioProducto(&p);
                break;
            }
        }
    });

    btnEliminar = new QPushButton(" Eliminar", this);
    btnEliminar->setStyleSheet(
        "QPushButton { background-color: #ef4444; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 16px; font-size: 13px; }"
        "QPushButton:hover { background-color: #dc2626; }"
    );
    connect(btnEliminar, &QPushButton::clicked, this, &VistaInventario::eliminarProductoSeleccionado);

    layoutTop->addWidget(txtBuscar, 4);
    layoutTop->addWidget(comboCategoriaFiltro, 2);
    layoutTop->addWidget(btnAgregar, 2);
    layoutTop->addWidget(btnEditar, 1);
    layoutTop->addWidget(btnEliminar, 1);

    layoutPrincipal->addLayout(layoutTop);

    // TABLA DE INVENTARIO
    tablaProductos = new QTableWidget(this);
    tablaProductos->setColumnCount(6);
    tablaProductos->setHorizontalHeaderLabels({"ID / Código", "Nombre", "Categoría", "Precio (₡)", "Stock", "Estado"});
    tablaProductos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaProductos->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaProductos->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaProductos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaProductos->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; gridline-color: #f3f4f6; }"
        "QHeaderView::section { background-color: #f9fafb; color: #374151; font-weight: bold; border: none; padding: 10px; font-size: 12px; }"
        "QTableWidget::item { padding: 8px; font-size: 13px; color: #111827; }"
        "QTableWidget::item:selected { background-color: #e0f2fe; color: #0369a1; }"
    );

    layoutPrincipal->addWidget(tablaProductos);
}

void VistaInventario::cargarDatos() {
    GestorJSON gestor;
    listaProductos = gestor.cargarProductos();

    if (listaProductos.empty()) {
        listaProductos = {
            {"S001", "Papas Tostadas Tosty", 750.0, "Snack de papa 115g", "Snacks", 25},
            {"S002", "Platanitos Salados", 600.0, "Plátano tostado 100g", "Snacks", 15},
            {"S003", "Doritos Queso Nacho", 850.0, "Snack de maíz 120g", "Snacks", 8},
            {"S004", "Chocolate Jet", 350.0, "Barra de chocolate 12g", "Snacks", 30},
            {"S005", "Galletas Chicky", 450.0, "Galletas con cobertura", "Snacks", 12},
            {"B001", "Agua Cristal 600ml", 600.0, "Agua embotellada", "Bebidas", 50},
            {"B002", "Café Caliente 8oz", 700.0, "Café negro / con leche", "Bebidas", 40},
            {"B003", "Coca-Cola 600ml", 1100.0, "Gaseosa en botella", "Bebidas", 18},
            {"B005", "Fresco Leche Fresa", 800.0, "Dos Pinos 315ml", "Bebidas", 10},
            {"C001", "Empanada de Carne", 1200.0, "Arreglada y caliente", "Comida", 5},
            {"C002", "Empanada de Queso", 1100.0, "Arreglada y caliente", "Comida", 7},
            {"C003", "Muffin de Chispas", 950.0, "Repostería dulce", "Comida", 9},
            {"U001", "Lapicero BIC Negro", 300.0, "Punto mediano", "Útiles", 100},
            {"U002", "Lapicero BIC Azul", 300.0, "Punto mediano", "Útiles", 100},
            {"U004", "Cuaderno Universitario", 1850.0, "100 hojas cosido", "Útiles", 20},
            {"H001", "Alcohol en Gel 60ml", 950.0, "Portátil antibacterial", "Higiene", 14},
            {"H002", "Pañuelos Kleenex", 450.0, "Paquete de bolsillo", "Higiene", 22}
        };
        gestor.guardarProductos(listaProductos);
    }

    renderizarTabla();
}

void VistaInventario::renderizarTabla() {
    tablaProductos->setRowCount(0);

    for (const auto& prod : listaProductos) {
        int fila = tablaProductos->rowCount();
        tablaProductos->insertRow(fila);

        QTableWidgetItem* itemID = new QTableWidgetItem(prod.id);
        QTableWidgetItem* itemNombre = new QTableWidgetItem(prod.nombre);
        QTableWidgetItem* itemCat = new QTableWidgetItem(prod.categoria);
        QTableWidgetItem* itemPrecio = new QTableWidgetItem(QString("₡ %1").arg(prod.precio, 0, 'f', 2));
        QTableWidgetItem* itemStock = new QTableWidgetItem(QString::number(prod.stock));
        QTableWidgetItem* itemEstado = new QTableWidgetItem();

        if (prod.stock <= 0) {
            itemEstado->setText("Agotado");
            itemEstado->setForeground(QBrush(QColor("#ef4444")));
            itemStock->setForeground(QBrush(QColor("#ef4444")));
        } else if (prod.stock <= 5) {
            itemEstado->setText("Stock Bajo");
            itemEstado->setForeground(QBrush(QColor("#f59e0b")));
            itemStock->setForeground(QBrush(QColor("#f59e0b")));
        } else {
            itemEstado->setText("Disponible");
            itemEstado->setForeground(QBrush(QColor("#10b981")));
        }

        tablaProductos->setItem(fila, 0, itemID);
        tablaProductos->setItem(fila, 1, itemNombre);
        tablaProductos->setItem(fila, 2, itemCat);
        tablaProductos->setItem(fila, 3, itemPrecio);
        tablaProductos->setItem(fila, 4, itemStock);
        tablaProductos->setItem(fila, 5, itemEstado);
    }
}

void VistaInventario::filtrarTabla() {
    QString texto = txtBuscar->text().trimmed().toLower();
    QString catFiltro = comboCategoriaFiltro->currentText();

    for (int i = 0; i < tablaProductos->rowCount(); ++i) {
        QString id = tablaProductos->item(i, 0)->text().toLower();
        QString nombre = tablaProductos->item(i, 1)->text().toLower();
        QString cat = tablaProductos->item(i, 2)->text();

        bool coincideTexto = texto.isEmpty() || id.contains(texto) || nombre.contains(texto);
        bool coincideCat = (catFiltro == "Todas las Categorías") || (cat == catFiltro);

        tablaProductos->setRowHidden(i, !(coincideTexto && coincideCat));
    }
}

void VistaInventario::abrirFormularioProducto(ProductoItem* prodExistente) {
    bool esEdicion = (prodExistente != nullptr);

    QDialog dlg(this);
    dlg.setWindowTitle(esEdicion ? "Editar Producto" : "Nuevo Producto");
    dlg.setFixedSize(400, 480);
    dlg.setStyleSheet("QDialog { background-color: #ffffff; } QLabel { color: #111827; font-size: 12px; font-weight: 500; }");

    QVBoxLayout* layoutDlg = new QVBoxLayout(&dlg);
    layoutDlg->setContentsMargins(20, 20, 20, 20);
    layoutDlg->setSpacing(10);

    QLabel* lblTitulo = new QLabel(esEdicion ? "Modificar Datos de Producto" : "Registrar Nuevo Producto", &dlg);
    lblTitulo->setStyleSheet("font-size: 16px; font-weight: bold; color: #111827; margin-bottom: 5px;");
    layoutDlg->addWidget(lblTitulo);

    QLineEdit* inputID = new QLineEdit(&dlg);
    inputID->setPlaceholderText("Código ID (ej: S006)");
    if (esEdicion) {
        inputID->setText(prodExistente->id);
        inputID->setEnabled(false);
    }

    QLineEdit* inputNombre = new QLineEdit(&dlg);
    inputNombre->setPlaceholderText("Nombre del producto");
    if (esEdicion) inputNombre->setText(prodExistente->nombre);

    QLineEdit* inputDesc = new QLineEdit(&dlg);
    inputDesc->setPlaceholderText("Descripción breve");
    if (esEdicion) inputDesc->setText(prodExistente->descripcion);

    QComboBox* comboCat = new QComboBox(&dlg);
    comboCat->addItems({"Snacks", "Bebidas", "Comida", "Útiles", "Higiene"});
    if (esEdicion) comboCat->setCurrentText(prodExistente->categoria);

    QDoubleSpinBox* spinPrecio = new QDoubleSpinBox(&dlg);
    spinPrecio->setRange(0.0, 100000.0);
    spinPrecio->setDecimals(2);
    spinPrecio->setPrefix("₡ ");
    spinPrecio->setMinimumHeight(35);
    if (esEdicion) spinPrecio->setValue(prodExistente->precio);

    QSpinBox* spinStock = new QSpinBox(&dlg);
    spinStock->setRange(0, 10000);
    spinStock->setMinimumHeight(35);
    if (esEdicion) spinStock->setValue(prodExistente->stock);

    QString estiloCampos =
        "QLineEdit, QComboBox, QDoubleSpinBox, QSpinBox { "
        "background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 6px; "
        "padding: 6px 10px; font-size: 13px; color: #111827; min-height: 25px; } "
        "QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus, QSpinBox:focus { border: 1px solid #10b981; }";

    inputID->setStyleSheet(estiloCampos);
    inputNombre->setStyleSheet(estiloCampos);
    inputDesc->setStyleSheet(estiloCampos);
    comboCat->setStyleSheet(estiloCampos);
    spinPrecio->setStyleSheet(estiloCampos);
    spinStock->setStyleSheet(estiloCampos);

    layoutDlg->addWidget(new QLabel("Código ID:", &dlg));
    layoutDlg->addWidget(inputID);
    layoutDlg->addWidget(new QLabel("Nombre:", &dlg));
    layoutDlg->addWidget(inputNombre);
    layoutDlg->addWidget(new QLabel("Descripción:", &dlg));
    layoutDlg->addWidget(inputDesc);
    layoutDlg->addWidget(new QLabel("Categoría:", &dlg));
    layoutDlg->addWidget(comboCat);

    QHBoxLayout* layoutValores = new QHBoxLayout();
    layoutValores->setSpacing(10);

    QVBoxLayout* colPrec = new QVBoxLayout();
    colPrec->addWidget(new QLabel("Precio:", &dlg));
    colPrec->addWidget(spinPrecio);

    QVBoxLayout* colStock = new QVBoxLayout();
    colStock->addWidget(new QLabel("Stock:", &dlg));
    colStock->addWidget(spinStock);

    layoutValores->addLayout(colPrec);
    layoutValores->addLayout(colStock);
    layoutDlg->addLayout(layoutValores);

    QPushButton* btnGuardar = new QPushButton("Guardar Producto", &dlg);
    btnGuardar->setMinimumHeight(40);
    btnGuardar->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border: none; border-radius: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: #059669; }"
    );

    connect(btnGuardar, &QPushButton::clicked, [&]() {
        if (inputID->text().trimmed().isEmpty() || inputNombre->text().trimmed().isEmpty()) {
            QMessageBox msgBox(&dlg);
            msgBox.setWindowTitle("Campos Vacíos");
            msgBox.setText("El código ID y el Nombre son obligatorios.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
            msgBox.exec();
            return;
        }

        ProductoItem p;
        p.id = inputID->text().trimmed();
        p.nombre = inputNombre->text().trimmed();
        p.descripcion = inputDesc->text().trimmed();
        p.categoria = comboCat->currentText();
        p.precio = spinPrecio->value();
        p.stock = spinStock->value();

        guardarProductoEnJSON(p, esEdicion);
        dlg.accept();
    });

    layoutDlg->addSpacing(10);
    layoutDlg->addWidget(btnGuardar);

    dlg.exec();
}

void VistaInventario::guardarProductoEnJSON(const ProductoItem& prod, bool esEdicion) {
    if (esEdicion) {
        for (auto& p : listaProductos) {
            if (p.id == prod.id) {
                p = prod;
                break;
            }
        }
    } else {
        for (const auto& p : listaProductos) {
            if (p.id == prod.id) {
                QMessageBox msgBox(this);
                msgBox.setWindowTitle("ID Duplicado");
                msgBox.setText("Ya existe un producto registrado con este código ID.");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
                msgBox.exec();
                return;
            }
        }
        listaProductos.push_back(prod);
    }

    GestorJSON gestor;
    if (gestor.guardarProductos(listaProductos)) {
        renderizarTabla();

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Éxito");
        msgBox.setText("El inventario se ha actualizado correctamente.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #ffffff; } "
            "QLabel { color: #111827; font-size: 13px; font-weight: 500; } "
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }"
            "QPushButton:hover { background-color: #059669; }"
        );
        msgBox.exec();
    }
}

void VistaInventario::eliminarProductoSeleccionado() {
    int fila = tablaProductos->currentRow();
    if (fila < 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Selección requerida");
        msgBox.setText("Seleccione un producto de la tabla para eliminar.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QString idProd = tablaProductos->item(fila, 0)->text();
    QString nombreProd = tablaProductos->item(fila, 1)->text();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Confirmar eliminación");
    msgBox.setText(QString("¿Está seguro de que desea eliminar '%1' del inventario?").arg(nombreProd));
    msgBox.setIcon(QMessageBox::Question);
    QPushButton* btnSi = msgBox.addButton("Sí", QMessageBox::YesRole);
    msgBox.addButton("No", QMessageBox::NoRole);
    msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #f3f4f6; color: #111827; font-weight: bold; border-radius: 6px; padding: 6px 12px; font-size: 12px; }");
    msgBox.exec();

    if (msgBox.clickedButton() == btnSi) {
        for (auto it = listaProductos.begin(); it != listaProductos.end(); ++it) {
            if (it->id == idProd) {
                listaProductos.erase(it);
                break;
            }
        }

        GestorJSON gestor;
        gestor.guardarProductos(listaProductos);
        renderizarTabla();
    }
}


#include "VistaHistorial.h"
#include <QShowEvent>

VistaHistorial::VistaHistorial(QWidget* parent) : QWidget(parent) {
    configurarInterfaz();
    cargarDatos();
}

void VistaHistorial::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizar();
}

void VistaHistorial::actualizar() {
    cargarDatos();
}

void VistaHistorial::configurarInterfaz() {
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);
    layoutPrincipal->setSpacing(15);
    this->setStyleSheet("background-color: #f8f9fa;");

    // --- CARDS SUPERIORES ---
    QHBoxLayout* layoutCards = new QHBoxLayout();
    layoutCards->setSpacing(15);

    auto crearCard = [this](const QString& titulo, QLabel*& lblValor, const QString& colorTexto) {
        QFrame* card = new QFrame(this);
        card->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e5e7eb; }");
        QVBoxLayout* layoutCard = new QVBoxLayout(card);
        layoutCard->setContentsMargins(15, 12, 15, 12);

        QLabel* lblTitle = new QLabel(titulo, card);
        lblTitle->setStyleSheet("color: #6b7280; font-size: 11px; font-weight: bold; border: none;");

        lblValor = new QLabel("₡ 0.00", card);
        lblValor->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold; border: none;").arg(colorTexto));

        layoutCard->addWidget(lblTitle);
        layoutCard->addWidget(lblValor);
        return card;
    };

    layoutCards->addWidget(crearCard("TOTAL VENTAS", lblTotalVendido, "#10b981"));
    layoutCards->addWidget(crearCard("IVA RECAUDADO (13%)", lblTotalIVA, "#3b82f6"));
    layoutCards->addWidget(crearCard("TRANSACCIONES", lblCantidadVentas, "#111827"));

    layoutPrincipal->addLayout(layoutCards);

    // --- BARRA DE FILTROS Y CONTROLES ---
    QHBoxLayout* layoutTop = new QHBoxLayout();
    layoutTop->setSpacing(10);

    txtBuscar = new QLineEdit(this);
    txtBuscar->setPlaceholderText("Buscar...");
    txtBuscar->setStyleSheet(
        "QLineEdit { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; "
        "padding: 8px 12px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 1px solid #10b981; }"
    );
    connect(txtBuscar, &QLineEdit::textChanged, this, &VistaHistorial::filtrarTabla);

    comboRangoFecha = new QComboBox(this);
    comboRangoFecha->addItems({"Todas las Fechas", "Hoy", "Últimos 7 Días", "Este Mes", "Rango Personalizado"});
    comboRangoFecha->setStyleSheet(
        "QComboBox { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; "
        "padding: 6px 10px; font-size: 12px; color: #111827; }"
    );

    dateInicio = new QDateEdit(QDate::currentDate(), this);
    dateInicio->setCalendarPopup(true);
    dateInicio->setDisplayFormat("yyyy-MM-dd");
    dateInicio->setVisible(false);
    dateInicio->setStyleSheet("QDateEdit { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 6px; padding: 4px; font-size: 12px; color: #111827; }");

    dateFin = new QDateEdit(QDate::currentDate(), this);
    dateFin->setCalendarPopup(true);
    dateFin->setDisplayFormat("yyyy-MM-dd");
    dateFin->setVisible(false);
    dateFin->setStyleSheet("QDateEdit { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 6px; padding: 4px; font-size: 12px; color: #111827; }");

    connect(comboRangoFecha, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        bool esPersonalizado = (comboRangoFecha->itemText(index) == "Rango Personalizado");
        dateInicio->setVisible(esPersonalizado);
        dateFin->setVisible(esPersonalizado);
        filtrarTabla();
    });

    connect(dateInicio, &QDateEdit::dateChanged, this, &VistaHistorial::filtrarTabla);
    connect(dateFin, &QDateEdit::dateChanged, this, &VistaHistorial::filtrarTabla);

    btnVerTicket = new QPushButton(" Ver Ticket", this);
    btnVerTicket->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 12px; font-size: 12px; }"
        "QPushButton:hover { background-color: #059669; }"
    );
    connect(btnVerTicket, &QPushButton::clicked, this, &VistaHistorial::mostrarTicketSeleccionado);

    btnExportarCSV = new QPushButton(" CSV", this);
    btnExportarCSV->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 12px; font-size: 12px; }"
        "QPushButton:hover { background-color: #2563eb; }"
    );
    connect(btnExportarCSV, &QPushButton::clicked, this, &VistaHistorial::exportarCSV);

    btnAnularVenta = new QPushButton(" Anular Venta", this);
    btnAnularVenta->setStyleSheet(
        "QPushButton { background-color: #ef4444; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 12px; font-size: 12px; }"
        "QPushButton:hover { background-color: #dc2626; }"
    );
    connect(btnAnularVenta, &QPushButton::clicked, this, &VistaHistorial::anularVentaSeleccionada);

    layoutTop->addWidget(txtBuscar, 2);
    layoutTop->addWidget(comboRangoFecha, 2);
    layoutTop->addWidget(dateInicio, 1);
    layoutTop->addWidget(dateFin, 1);
    layoutTop->addWidget(btnVerTicket, 1);
    layoutTop->addWidget(btnExportarCSV, 1);
    layoutTop->addWidget(btnAnularVenta, 1);

    layoutPrincipal->addLayout(layoutTop);

    // --- TABLA HISTORIAL ---
    tablaVentas = new QTableWidget(this);
    tablaVentas->setColumnCount(6);
    tablaVentas->setHorizontalHeaderLabels({"ID Venta", "Fecha y Hora", "Cajero", "Subtotal (₡)", "IVA (₡)", "Total (₡)"});
    tablaVentas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaVentas->verticalHeader()->setVisible(false);
    tablaVentas->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaVentas->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaVentas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaVentas->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; gridline-color: #f3f4f6; }"
        "QHeaderView::section { background-color: #f9fafb; color: #374151; font-weight: bold; border: none; padding: 10px; font-size: 12px; }"
        "QTableWidget::item { padding: 10px 8px; font-size: 13px; color: #111827; }"
        "QTableWidget::item:selected { background-color: #e0f2fe; color: #0369a1; }"
    );

    layoutPrincipal->addWidget(tablaVentas);
}

void VistaHistorial::cargarDatos() {
    GestorJSON gestor;
    listaVentas = gestor.cargarVentas();
    renderizarTabla();
}

void VistaHistorial::renderizarTabla() {
    tablaVentas->setRowCount(0);

    for (const auto& venta : listaVentas) {
        int fila = tablaVentas->rowCount();
        tablaVentas->insertRow(fila);

        QTableWidgetItem* itemID = new QTableWidgetItem(venta.idVenta);
        QTableWidgetItem* itemFecha = new QTableWidgetItem(venta.fechaHora.toString("yyyy-MM-dd hh:mm AP"));
        QTableWidgetItem* itemCajero = new QTableWidgetItem(venta.cajero);
        QTableWidgetItem* itemSub = new QTableWidgetItem(QString("₡ %1").arg(venta.subtotal, 0, 'f', 2));
        QTableWidgetItem* itemIVA = new QTableWidgetItem(QString("₡ %1").arg(venta.iva, 0, 'f', 2));
        QTableWidgetItem* itemTotal = new QTableWidgetItem(QString("₡ %1").arg(venta.total, 0, 'f', 2));

        itemTotal->setForeground(QBrush(QColor("#10b981")));

        tablaVentas->setItem(fila, 0, itemID);
        tablaVentas->setItem(fila, 1, itemFecha);
        tablaVentas->setItem(fila, 2, itemCajero);
        tablaVentas->setItem(fila, 3, itemSub);
        tablaVentas->setItem(fila, 4, itemIVA);
        tablaVentas->setItem(fila, 5, itemTotal);
    }

    filtrarTabla();
}

void VistaHistorial::filtrarTabla() {
    QString texto = txtBuscar->text().trimmed().toLower();
    QString opcionFecha = comboRangoFecha->currentText();
    QDate hoy = QDate::currentDate();

    double acumuladoVentasVisibles = 0.0;
    double acumuladoIVAVisibles = 0.0;
    int contadorVisibles = 0;

    for (int i = 0; i < tablaVentas->rowCount(); ++i) {
        QString id = tablaVentas->item(i, 0)->text().toLower();
        QString cajero = tablaVentas->item(i, 2)->text().toLower();
        QDateTime fechaVenta = listaVentas[i].fechaHora;
        QDate fechaSolo = fechaVenta.date();

        bool coincideTexto = texto.isEmpty() || id.contains(texto) || cajero.contains(texto);
        bool coincideFecha = true;

        if (opcionFecha == "Hoy") {
            coincideFecha = (fechaSolo == hoy);
        } else if (opcionFecha == "Últimos 7 Días") {
            coincideFecha = (fechaSolo >= hoy.addDays(-7) && fechaSolo <= hoy);
        } else if (opcionFecha == "Este Mes") {
            coincideFecha = (fechaSolo.year() == hoy.year() && fechaSolo.month() == hoy.month());
        } else if (opcionFecha == "Rango Personalizado") {
            coincideFecha = (fechaSolo >= dateInicio->date() && fechaSolo <= dateFin->date());
        }

        bool mostrar = coincideTexto && coincideFecha;
        tablaVentas->setRowHidden(i, !mostrar);

        if (mostrar) {
            acumuladoVentasVisibles += listaVentas[i].total;
            acumuladoIVAVisibles += listaVentas[i].iva;
            contadorVisibles++;
        }
    }

    lblTotalVendido->setText(QString("₡ %1").arg(acumuladoVentasVisibles, 0, 'f', 2));
    lblTotalIVA->setText(QString("₡ %1").arg(acumuladoIVAVisibles, 0, 'f', 2));
    lblCantidadVentas->setText(QString::number(contadorVisibles));
}

void VistaHistorial::mostrarTicketSeleccionado() {
    int fila = tablaVentas->currentRow();
    if (fila < 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Selección Requerida");
        msgBox.setText("Seleccione una venta de la tabla para visualizar su ticket.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QString idVenta = tablaVentas->item(fila, 0)->text();

    for (const auto& v : listaVentas) {
        if (v.idVenta == idVenta) {
            GeneradorTicketHTML generador;
            QString contenidoHTML = generador.generarContenido(v);

            QDialog dlgTicket(this);
            dlgTicket.setWindowTitle(QString("Ticket Venta %1").arg(idVenta));
            dlgTicket.setFixedSize(360, 540);
            dlgTicket.setStyleSheet("background-color: #ffffff;");

            QVBoxLayout* layoutDlg = new QVBoxLayout(&dlgTicket);
            layoutDlg->setContentsMargins(15, 15, 15, 15);

            QTextBrowser* textViewer = new QTextBrowser(&dlgTicket);
            textViewer->setHtml(contenidoHTML);
            textViewer->setStyleSheet("QTextBrowser { border: 1px solid #e5e7eb; border-radius: 8px; background-color: #ffffff; color: #111827; padding: 10px; }");

            QPushButton* btnCerrar = new QPushButton("Cerrar Visualización", &dlgTicket);
            btnCerrar->setStyleSheet("QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border: none; border-radius: 8px; padding: 10px; font-size: 13px; }"
                                     "QPushButton:hover { background-color: #059669; }");
            connect(btnCerrar, &QPushButton::clicked, &dlgTicket, &QDialog::accept);

            layoutDlg->addWidget(textViewer);
            layoutDlg->addSpacing(10);
            layoutDlg->addWidget(btnCerrar);

            dlgTicket.exec();
            break;
        }
    }
}

void VistaHistorial::exportarCSV() {
    if (listaVentas.empty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Sin datos");
        msgBox.setText("No hay ventas registradas para exportar.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QString rutaArchivo = QFileDialog::getSaveFileName(this, "Guardar Reporte CSV", "Reporte_Ventas.csv", "Archivos CSV (*.csv)");
    if (rutaArchivo.isEmpty()) return;

    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Error de Archivo");
        msgBox.setText("No se pudo crear el archivo de reporte.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #ef4444; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QTextStream stream(&archivo);
    stream << "ID Venta,Fecha y Hora,Cajero,Subtotal,IVA,Total\n";

    for (int i = 0; i < tablaVentas->rowCount(); ++i) {
        if (!tablaVentas->isRowHidden(i)) {
            const auto& v = listaVentas[i];
            stream << QString("\"%1\",\"%2\",\"%3\",%4,%5,%6\n")
                      .arg(v.idVenta)
                      .arg(v.fechaHora.toString("yyyy-MM-dd hh:mm AP"))
                      .arg(v.cajero)
                      .arg(v.subtotal, 0, 'f', 2)
                      .arg(v.iva, 0, 'f', 2)
                      .arg(v.total, 0, 'f', 2);
        }
    }

    archivo.close();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Reporte Creado");
    msgBox.setText("El reporte CSV se ha exportado exitosamente.");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
    msgBox.exec();
}

void VistaHistorial::anularVentaSeleccionada() {
    int fila = tablaVentas->currentRow();
    if (fila < 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Selección Requerida");
        msgBox.setText("Seleccione una venta para anular.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QString idVenta = tablaVentas->item(fila, 0)->text();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Confirmar Anulación");
    msgBox.setText(QString("¿Desea anular la venta '%1'? El stock correspondiente será devuelto al inventario.").arg(idVenta));
    msgBox.setIcon(QMessageBox::Question);
    QPushButton* btnSi = msgBox.addButton("Sí, Anular", QMessageBox::YesRole);
    msgBox.addButton("Cancelar", QMessageBox::NoRole);
    msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #ef4444; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 12px; font-size: 12px; }");
    msgBox.exec();

    if (msgBox.clickedButton() == btnSi) {
        GestorJSON gestor;
        auto prods = gestor.cargarProductos();

        // 1. Reintegrar stock de los items de la venta
        for (const auto& v : listaVentas) {
            if (v.idVenta == idVenta) {
                for (const auto& item : v.items) {
                    for (auto& p : prods) {
                        if (p.nombre == item.nombreProducto) {
                            p.stock += item.cantidad;
                            break;
                        }
                    }
                }
                break;
            }
        }
        gestor.guardarProductos(prods);

        // 2. Eliminar la venta de la lista y actualizar JSON de ventas
        for (auto it = listaVentas.begin(); it != listaVentas.end(); ++it) {
            if (it->idVenta == idVenta) {
                listaVentas.erase(it);
                break;
            }
        }
        gestor.guardarVentas(listaVentas);

        renderizarTabla();

        QMessageBox msgExito(this);
        msgExito.setWindowTitle("Venta Anulada");
        msgExito.setText("La venta se ha anulado y las unidades han sido devueltas al inventario.");
        msgExito.setIcon(QMessageBox::Information);
        msgExito.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgExito.exec();
    }
}
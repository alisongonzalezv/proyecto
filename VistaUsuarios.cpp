#include "VistaUsuarios.h"
#include <QShowEvent>

VistaUsuarios::VistaUsuarios(QWidget* parent) : QWidget(parent) {
    configurarInterfaz();
    cargarDatos();
}

void VistaUsuarios::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizar();
}

void VistaUsuarios::actualizar() {
    cargarDatos();
}

void VistaUsuarios::configurarInterfaz() {
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);
    layoutPrincipal->setSpacing(15);
    this->setStyleSheet("background-color: #f8f9fa;");

    // BARRA SUPERIOR
    QHBoxLayout* layoutTop = new QHBoxLayout();
    layoutTop->setSpacing(10);

    txtBuscar = new QLineEdit(this);
    txtBuscar->setPlaceholderText("Buscar por Nombre de Usuario...");
    txtBuscar->setStyleSheet(
        "QLineEdit { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 8px; "
        "padding: 8px 12px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 1px solid #10b981; }"
    );
    connect(txtBuscar, &QLineEdit::textChanged, this, &VistaUsuarios::filtrarTabla);

    btnAgregar = new QPushButton("+ Nuevo Usuario", this);
    btnAgregar->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 16px; font-size: 13px; }"
        "QPushButton:hover { background-color: #059669; }"
    );
    connect(btnAgregar, &QPushButton::clicked, [this]() { abrirFormularioUsuario(nullptr); });

    btnEditar = new QPushButton(" Editar", this);
    btnEditar->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; padding: 8px 16px; font-size: 13px; }"
        "QPushButton:hover { background-color: #2563eb; }"
    );
    connect(btnEditar, &QPushButton::clicked, [this]() {
        int fila = tablaUsuarios->currentRow();
        if (fila < 0) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Selección requerida");
            msgBox.setText("Seleccione un usuario para editar.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
            msgBox.exec();
            return;
        }
        QString username = tablaUsuarios->item(fila, 0)->text();
        for (auto& u : listaUsuarios) {
            if (u->getNombreUsuario() == username) {
                abrirFormularioUsuario(u);
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
    connect(btnEliminar, &QPushButton::clicked, this, &VistaUsuarios::eliminarUsuarioSeleccionado);

    layoutTop->addWidget(txtBuscar, 4);
    layoutTop->addWidget(btnAgregar, 2);
    layoutTop->addWidget(btnEditar, 1);
    layoutTop->addWidget(btnEliminar, 1);

    layoutPrincipal->addLayout(layoutTop);

    // TABLA DE USUARIOS
    tablaUsuarios = new QTableWidget(this);
    tablaUsuarios->setColumnCount(3);
    tablaUsuarios->setHorizontalHeaderLabels({"Usuario", "Rol de Acceso", "Estado"});
    tablaUsuarios->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaUsuarios->verticalHeader()->setVisible(false);
    tablaUsuarios->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaUsuarios->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaUsuarios->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaUsuarios->setStyleSheet(
        "QTableWidget { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 12px; gridline-color: #f3f4f6; }"
        "QHeaderView::section { background-color: #f9fafb; color: #374151; font-weight: bold; border: none; padding: 10px; font-size: 12px; }"
        "QTableWidget::item { padding: 10px 8px; font-size: 13px; color: #111827; }"
        "QTableWidget::item:selected { background-color: #e0f2fe; color: #0369a1; }"
    );

    layoutPrincipal->addWidget(tablaUsuarios);
}

void VistaUsuarios::cargarDatos() {
    GestorJSON gestor;
    listaUsuarios = gestor.cargarUsuarios();

    if (listaUsuarios.empty()) {
        listaUsuarios.push_back(std::make_shared<Administrador>("admin", "admin123"));
        listaUsuarios.push_back(std::make_shared<Cajero>("cajero1", "cajero123"));
        gestor.guardarUsuarios(listaUsuarios);
    }

    renderizarTabla();
}

void VistaUsuarios::renderizarTabla() {
    tablaUsuarios->setRowCount(0);

    for (const auto& usr : listaUsuarios) {
        int fila = tablaUsuarios->rowCount();
        tablaUsuarios->insertRow(fila);

        QTableWidgetItem* itemNombre = new QTableWidgetItem(usr->getNombreUsuario());
        QTableWidgetItem* itemRol = new QTableWidgetItem(usr->rol());
        QTableWidgetItem* itemEstado = new QTableWidgetItem("Activo");

        if (usr->rol() == "Administrador") {
            itemRol->setForeground(QBrush(QColor("#3b82f6")));
        } else {
            itemRol->setForeground(QBrush(QColor("#10b981")));
        }

        itemEstado->setForeground(QBrush(QColor("#10b981")));

        tablaUsuarios->setItem(fila, 0, itemNombre);
        tablaUsuarios->setItem(fila, 1, itemRol);
        tablaUsuarios->setItem(fila, 2, itemEstado);
    }
}

void VistaUsuarios::filtrarTabla() {
    QString texto = txtBuscar->text().trimmed().toLower();

    for (int i = 0; i < tablaUsuarios->rowCount(); ++i) {
        QString usr = tablaUsuarios->item(i, 0)->text().toLower();
        bool coincide = texto.isEmpty() || usr.contains(texto);
        tablaUsuarios->setRowHidden(i, !coincide);
    }
}

void VistaUsuarios::abrirFormularioUsuario(std::shared_ptr<Usuario> usuarioExistente) {
    bool esEdicion = (usuarioExistente != nullptr);

    QDialog dlg(this);
    dlg.setWindowTitle(esEdicion ? "Editar Usuario" : "Nuevo Usuario");
    dlg.setFixedSize(360, 320);
    dlg.setStyleSheet("QDialog { background-color: #ffffff; } QLabel { color: #111827; font-size: 12px; font-weight: 500; }");

    QVBoxLayout* layoutDlg = new QVBoxLayout(&dlg);
    layoutDlg->setContentsMargins(20, 20, 20, 20);
    layoutDlg->setSpacing(10);

    QLabel* lblTitulo = new QLabel(esEdicion ? "Modificar Credenciales" : "Registrar Nuevo Usuario", &dlg);
    lblTitulo->setStyleSheet("font-size: 16px; font-weight: bold; color: #111827; margin-bottom: 5px;");
    layoutDlg->addWidget(lblTitulo);

    QLineEdit* inputUser = new QLineEdit(&dlg);
    inputUser->setPlaceholderText("Nombre de usuario");
    if (esEdicion) {
        inputUser->setText(usuarioExistente->getNombreUsuario());
        inputUser->setEnabled(false);
    }

    QLineEdit* inputPass = new QLineEdit(&dlg);
    inputPass->setPlaceholderText("Contraseña");
    inputPass->setEchoMode(QLineEdit::Password);

    // Acción integrada dentro del mismo QLineEdit para conmutar visibilidad
    QAction* actionVerPass = inputPass->addAction(QIcon(), QLineEdit::TrailingPosition);
    actionVerPass->setText("👁");

    connect(actionVerPass, &QAction::triggered, [inputPass, actionVerPass]() {
        if (inputPass->echoMode() == QLineEdit::Password) {
            inputPass->setEchoMode(QLineEdit::Normal);
            actionVerPass->setText("🔒");
        } else {
            inputPass->setEchoMode(QLineEdit::Password);
            actionVerPass->setText("👁");
        }
    });

    if (esEdicion) {
        inputPass->setText(usuarioExistente->getClave());
    }

    QComboBox* comboRol = new QComboBox(&dlg);
    comboRol->addItems({"Cajero", "Administrador"});
    if (esEdicion) comboRol->setCurrentText(usuarioExistente->rol());

    QString estiloCampos =
        "QLineEdit, QComboBox { "
        "background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 6px; "
        "padding: 6px 10px; font-size: 13px; color: #111827; min-height: 25px; } "
        "QLineEdit:focus, QComboBox:focus { border: 1px solid #10b981; }";

    inputUser->setStyleSheet(estiloCampos);
    inputPass->setStyleSheet(estiloCampos);
    comboRol->setStyleSheet(estiloCampos);

    layoutDlg->addWidget(new QLabel("Usuario:", &dlg));
    layoutDlg->addWidget(inputUser);
    layoutDlg->addWidget(new QLabel("Contraseña:", &dlg));
    layoutDlg->addWidget(inputPass);
    layoutDlg->addWidget(new QLabel("Rol de Acceso:", &dlg));
    layoutDlg->addWidget(comboRol);

    QPushButton* btnGuardar = new QPushButton("Guardar Usuario", &dlg);
    btnGuardar->setMinimumHeight(38);
    btnGuardar->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border: none; border-radius: 8px; font-size: 13px; }"
        "QPushButton:hover { background-color: #059669; }"
    );

    connect(btnGuardar, &QPushButton::clicked, [&]() {
        if (inputUser->text().trimmed().isEmpty() || inputPass->text().trimmed().isEmpty()) {
            QMessageBox msgBox(&dlg);
            msgBox.setWindowTitle("Campos Vacíos");
            msgBox.setText("Todos los campos son obligatorios.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
            msgBox.exec();
            return;
        }

        QString username = inputUser->text().trimmed();
        QString pass = inputPass->text().trimmed();
        QString rol = comboRol->currentText();

        GestorJSON gestor;

        if (esEdicion) {
            for (auto& u : listaUsuarios) {
                if (u->getNombreUsuario() == username) {
                    if (rol == "Administrador") {
                        u = std::make_shared<Administrador>(username, pass);
                    } else {
                        u = std::make_shared<Cajero>(username, pass);
                    }
                    break;
                }
            }
        } else {
            for (const auto& u : listaUsuarios) {
                if (u->getNombreUsuario() == username) {
                    QMessageBox msgBox(&dlg);
                    msgBox.setWindowTitle("Usuario Existente");
                    msgBox.setText("El nombre de usuario ya está registrado.");
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
                    msgBox.exec();
                    return;
                }
            }

            if (rol == "Administrador") {
                listaUsuarios.push_back(std::make_shared<Administrador>(username, pass));
            } else {
                listaUsuarios.push_back(std::make_shared<Cajero>(username, pass));
            }
        }

        gestor.guardarUsuarios(listaUsuarios);
        renderizarTabla();

        QMessageBox msgBox(&dlg);
        msgBox.setWindowTitle("Éxito");
        msgBox.setText("Los cambios de usuarios se guardaron correctamente.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();

        dlg.accept();
    });

    layoutDlg->addSpacing(10);
    layoutDlg->addWidget(btnGuardar);

    dlg.exec();
}

void VistaUsuarios::eliminarUsuarioSeleccionado() {
    int fila = tablaUsuarios->currentRow();
    if (fila < 0) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Selección Requerida");
        msgBox.setText("Seleccione un usuario para eliminar.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QString username = tablaUsuarios->item(fila, 0)->text();

    if (username == "admin") {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Acción Protegida");
        msgBox.setText("No se puede eliminar el usuario 'admin' principal del sistema.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; border: none; }");
        msgBox.exec();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Confirmar Eliminación");
    msgBox.setText(QString("¿Está seguro de que desea eliminar al usuario '%1'?").arg(username));
    msgBox.setIcon(QMessageBox::Question);
    QPushButton* btnSi = msgBox.addButton("Sí, Eliminar", QMessageBox::YesRole);
    msgBox.addButton("Cancelar", QMessageBox::NoRole);
    msgBox.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #ef4444; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 12px; font-size: 12px; }");
    msgBox.exec();

    if (msgBox.clickedButton() == btnSi) {
        for (auto it = listaUsuarios.begin(); it != listaUsuarios.end(); ++it) {
            if ((*it)->getNombreUsuario() == username) {
                listaUsuarios.erase(it);
                break;
            }
        }

        GestorJSON gestor;
        gestor.guardarUsuarios(listaUsuarios);
        renderizarTabla();
    }
}
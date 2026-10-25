#include "VentanaLogin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QFile>
#include "GestorJSON.h"

VentanaLogin::VentanaLogin(QWidget* parent) : QDialog(parent) {
    setWindowTitle("UNA Pulpe - Iniciar Sesión");
    setFixedSize(380, 460);
    setStyleSheet("QDialog { background-color: #ffffff; }");

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(30, 25, 30, 25);
    layoutPrincipal->setSpacing(14);

    // ==========================================
    // CARGA Y DESPLIEGUE DE LA IMAGEN LOGO.JPEG
    // ==========================================
    QLabel* lblLogoImagen = new QLabel(this);
    lblLogoImagen->setAlignment(Qt::AlignCenter);

    QString rutaLogo = "logo.jpeg";
    if (!QFile::exists(rutaLogo)) {
        rutaLogo = "../logo.jpeg";
    }

    QPixmap pixmapLogo(rutaLogo);

    if (!pixmapLogo.isNull()) {
        lblLogoImagen->setPixmap(pixmapLogo.scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblLogoImagen->setText("UNA Pulpe");
        lblLogoImagen->setStyleSheet("font-size: 26px; font-weight: bold; color: #10b981;");
    }

    layoutPrincipal->addWidget(lblLogoImagen);
    layoutPrincipal->addSpacing(10);

    // CAMPOS DE ENTRADA
    txtUsuario = new QLineEdit(this);
    txtUsuario->setPlaceholderText("Nombre de usuario");

    QHBoxLayout* layoutPass = new QHBoxLayout();
    layoutPass->setSpacing(5);

    txtClave = new QLineEdit(this);
    txtClave->setPlaceholderText("Contraseña");
    txtClave->setEchoMode(QLineEdit::Password);

    QPushButton* btnVerPass = new QPushButton("Ver", this);
    btnVerPass->setFixedWidth(65);
    btnVerPass->setCursor(Qt::PointingHandCursor);
    btnVerPass->setStyleSheet(
        "QPushButton { background-color: #f3f4f6; color: #374151; font-weight: bold; "
        "border: 1px solid #d1d5db; border-radius: 6px; padding: 6px; font-size: 11px; }"
        "QPushButton:hover { background-color: #e5e7eb; }"
    );

    connect(btnVerPass, &QPushButton::clicked, [this, btnVerPass]() {
        if (txtClave->echoMode() == QLineEdit::Password) {
            txtClave->setEchoMode(QLineEdit::Normal);
            btnVerPass->setText("Ocultar");
        } else {
            txtClave->setEchoMode(QLineEdit::Password);
            btnVerPass->setText("Ver");
        }
    });

    layoutPass->addWidget(txtClave, 1);
    layoutPass->addWidget(btnVerPass);

    QString estiloEntradas =
        "QLineEdit { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 8px; "
        "padding: 10px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 1px solid #10b981; }";

    txtUsuario->setStyleSheet(estiloEntradas);
    txtClave->setStyleSheet(estiloEntradas);

    QLabel* lblUser = new QLabel("Usuario:", this);
    lblUser->setStyleSheet("color: #374151; font-size: 12px; font-weight: bold;");

    QLabel* lblPass = new QLabel("Contraseña:", this);
    lblPass->setStyleSheet("color: #374151; font-size: 12px; font-weight: bold;");

    layoutPrincipal->addWidget(lblUser);
    layoutPrincipal->addWidget(txtUsuario);
    layoutPrincipal->addWidget(lblPass);
    layoutPrincipal->addLayout(layoutPass);

    layoutPrincipal->addSpacing(15);

    // BOTÓN INICIAR SESIÓN
    btnIngresar = new QPushButton("Iniciar Sesión", this);
    btnIngresar->setMinimumHeight(44);
    btnIngresar->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
        "border: none; border-radius: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: #059669; }"
    );
    connect(btnIngresar, &QPushButton::clicked, this, &VentanaLogin::intentarLogin);

    layoutPrincipal->addWidget(btnIngresar);
}

void VentanaLogin::intentarLogin() {
    QString user = txtUsuario->text().trimmed();
    QString pass = txtClave->text().trimmed();

    QString estiloAlertas =
        "QMessageBox { background-color: #ffffff; }"
        "QLabel { color: #111827; font-size: 13px; font-weight: 500; }"
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
        "border-radius: 6px; padding: 6px 15px; border: none; font-size: 12px; }";

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Error de Validación");
        msg.setText("Por favor ingrese su usuario y contraseña.");
        msg.setIcon(QMessageBox::Warning);
        msg.setStyleSheet(estiloAlertas);
        msg.exec();
        return;
    }

    GestorJSON gestor;
    auto usuarios = gestor.cargarUsuarios();

    // Si no existen usuarios en usuarios.json, inicializa los por defecto
    if (usuarios.empty()) {
        usuarios.push_back(std::make_shared<Administrador>("admin", "admin123"));
        usuarios.push_back(std::make_shared<Cajero>("cajero1", "cajero123"));
        gestor.guardarUsuarios(usuarios);
    }

    for (const auto& u : usuarios) {
        if (u->getNombreUsuario() == user && u->getClave() == pass) {
            usuarioAutenticado = u;
            accept();
            return;
        }
    }

    QMessageBox msg(this);
    msg.setWindowTitle("Acceso Denegado");
    msg.setText("Usuario o contraseña incorrectos. Verifique sus credenciales.");
    msg.setIcon(QMessageBox::Critical);
    msg.setStyleSheet(
        "QMessageBox { background-color: #ffffff; }"
        "QLabel { color: #111827; font-size: 13px; font-weight: 500; }"
        "QPushButton { background-color: #ef4444; color: #ffffff; font-weight: bold; "
        "border-radius: 6px; padding: 6px 15px; border: none; font-size: 12px; }"
    );
    msg.exec();
}

std::shared_ptr<Usuario> VentanaLogin::getUsuarioAutenticado() const {
    return usuarioAutenticado;
}
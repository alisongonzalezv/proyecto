#include "GestorJSON.h"
#include "IGeneradorTicket.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>

namespace {
QString rutaDatos(const QString& nombreArchivo) {
    QString directorio = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (directorio.isEmpty()) {
        directorio = QDir::home().filePath(".una-pulpe");
    }
    QDir().mkpath(directorio);

    const QString destino = QDir(directorio).filePath(nombreArchivo);
    if (!QFile::exists(destino)) {
        const QStringList candidatos = {
            QDir::current().filePath(nombreArchivo),
            QDir(QCoreApplication::applicationDirPath()).filePath(nombreArchivo)
        };
        for (const QString& origen : candidatos) {
            if (QFileInfo(origen).absoluteFilePath() == QFileInfo(destino).absoluteFilePath()) {
                continue;
            }
            if (QFile::exists(origen) && QFile::copy(origen, destino)) {
                break;
            }
        }
    }
    return destino;
}

bool guardarJsonAtomico(const QString& ruta, const QJsonDocument& documento) {
    const QFileInfo info(ruta);
    if (!QDir().mkpath(info.absolutePath())) {
        return false;
    }

    QSaveFile archivo(ruta);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    const QByteArray contenido = documento.toJson();
    if (archivo.write(contenido) != contenido.size()) {
        archivo.cancelWriting();
        return false;
    }
    return archivo.commit();
}
}

GestorJSON::GestorJSON(const QString& rutaProds, const QString& rutaVnts)
    : rutaProductos(rutaProds.isEmpty() ? rutaDatos("productos.json") : rutaProds),
      rutaVentas(rutaVnts.isEmpty() ? rutaDatos("ventas.json") : rutaVnts) {}

std::vector<std::shared_ptr<Usuario>> GestorJSON::cargarUsuarios() {
    std::vector<std::shared_ptr<Usuario>> lista;
    QFile archivo(rutaDatos("usuarios.json"));

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lista;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(datos);
    if (!doc.isArray()) return lista;

    QJsonArray array = doc.array();
    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        QString user = obj["usuario"].toString();
        QString pass = obj["clave"].toString();
        QString rol = obj["rol"].toString();

        if (rol == "Administrador") {
            lista.push_back(std::make_shared<Administrador>(user, pass));
        } else {
            lista.push_back(std::make_shared<Cajero>(user, pass));
        }
    }

    return lista;
}

bool GestorJSON::guardarUsuarios(const std::vector<std::shared_ptr<Usuario>>& usuarios) {
    QJsonArray array;
    for (const auto& u : usuarios) {
        QJsonObject obj;
        obj["usuario"] = u->getNombreUsuario();
        obj["clave"] = u->getClave();
        obj["rol"] = u->rol();
        array.append(obj);
    }

    QJsonDocument doc(array);
    return guardarJsonAtomico(rutaDatos("usuarios.json"), doc);
}

std::vector<ProductoItem> GestorJSON::cargarProductos() {
    std::vector<ProductoItem> lista;
    QFile archivo(rutaProductos);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lista;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(datos);
    if (!doc.isArray()) return lista;

    QJsonArray array = doc.array();
    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        ProductoItem p;
        p.id = obj["id"].toString();
        p.nombre = obj["nombre"].toString();
        p.precio = obj["precio"].toDouble();
        p.descripcion = obj["descripcion"].toString();
        p.categoria = obj["categoria"].toString();
        p.stock = obj["stock"].toInt();
        lista.push_back(p);
    }

    return lista;
}

bool GestorJSON::guardarProductos(const std::vector<ProductoItem>& productos) {
    QJsonArray array;
    for (const auto& p : productos) {
        QJsonObject obj;
        obj["id"] = p.id;
        obj["nombre"] = p.nombre;
        obj["precio"] = p.precio;
        obj["descripcion"] = p.descripcion;
        obj["categoria"] = p.categoria;
        obj["stock"] = p.stock;
        array.append(obj);
    }

    QJsonDocument doc(array);
    return guardarJsonAtomico(rutaProductos, doc);
}

std::vector<DatosVentaTicket> GestorJSON::cargarVentas() {
    std::vector<DatosVentaTicket> lista;
    QFile archivo(rutaVentas);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lista;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(datos);
    if (!doc.isArray()) return lista;

    QJsonArray array = doc.array();
    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        DatosVentaTicket v;
        v.idVenta = obj["idVenta"].toString();
        v.cajero = obj["cajero"].toString();
        v.fechaHora = QDateTime::fromString(obj["fechaHora"].toString(), Qt::ISODate);
        v.subtotal = obj["subtotal"].toDouble();
        v.iva = obj["iva"].toDouble();
        v.total = obj["total"].toDouble();
        v.pagaCon = obj["pagaCon"].toDouble();
        v.cambio = obj["cambio"].toDouble();

        QJsonArray arrayItems = obj["items"].toArray();
        for (const QJsonValue& itemVal : arrayItems) {
            QJsonObject itemObj = itemVal.toObject();
            ItemTicket item;
            item.nombreProducto = itemObj["producto"].toString();
            item.cantidad = itemObj["cantidad"].toInt();
            item.precioUnitario = itemObj["precioUnitario"].toDouble();
            item.subtotal = itemObj["subtotal"].toDouble();
            v.items.push_back(item);
        }

        lista.push_back(v);
    }

    return lista;
}

bool GestorJSON::guardarVentas(const std::vector<DatosVentaTicket>& ventas) {
    QJsonArray arrayVentas;
    for (const auto& v : ventas) {
        QJsonObject objVenta;
        objVenta["idVenta"] = v.idVenta;
        objVenta["cajero"] = v.cajero;
        objVenta["fechaHora"] = v.fechaHora.toString(Qt::ISODate);
        objVenta["subtotal"] = v.subtotal;
        objVenta["iva"] = v.iva;
        objVenta["total"] = v.total;
        objVenta["pagaCon"] = v.pagaCon;
        objVenta["cambio"] = v.cambio;

        QJsonArray arrayItems;
        for (const auto& item : v.items) {
            QJsonObject itemObj;
            itemObj["producto"] = item.nombreProducto;
            itemObj["cantidad"] = item.cantidad;
            itemObj["precioUnitario"] = item.precioUnitario;
            itemObj["subtotal"] = item.subtotal;
            arrayItems.append(itemObj);
        }
        objVenta["items"] = arrayItems;

        arrayVentas.append(objVenta);
    }

    QJsonDocument doc(arrayVentas);
    return guardarJsonAtomico(rutaVentas, doc);
}

bool GestorJSON::registrarVenta(const DatosVentaTicket& venta) {
    std::vector<DatosVentaTicket> listaActual = cargarVentas();
    listaActual.push_back(venta);
    return guardarVentas(listaActual);
}

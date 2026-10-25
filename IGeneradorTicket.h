#ifndef IGENERADORTICKET_H
#define IGENERADORTICKET_H

#include <QString>
#include <QDateTime>
#include <vector>

struct ItemTicket {
    QString nombreProducto;
    int cantidad = 0;
    double precioUnitario = 0.0;
    double subtotal = 0.0;
};

struct DatosVentaTicket {
    QString idVenta;
    QString cajero;
    QDateTime fechaHora;
    std::vector<ItemTicket> items;
    double subtotal = 0.0;
    double iva = 0.0;
    double total = 0.0;
    double pagaCon = 0.0;
    double cambio = 0.0;
};

class IGeneradorTicket {
public:
    virtual ~IGeneradorTicket() = default;
    virtual QString generarContenido(const DatosVentaTicket& datos) = 0;
    virtual QString obtenerExtension() const = 0;
};

class GeneradorTicketTXT : public IGeneradorTicket {
public:
    QString obtenerExtension() const override { return ".txt"; }

    QString generarContenido(const DatosVentaTicket& datos) override {
        QString texto;
        texto += "========================================\n";
        texto += "              UNA PULPE                 \n";
        texto += "========================================\n";
        texto += "Ticket ID : " + datos.idVenta + "\n";
        texto += "Cajero    : " + datos.cajero + "\n";
        texto += "Fecha     : " + datos.fechaHora.toString("yyyy-MM-dd hh:mm AP") + "\n";
        texto += "----------------------------------------\n";
        texto += QString("%1 %2 %3\n").arg("Cant/Prod", -25).arg("P.Unit", 7).arg("Total", 8);
        texto += "----------------------------------------\n";

        for (const auto& item : datos.items) {
            QString lineaProd = QString("%1x %2").arg(item.cantidad).arg(item.nombreProducto);
            if (lineaProd.length() > 24) lineaProd = lineaProd.left(21) + "...";

            texto += QString("%1 ₡%2 ₡%3\n")
                     .arg(lineaProd, -25)
                     .arg(item.precioUnitario, 6, 'f', 0)
                     .arg(item.subtotal, 7, 'f', 0);
        }

        texto += "----------------------------------------\n";
        texto += QString("Subtotal : ₡%1\n").arg(datos.subtotal, 0, 'f', 2);
        texto += QString("IVA (13%): ₡%1\n").arg(datos.iva, 0, 'f', 2);
        texto += QString("TOTAL    : ₡%1\n").arg(datos.total, 0, 'f', 2);
        texto += "----------------------------------------\n";
        texto += QString("Paga con : ₡%1\n").arg(datos.pagaCon, 0, 'f', 2);
        texto += QString("Cambio   : ₡%1\n").arg(datos.cambio, 0, 'f', 2);
        texto += "========================================\n";
        texto += "        ¡Gracias por su compra!         \n";
        texto += "========================================\n";
        return texto;
    }
};

class GeneradorTicketHTML : public IGeneradorTicket {
public:
    QString obtenerExtension() const override { return ".html"; }

    QString generarContenido(const DatosVentaTicket& datos) override {
        QString html = "<html><body style='font-family: sans-serif; font-size: 12px; color: #111827;'>";
        html += "<h2 style='text-align: center; color: #10b981; margin-bottom: 5px;'>UNA Pulpe</h2>";
        html += "<p style='text-align: center; font-size: 11px; color: #6b7280; margin-top: 0;'>Punto de Venta Universitario</p>";
        html += "<hr style='border: none; border-top: 1px dashed #d1d5db;'>";
        html += "<p><b>Ticket #:</b> " + datos.idVenta + "<br>";
        html += "<b>Cajero:</b> " + datos.cajero + "<br>";
        html += "<b>Fecha:</b> " + datos.fechaHora.toString("yyyy-MM-dd hh:mm AP") + "</p>";
        html += "<hr style='border: none; border-top: 1px dashed #d1d5db;'>";
        html += "<table width='100%' style='border-collapse: collapse; font-size: 12px;'>";
        html += "<tr style='color: #6b7280;'><th align='left'>Cant/Prod</th><th align='right'>Total</th></tr>";

        for (const auto& item : datos.items) {
            html += QString("<tr><td style='padding: 3px 0;'>%1x %2</td><td align='right'>₡%3</td></tr>")
                    .arg(item.cantidad)
                    .arg(item.nombreProducto)
                    .arg(item.subtotal, 0, 'f', 2);
        }

        html += "</table><hr style='border: none; border-top: 1px dashed #d1d5db;'>";
        html += QString("<p align='right' style='margin: 3px 0;'>Subtotal: ₡%1</p>").arg(datos.subtotal, 0, 'f', 2);
        html += QString("<p align='right' style='margin: 3px 0;'>IVA (13%): ₡%1</p>").arg(datos.iva, 0, 'f', 2);
        html += QString("<p align='right' style='font-size: 14px; color: #10b981; margin: 5px 0;'><b>TOTAL: ₡%1</b></p>").arg(datos.total, 0, 'f', 2);
        html += "<hr style='border: none; border-top: 1px dashed #d1d5db;'>";
        html += QString("<p align='right' style='margin: 3px 0;'>Paga con: ₡%1</p>").arg(datos.pagaCon, 0, 'f', 2);
        html += QString("<p align='right' style='margin: 3px 0;'>Cambio: ₡%1</p>").arg(datos.cambio, 0, 'f', 2);
        html += "<p style='text-align: center; margin-top: 15px; color: #6b7280;'>¡Gracias por su compra!</p>";
        html += "</body></html>";
        return html;
    }
};

#endif // IGENERADORTICKET_H
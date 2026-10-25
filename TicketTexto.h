#ifndef IGENERADORTICKET_H
#define IGENERADORTICKET_H

#include "Venta.h"
#include <QString>

class IGeneradorTicket {
public:
    virtual ~IGeneradorTicket() = default;
    virtual QString generar(const Venta& venta) = 0;
};

class TicketTexto : public IGeneradorTicket {
public:
    QString generar(const Venta& venta) override {
        QString out;
        out += "========================================\n";
        out += "          LA PULPE - TICKET             \n";
        out += "========================================\n";
        out += "Folio: " + venta.getFolio() + "\n";
        out += "Fecha: " + venta.getFecha().toString("yyyy-MM-dd hh:mm") + "\n";
        out += "Cajero: " + venta.getNombreCajero() + "\n";
        out += "----------------------------------------\n";
        for (const auto& l : venta.getLineas()) {
            out += QString("%1x %2 -> CRC %3\n")
                       .arg(l.cantidad)
                       .arg(l.nombreProducto)
                       .arg(l.subtotal, 0, 'f', 2);
        }
        out += "----------------------------------------\n";
        out += QString("SUBTOTAL:  CRC %1\n").arg(venta.getSubtotalBruto(), 0, 'f', 2);
        out += QString("DESCUENTO: CRC %1\n").arg(venta.getMontoDescuento(), 0, 'f', 2);
        out += QString("IVA (13%): CRC %1\n").arg(venta.getMontoIVA(), 0, 'f', 2);
        out += QString("TOTAL:     CRC %1\n").arg(venta.getTotalNeto(), 0, 'f', 2);
        out += QString("EFECTIVO:  CRC %1\n").arg(venta.getEfectivoRecibido(), 0, 'f', 2);
        out += QString("CAMBIO:    CRC %1\n").arg(venta.getCambio(), 0, 'f', 2);
        out += "========================================\n";
        return out;
    }
};

#endif // IGENERADORTICKET_H
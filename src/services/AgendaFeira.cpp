#include "services/AgendaFeira.hpp"
#include <QTimeZone>

QDateTime AgendaFeira::agora()
{
    return QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("America/Recife")); // Fuso de Recife
}

// Checagem da feira aberta
bool AgendaFeira::aberta(const FeiraComprador &feira, const QDateTime &momento)
{
    QTime inicio = QTime::fromString(QString::fromStdString(feira.inicio), "HH:mm");
    QTime fim = QTime::fromString(QString::fromStdString(feira.fim), "HH:mm");
    return inicio.isValid() && fim.isValid() && momento.date().dayOfWeek() == feira.diaSemana
        && momento.time() >= inicio && momento.time() < fim;
}

// Checagem de dias abertos
QStringList AgendaFeira::datas(const FeiraComprador &feira, const QDateTime &momento)
{
    QStringList lista;
    if (feira.diaSemana < 1 || feira.diaSemana > 7) return lista;
    for (int i = 0; i <= 35 && lista.size() < 5; ++i) {
        QDate data = momento.date().addDays(i);
        if (data.dayOfWeek() != feira.diaSemana) continue;
        QString texto = data.toString(Qt::ISODate);
        if (!janelas(feira, texto, momento).isEmpty()) lista.append(texto);
    }
    return lista;
}

// Janelas de horários
QVector<AgendamentoReserva> AgendaFeira::janelas(const FeiraComprador &feira, const QString &dataTexto,
                                               const QDateTime &momento)
{
    QVector<AgendamentoReserva> lista;
    QDate data = QDate::fromString(dataTexto, Qt::ISODate);
    QTime inicio = QTime::fromString(QString::fromStdString(feira.inicio), "HH:mm");
    QTime fim = QTime::fromString(QString::fromStdString(feira.fim), "HH:mm");
    if (!data.isValid() || data < momento.date() || data > momento.date().addDays(35)
        || data.dayOfWeek() != feira.diaSemana || !inicio.isValid() || !fim.isValid() || inicio >= fim)
        return lista;
    for (QTime hora = inicio; hora < fim;) {
        QTime proxima = hora.addSecs(3600);
        if (proxima > fim || proxima < hora) proxima = fim;
        if (data > momento.date() || proxima > momento.time())
            lista.append({feira.id, dataTexto, hora.toString("HH:mm"), proxima.toString("HH:mm")});
        hora = proxima;
    }
    return lista;
}

// Validar janelas de agendamento
bool AgendaFeira::validar(const FeiraComprador &feira, const AgendamentoReserva &agendamento,
                         const QDateTime &momento)
{
    for (const AgendamentoReserva &janela : janelas(feira, agendamento.data, momento))
        if (janela.feiraId == agendamento.feiraId && janela.inicio == agendamento.inicio
            && janela.fim == agendamento.fim) return true;
    return false;
}

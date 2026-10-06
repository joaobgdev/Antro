#ifndef AGENDAFEIRA_HPP
#define AGENDAFEIRA_HPP

#include <QDateTime>
#include <QStringList>
#include <QVector>
#include "services/CatalogoComprador.hpp"
#include "services/RepositorioCatalogo.hpp"

class AgendaFeira {
public:
    static QDateTime agora();
    static bool aberta(const FeiraComprador &feira, const QDateTime &momento = agora());
    static QStringList datas(const FeiraComprador &feira, const QDateTime &momento = agora());
    static QVector<AgendamentoReserva> janelas(const FeiraComprador &feira, const QString &data,
                                              const QDateTime &momento = agora());
    static bool validar(const FeiraComprador &feira, const AgendamentoReserva &agendamento,
                        const QDateTime &momento = agora());
};

#endif

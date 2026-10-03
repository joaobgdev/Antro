#ifndef COMPRADORCONTROLLER_HPP
#define COMPRADORCONTROLLER_HPP

#include <QObject>
#include <QVariantList>
#include <QtQmlIntegration>
#include "services/CatalogoComprador.hpp"


class CompradorController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QVariantList sacola READ sacola NOTIFY sacolaChanged)
    Q_PROPERTY(int tiposNaSacola READ tiposNaSacola NOTIFY sacolaChanged)
    Q_PROPERTY(double totalEstimado READ totalEstimado NOTIFY sacolaChanged)
public:
    explicit CompradorController(QObject* parent = nullptr);
    Q_INVOKABLE QVariantList feiras() const;
    Q_INVOKABLE QVariantMap feira(int id) const;
    Q_INVOKABLE QVariantMap vendedor(int id) const;
    Q_INVOKABLE QVariantList vendedores(int feiraId) const;
    Q_INVOKABLE QVariantList produtos(int feiraId, int vendedorId) const;
    Q_INVOKABLE bool adicionar(int feiraId, int vendedorId, int produtoId, double quantidade);
    Q_INVOKABLE void remover(int indice);
    Q_INVOKABLE void limpar();
    QVariantList sacola() const;
    int tiposNaSacola() const;
    double totalEstimado() const;
signals:
    void sacolaChanged();
private:
    CatalogoComprador catalogo;
};
#endif

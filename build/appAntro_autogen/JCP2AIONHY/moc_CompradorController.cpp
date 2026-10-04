/****************************************************************************
** Meta object code from reading C++ file 'CompradorController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ui/CompradorController.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CompradorController.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN19CompradorControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto CompradorController::qt_create_metaobjectdata<qt_meta_tag_ZN19CompradorControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CompradorController",
        "QML.Element",
        "auto",
        "QML.Singleton",
        "true",
        "sacolaChanged",
        "",
        "feiras",
        "QVariantList",
        "feira",
        "QVariantMap",
        "id",
        "vendedor",
        "vendedores",
        "feiraId",
        "produtos",
        "vendedorId",
        "adicionar",
        "produtoId",
        "quantidade",
        "remover",
        "indice",
        "limpar",
        "sacola",
        "tiposNaSacola",
        "totalEstimado"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sacolaChanged'
        QtMocHelpers::SignalData<void()>(5, 6, QMC::AccessPublic, QMetaType::Void),
        // Method 'feiras'
        QtMocHelpers::MethodData<QVariantList() const>(7, 6, QMC::AccessPublic, 0x80000000 | 8),
        // Method 'feira'
        QtMocHelpers::MethodData<QVariantMap(int) const>(9, 6, QMC::AccessPublic, 0x80000000 | 10, {{
            { QMetaType::Int, 11 },
        }}),
        // Method 'vendedor'
        QtMocHelpers::MethodData<QVariantMap(int) const>(12, 6, QMC::AccessPublic, 0x80000000 | 10, {{
            { QMetaType::Int, 11 },
        }}),
        // Method 'vendedores'
        QtMocHelpers::MethodData<QVariantList(int) const>(13, 6, QMC::AccessPublic, 0x80000000 | 8, {{
            { QMetaType::Int, 14 },
        }}),
        // Method 'produtos'
        QtMocHelpers::MethodData<QVariantList(int, int) const>(15, 6, QMC::AccessPublic, 0x80000000 | 8, {{
            { QMetaType::Int, 14 }, { QMetaType::Int, 16 },
        }}),
        // Method 'adicionar'
        QtMocHelpers::MethodData<bool(int, int, int, double)>(17, 6, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 14 }, { QMetaType::Int, 16 }, { QMetaType::Int, 18 }, { QMetaType::Double, 19 },
        }}),
        // Method 'remover'
        QtMocHelpers::MethodData<void(int)>(20, 6, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Method 'limpar'
        QtMocHelpers::MethodData<void()>(22, 6, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'sacola'
        QtMocHelpers::PropertyData<QVariantList>(23, 0x80000000 | 8, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'tiposNaSacola'
        QtMocHelpers::PropertyData<int>(24, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'totalEstimado'
        QtMocHelpers::PropertyData<double>(25, QMetaType::Double, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
            {    3,    4 },
    });
    return QtMocHelpers::metaObjectData<CompradorController, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject CompradorController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19CompradorControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19CompradorControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19CompradorControllerE_t>.metaTypes,
    nullptr
} };

void CompradorController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CompradorController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sacolaChanged(); break;
        case 1: { QVariantList _r = _t->feiras();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 2: { QVariantMap _r = _t->feira((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 3: { QVariantMap _r = _t->vendedor((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 4: { QVariantList _r = _t->vendedores((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 5: { QVariantList _r = _t->produtos((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->adicionar((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->remover((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->limpar(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CompradorController::*)()>(_a, &CompradorController::sacolaChanged, 0))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QVariantList*>(_v) = _t->sacola(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->tiposNaSacola(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->totalEstimado(); break;
        default: break;
        }
    }
}

const QMetaObject *CompradorController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CompradorController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19CompradorControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CompradorController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CompradorController::sacolaChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP

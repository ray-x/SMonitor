/****************************************************************************
** Meta object code from reading C++ file 'CPasswdConfig.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../CPasswdConfig.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CPasswdConfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CPasswdConfig_t {
    QByteArrayData data[4];
    char stringdata[40];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CPasswdConfig_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CPasswdConfig_t qt_meta_stringdata_CPasswdConfig = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 12),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 11)
    },
    "CPasswdConfig\0acceptPasswd\0\0dataChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CPasswdConfig[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CPasswdConfig::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CPasswdConfig *_t = static_cast<CPasswdConfig *>(_o);
        switch (_id) {
        case 0: _t->acceptPasswd(); break;
        case 1: _t->dataChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CPasswdConfig::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CPasswdConfig.data,
      qt_meta_data_CPasswdConfig,  qt_static_metacall, 0, 0}
};


const QMetaObject *CPasswdConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CPasswdConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CPasswdConfig.stringdata))
        return static_cast<void*>(const_cast< CPasswdConfig*>(this));
    return QDialog::qt_metacast(_clname);
}

int CPasswdConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

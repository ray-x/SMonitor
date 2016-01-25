/****************************************************************************
** Meta object code from reading C++ file 'CLoginDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CLoginDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CLoginDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CLoginDialog_t {
    QByteArrayData data[12];
    char stringdata[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CLoginDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CLoginDialog_t qt_meta_stringdata_CLoginDialog = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 5),
QT_MOC_LITERAL(2, 19, 0),
QT_MOC_LITERAL(3, 20, 2),
QT_MOC_LITERAL(4, 23, 11),
QT_MOC_LITERAL(5, 35, 13),
QT_MOC_LITERAL(6, 49, 8),
QT_MOC_LITERAL(7, 58, 6),
QT_MOC_LITERAL(8, 65, 4),
QT_MOC_LITERAL(9, 70, 8),
QT_MOC_LITERAL(10, 79, 5),
QT_MOC_LITERAL(11, 85, 10)
    },
    "CLoginDialog\0login\0\0id\0clientLogin\0"
    "msgSendResult\0nMsghead\0result\0info\0"
    "msgError\0error\0msgLoginIn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CLoginDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   42,    2, 0x08 /* Private */,
       5,    3,   43,    2, 0x08 /* Private */,
       9,    2,   50,    2, 0x08 /* Private */,
      11,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::QString,    6,    7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    6,   10,
    QMetaType::Void,

       0        // eod
};

void CLoginDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CLoginDialog *_t = static_cast<CLoginDialog *>(_o);
        switch (_id) {
        case 0: _t->login((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->clientLogin(); break;
        case 2: _t->msgSendResult((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->msgError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->msgLoginIn(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CLoginDialog::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CLoginDialog::login)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject CLoginDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CLoginDialog.data,
      qt_meta_data_CLoginDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *CLoginDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CLoginDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CLoginDialog.stringdata))
        return static_cast<void*>(const_cast< CLoginDialog*>(this));
    if (!strcmp(_clname, "Ui::CLoginDialog"))
        return static_cast< Ui::CLoginDialog*>(const_cast< CLoginDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int CLoginDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CLoginDialog::login(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

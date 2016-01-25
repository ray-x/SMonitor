/****************************************************************************
** Meta object code from reading C++ file 'CSystemTrayIcon.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../CSystemTrayIcon.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CSystemTrayIcon.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CSystemTrayIcon_t {
    QByteArrayData data[11];
    char stringdata[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CSystemTrayIcon_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CSystemTrayIcon_t qt_meta_stringdata_CSystemTrayIcon = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 20),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 5),
QT_MOC_LITERAL(4, 44, 15),
QT_MOC_LITERAL(5, 60, 16),
QT_MOC_LITERAL(6, 77, 16),
QT_MOC_LITERAL(7, 94, 8),
QT_MOC_LITERAL(8, 103, 14),
QT_MOC_LITERAL(9, 118, 9),
QT_MOC_LITERAL(10, 128, 6)
    },
    "CSystemTrayIcon\0trayIconStateChanged\0"
    "\0state\0quitApplication\0aboutApplication\0"
    "hideOrShowWidget\0setState\0SMonitor::Type\0"
    "setHidden\0hidden"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CSystemTrayIcon[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    0,   48,    2, 0x06 /* Public */,
       6,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   50,    2, 0x0a /* Public */,
       9,    1,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    3,
    QMetaType::Void, QMetaType::Bool,   10,

       0        // eod
};

void CSystemTrayIcon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CSystemTrayIcon *_t = static_cast<CSystemTrayIcon *>(_o);
        switch (_id) {
        case 0: _t->trayIconStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->quitApplication(); break;
        case 2: _t->aboutApplication(); break;
        case 3: _t->hideOrShowWidget(); break;
        case 4: _t->setState((*reinterpret_cast< SMonitor::Type(*)>(_a[1]))); break;
        case 5: _t->setHidden((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CSystemTrayIcon::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSystemTrayIcon::trayIconStateChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (CSystemTrayIcon::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSystemTrayIcon::quitApplication)) {
                *result = 1;
            }
        }
        {
            typedef void (CSystemTrayIcon::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSystemTrayIcon::aboutApplication)) {
                *result = 2;
            }
        }
        {
            typedef void (CSystemTrayIcon::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSystemTrayIcon::hideOrShowWidget)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject CSystemTrayIcon::staticMetaObject = {
    { &QSystemTrayIcon::staticMetaObject, qt_meta_stringdata_CSystemTrayIcon.data,
      qt_meta_data_CSystemTrayIcon,  qt_static_metacall, 0, 0}
};


const QMetaObject *CSystemTrayIcon::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CSystemTrayIcon::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CSystemTrayIcon.stringdata))
        return static_cast<void*>(const_cast< CSystemTrayIcon*>(this));
    return QSystemTrayIcon::qt_metacast(_clname);
}

int CSystemTrayIcon::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSystemTrayIcon::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CSystemTrayIcon::trayIconStateChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CSystemTrayIcon::quitApplication()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CSystemTrayIcon::aboutApplication()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CSystemTrayIcon::hideOrShowWidget()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE

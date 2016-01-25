/****************************************************************************
** Meta object code from reading C++ file 'databasemanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../databasemanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'databasemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DatabaseManager_t {
    QByteArrayData data[13];
    char stringdata[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DatabaseManager_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DatabaseManager_t qt_meta_stringdata_DatabaseManager = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 21),
QT_MOC_LITERAL(2, 38, 0),
QT_MOC_LITERAL(3, 39, 22),
QT_MOC_LITERAL(4, 62, 3),
QT_MOC_LITERAL(5, 66, 3),
QT_MOC_LITERAL(6, 70, 23),
QT_MOC_LITERAL(7, 94, 20),
QT_MOC_LITERAL(8, 115, 4),
QT_MOC_LITERAL(9, 120, 7),
QT_MOC_LITERAL(10, 128, 10),
QT_MOC_LITERAL(11, 139, 12),
QT_MOC_LITERAL(12, 152, 12)
    },
    "DatabaseManager\0sig_updateInstallData\0"
    "\0QPair<QString,QString>\0key\0num\0"
    "sig_updateUnInstallData\0sig_updateAllNumData\0"
    "name\0version\0installNum\0unInstallNum\0"
    "sig_initData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DatabaseManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       6,    2,   39,    2, 0x06 /* Public */,
       7,    4,   44,    2, 0x06 /* Public */,
      12,    0,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int, QMetaType::Int,    8,    9,   10,   11,
    QMetaType::Void,

       0        // eod
};

void DatabaseManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DatabaseManager *_t = static_cast<DatabaseManager *>(_o);
        switch (_id) {
        case 0: _t->sig_updateInstallData((*reinterpret_cast< QPair<QString,QString>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->sig_updateUnInstallData((*reinterpret_cast< QPair<QString,QString>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->sig_updateAllNumData((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 3: _t->sig_initData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DatabaseManager::*_t)(QPair<QString,QString> , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DatabaseManager::sig_updateInstallData)) {
                *result = 0;
            }
        }
        {
            typedef void (DatabaseManager::*_t)(QPair<QString,QString> , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DatabaseManager::sig_updateUnInstallData)) {
                *result = 1;
            }
        }
        {
            typedef void (DatabaseManager::*_t)(const QString & , const QString & , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DatabaseManager::sig_updateAllNumData)) {
                *result = 2;
            }
        }
        {
            typedef void (DatabaseManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DatabaseManager::sig_initData)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject DatabaseManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DatabaseManager.data,
      qt_meta_data_DatabaseManager,  qt_static_metacall, 0, 0}
};


const QMetaObject *DatabaseManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DatabaseManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DatabaseManager.stringdata))
        return static_cast<void*>(const_cast< DatabaseManager*>(this));
    return QObject::qt_metacast(_clname);
}

int DatabaseManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DatabaseManager::sig_updateInstallData(QPair<QString,QString> _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DatabaseManager::sig_updateUnInstallData(QPair<QString,QString> _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DatabaseManager::sig_updateAllNumData(const QString & _t1, const QString & _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DatabaseManager::sig_initData()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE

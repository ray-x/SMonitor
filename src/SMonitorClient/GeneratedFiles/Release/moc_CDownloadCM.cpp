/****************************************************************************
** Meta object code from reading C++ file 'CDownloadCM.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CDownloadCM.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDownloadCM.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CDownloadCM_t {
    QByteArrayData data[15];
    char stringdata[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDownloadCM_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDownloadCM_t qt_meta_stringdata_CDownloadCM = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 16),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 3),
QT_MOC_LITERAL(4, 34, 20),
QT_MOC_LITERAL(5, 55, 3),
QT_MOC_LITERAL(6, 59, 15),
QT_MOC_LITERAL(7, 75, 14),
QT_MOC_LITERAL(8, 90, 10),
QT_MOC_LITERAL(9, 101, 12),
QT_MOC_LITERAL(10, 114, 14),
QT_MOC_LITERAL(11, 129, 14),
QT_MOC_LITERAL(12, 144, 18),
QT_MOC_LITERAL(13, 163, 15),
QT_MOC_LITERAL(14, 179, 13)
    },
    "CDownloadCM\0downloadFinished\0\0key\0"
    "downloadErrorOccured\0msg\0downloadPercent\0"
    "lDownloadBytes\0totalBytes\0downloadQuit\0"
    "downloadFinish\0downloadQuited\0"
    "downloadErrorOccur\0downloadProcess\0"
    "lDownLoadByte"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDownloadCM[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    2,   57,    2, 0x06 /* Public */,
       6,    3,   62,    2, 0x06 /* Public */,
       9,    1,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,
      12,    1,   74,    2, 0x08 /* Private */,
      13,    2,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Long, QMetaType::Int,    3,    7,    8,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Long, QMetaType::Int,   14,    8,

       0        // eod
};

void CDownloadCM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDownloadCM *_t = static_cast<CDownloadCM *>(_o);
        switch (_id) {
        case 0: _t->downloadFinished((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->downloadErrorOccured((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->downloadPercent((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->downloadQuit((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->downloadFinish(); break;
        case 5: _t->downloadQuited(); break;
        case 6: _t->downloadErrorOccur((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->downloadProcess((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CDownloadCM::*_t)(const QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CDownloadCM::downloadFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (CDownloadCM::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CDownloadCM::downloadErrorOccured)) {
                *result = 1;
            }
        }
        {
            typedef void (CDownloadCM::*_t)(const QString & , long , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CDownloadCM::downloadPercent)) {
                *result = 2;
            }
        }
        {
            typedef void (CDownloadCM::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CDownloadCM::downloadQuit)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject CDownloadCM::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CDownloadCM.data,
      qt_meta_data_CDownloadCM,  qt_static_metacall, 0, 0}
};


const QMetaObject *CDownloadCM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDownloadCM::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDownloadCM.stringdata))
        return static_cast<void*>(const_cast< CDownloadCM*>(this));
    return QObject::qt_metacast(_clname);
}

int CDownloadCM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void CDownloadCM::downloadFinished(const QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CDownloadCM::downloadErrorOccured(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CDownloadCM::downloadPercent(const QString & _t1, long _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CDownloadCM::downloadQuit(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE

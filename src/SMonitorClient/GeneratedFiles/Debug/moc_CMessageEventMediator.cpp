/****************************************************************************
** Meta object code from reading C++ file 'CMessageEventMediator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CMessageEventMediator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CMessageEventMediator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CMessageEventMediator_t {
    QByteArrayData data[21];
    char stringdata[286];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CMessageEventMediator_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CMessageEventMediator_t qt_meta_stringdata_CMessageEventMediator = {
    {
QT_MOC_LITERAL(0, 0, 21),
QT_MOC_LITERAL(1, 22, 10),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 8),
QT_MOC_LITERAL(4, 43, 8),
QT_MOC_LITERAL(5, 52, 8),
QT_MOC_LITERAL(6, 61, 9),
QT_MOC_LITERAL(7, 71, 16),
QT_MOC_LITERAL(8, 88, 18),
QT_MOC_LITERAL(9, 107, 19),
QT_MOC_LITERAL(10, 127, 18),
QT_MOC_LITERAL(11, 146, 16),
QT_MOC_LITERAL(12, 163, 26),
QT_MOC_LITERAL(13, 190, 4),
QT_MOC_LITERAL(14, 195, 7),
QT_MOC_LITERAL(15, 203, 20),
QT_MOC_LITERAL(16, 224, 9),
QT_MOC_LITERAL(17, 234, 22),
QT_MOC_LITERAL(18, 257, 3),
QT_MOC_LITERAL(19, 261, 4),
QT_MOC_LITERAL(20, 266, 19)
    },
    "CMessageEventMediator\0sigLoginIn\0\0"
    "sigError\0nMsgHead\0strError\0sigGetXml\0"
    "strUpdateXmlPath\0strHelpInfoXmlPath\0"
    "strVersionCheckFile\0sigRegisterSuccess\0"
    "sigModifySuccess\0sigServerHasGetSoftInstall\0"
    "name\0version\0sigGetHelpInfoSucess\0"
    "StringMap\0sigServerDistributeMsg\0msg\0"
    "time\0sigGetDistributeMsg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMessageEventMediator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    2,   60,    2, 0x06 /* Public */,
       6,    3,   65,    2, 0x06 /* Public */,
      10,    0,   72,    2, 0x06 /* Public */,
      11,    0,   73,    2, 0x06 /* Public */,
      12,    2,   74,    2, 0x06 /* Public */,
      15,    1,   79,    2, 0x06 /* Public */,
      17,    2,   82,    2, 0x06 /* Public */,
      20,    2,   87,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    7,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   13,   14,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   18,   19,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   18,   19,

       0        // eod
};

void CMessageEventMediator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CMessageEventMediator *_t = static_cast<CMessageEventMediator *>(_o);
        switch (_id) {
        case 0: _t->sigLoginIn(); break;
        case 1: _t->sigError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->sigGetXml((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->sigRegisterSuccess(); break;
        case 4: _t->sigModifySuccess(); break;
        case 5: _t->sigServerHasGetSoftInstall((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->sigGetHelpInfoSucess((*reinterpret_cast< const StringMap(*)>(_a[1]))); break;
        case 7: _t->sigServerDistributeMsg((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 8: _t->sigGetDistributeMsg((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CMessageEventMediator::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigLoginIn)) {
                *result = 0;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)(int , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigError)) {
                *result = 1;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)(const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigGetXml)) {
                *result = 2;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigRegisterSuccess)) {
                *result = 3;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigModifySuccess)) {
                *result = 4;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigServerHasGetSoftInstall)) {
                *result = 5;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)(const StringMap & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigGetHelpInfoSucess)) {
                *result = 6;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigServerDistributeMsg)) {
                *result = 7;
            }
        }
        {
            typedef void (CMessageEventMediator::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMessageEventMediator::sigGetDistributeMsg)) {
                *result = 8;
            }
        }
    }
}

const QMetaObject CMessageEventMediator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CMessageEventMediator.data,
      qt_meta_data_CMessageEventMediator,  qt_static_metacall, 0, 0}
};


const QMetaObject *CMessageEventMediator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CMessageEventMediator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CMessageEventMediator.stringdata))
        return static_cast<void*>(const_cast< CMessageEventMediator*>(this));
    return QObject::qt_metacast(_clname);
}

int CMessageEventMediator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CMessageEventMediator::sigLoginIn()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CMessageEventMediator::sigError(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CMessageEventMediator::sigGetXml(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CMessageEventMediator::sigRegisterSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void CMessageEventMediator::sigModifySuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void CMessageEventMediator::sigServerHasGetSoftInstall(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CMessageEventMediator::sigGetHelpInfoSucess(const StringMap & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CMessageEventMediator::sigServerDistributeMsg(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void CMessageEventMediator::sigGetDistributeMsg(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE

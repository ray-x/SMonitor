/****************************************************************************
** Meta object code from reading C++ file 'CSoftItemWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CSoftItemWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CSoftItemWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CSoftItemWidget_t {
    QByteArrayData data[18];
    char stringdata[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CSoftItemWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CSoftItemWidget_t qt_meta_stringdata_CSoftItemWidget = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 15),
QT_MOC_LITERAL(2, 32, 0),
QT_MOC_LITERAL(3, 33, 14),
QT_MOC_LITERAL(4, 48, 4),
QT_MOC_LITERAL(5, 53, 8),
QT_MOC_LITERAL(6, 62, 4),
QT_MOC_LITERAL(7, 67, 14),
QT_MOC_LITERAL(8, 82, 3),
QT_MOC_LITERAL(9, 86, 8),
QT_MOC_LITERAL(10, 95, 7),
QT_MOC_LITERAL(11, 103, 7),
QT_MOC_LITERAL(12, 111, 11),
QT_MOC_LITERAL(13, 123, 8),
QT_MOC_LITERAL(14, 132, 12),
QT_MOC_LITERAL(15, 145, 15),
QT_MOC_LITERAL(16, 161, 17),
QT_MOC_LITERAL(17, 179, 12)
    },
    "CSoftItemWidget\0NotifyOperation\0\0"
    "SMonitor::Type\0type\0category\0name\0"
    "downloadNotify\0key\0QString&\0srcPath\0"
    "desPath\0updateState\0SoftData\0setOperation\0"
    "installFinished\0installErrorOccur\0"
    "downloadQuit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CSoftItemWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   49,    2, 0x06 /* Public */,
       7,    3,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   63,    2, 0x0a /* Public */,
      14,    0,   66,    2, 0x0a /* Public */,
      15,    0,   67,    2, 0x0a /* Public */,
      16,    0,   68,    2, 0x0a /* Public */,
      17,    0,   69,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString, QMetaType::QString,    4,    5,    6,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 9, QMetaType::QString,    8,   10,   11,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 13,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CSoftItemWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CSoftItemWidget *_t = static_cast<CSoftItemWidget *>(_o);
        switch (_id) {
        case 0: _t->NotifyOperation((*reinterpret_cast< SMonitor::Type(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: _t->downloadNotify((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->updateState((*reinterpret_cast< const SoftData(*)>(_a[1]))); break;
        case 3: _t->setOperation(); break;
        case 4: _t->installFinished(); break;
        case 5: _t->installErrorOccur(); break;
        case 6: _t->downloadQuit(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CSoftItemWidget::*_t)(SMonitor::Type , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftItemWidget::NotifyOperation)) {
                *result = 0;
            }
        }
        {
            typedef void (CSoftItemWidget::*_t)(const QString & , QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftItemWidget::downloadNotify)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject CSoftItemWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_CSoftItemWidget.data,
      qt_meta_data_CSoftItemWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *CSoftItemWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CSoftItemWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CSoftItemWidget.stringdata))
        return static_cast<void*>(const_cast< CSoftItemWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int CSoftItemWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CSoftItemWidget::NotifyOperation(SMonitor::Type _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CSoftItemWidget::downloadNotify(const QString & _t1, QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

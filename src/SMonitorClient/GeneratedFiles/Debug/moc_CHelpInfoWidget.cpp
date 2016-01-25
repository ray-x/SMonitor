/****************************************************************************
** Meta object code from reading C++ file 'CHelpInfoWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CHelpInfoWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CHelpInfoWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CHelpInfoWidget_t {
    QByteArrayData data[14];
    char stringdata[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CHelpInfoWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CHelpInfoWidget_t qt_meta_stringdata_CHelpInfoWidget = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 14),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 8),
QT_MOC_LITERAL(4, 41, 10),
QT_MOC_LITERAL(5, 52, 8),
QT_MOC_LITERAL(6, 61, 7),
QT_MOC_LITERAL(7, 69, 7),
QT_MOC_LITERAL(8, 77, 14),
QT_MOC_LITERAL(9, 92, 8),
QT_MOC_LITERAL(10, 101, 16),
QT_MOC_LITERAL(11, 118, 9),
QT_MOC_LITERAL(12, 128, 4),
QT_MOC_LITERAL(13, 133, 13)
    },
    "CHelpInfoWidget\0updateHelpInfo\0\0"
    "modifyId\0SendResult\0nMsgHead\0bResult\0"
    "strInfo\0serverReqError\0strError\0"
    "updateInfomation\0StringMap\0data\0"
    "modifySuccess"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CHelpInfoWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    3,   46,    2, 0x08 /* Private */,
       8,    2,   53,    2, 0x08 /* Private */,
      10,    1,   58,    2, 0x08 /* Private */,
      13,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::QString,    5,    6,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    5,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,

       0        // eod
};

void CHelpInfoWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CHelpInfoWidget *_t = static_cast<CHelpInfoWidget *>(_o);
        switch (_id) {
        case 0: _t->updateHelpInfo(); break;
        case 1: _t->modifyId(); break;
        case 2: _t->SendResult((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->serverReqError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->updateInfomation((*reinterpret_cast< const StringMap(*)>(_a[1]))); break;
        case 5: _t->modifySuccess(); break;
        default: ;
        }
    }
}

const QMetaObject CHelpInfoWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CHelpInfoWidget.data,
      qt_meta_data_CHelpInfoWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *CHelpInfoWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CHelpInfoWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CHelpInfoWidget.stringdata))
        return static_cast<void*>(const_cast< CHelpInfoWidget*>(this));
    if (!strcmp(_clname, "Ui::CHelpInfoWidget"))
        return static_cast< Ui::CHelpInfoWidget*>(const_cast< CHelpInfoWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CHelpInfoWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE

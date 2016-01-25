/****************************************************************************
** Meta object code from reading C++ file 'CHelpInfoWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
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
    QByteArrayData data[9];
    char stringdata[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CHelpInfoWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CHelpInfoWidget_t qt_meta_stringdata_CHelpInfoWidget = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 10),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 13),
QT_MOC_LITERAL(4, 42, 11),
QT_MOC_LITERAL(5, 54, 11),
QT_MOC_LITERAL(6, 66, 17),
QT_MOC_LITERAL(7, 84, 4),
QT_MOC_LITERAL(8, 89, 9)
    },
    "CHelpInfoWidget\0addContent\0\0removeContent\0"
    "saveContent\0itemChanged\0QTableWidgetItem*\0"
    "item\0SaveToXml"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CHelpInfoWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    1,   42,    2, 0x08 /* Private */,
       8,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,

       0        // eod
};

void CHelpInfoWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CHelpInfoWidget *_t = static_cast<CHelpInfoWidget *>(_o);
        switch (_id) {
        case 0: _t->addContent(); break;
        case 1: _t->removeContent(); break;
        case 2: _t->saveContent(); break;
        case 3: _t->itemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->SaveToXml(); break;
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
    if (!strcmp(_clname, "Ui::CHelpInfo"))
        return static_cast< Ui::CHelpInfo*>(const_cast< CHelpInfoWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CHelpInfoWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE

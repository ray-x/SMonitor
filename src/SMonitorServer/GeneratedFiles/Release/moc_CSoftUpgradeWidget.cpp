/****************************************************************************
** Meta object code from reading C++ file 'CSoftUpgradeWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../CSoftUpgradeWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CSoftUpgradeWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CSoftUpgradeWidget_t {
    QByteArrayData data[21];
    char stringdata[247];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CSoftUpgradeWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CSoftUpgradeWidget_t qt_meta_stringdata_CSoftUpgradeWidget = {
    {
QT_MOC_LITERAL(0, 0, 18),
QT_MOC_LITERAL(1, 19, 11),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 11),
QT_MOC_LITERAL(4, 44, 4),
QT_MOC_LITERAL(5, 49, 9),
QT_MOC_LITERAL(6, 59, 10),
QT_MOC_LITERAL(7, 70, 12),
QT_MOC_LITERAL(8, 83, 9),
QT_MOC_LITERAL(9, 93, 7),
QT_MOC_LITERAL(10, 101, 10),
QT_MOC_LITERAL(11, 112, 11),
QT_MOC_LITERAL(12, 124, 17),
QT_MOC_LITERAL(13, 142, 4),
QT_MOC_LITERAL(14, 147, 17),
QT_MOC_LITERAL(15, 165, 21),
QT_MOC_LITERAL(16, 187, 3),
QT_MOC_LITERAL(17, 191, 11),
QT_MOC_LITERAL(18, 203, 19),
QT_MOC_LITERAL(19, 223, 10),
QT_MOC_LITERAL(20, 234, 12)
    },
    "CSoftUpgradeWidget\0dataChanged\0\0"
    "itemRemoved\0nRow\0itemAdded\0brandAdded\0"
    "brandChanged\0datasaved\0addItem\0"
    "removeItem\0itemChanged\0QTableWidgetItem*\0"
    "item\0SaveContentToFile\0SaveContentToDatabase\0"
    "row\0addSubBrand\0currentBrandChanged\0"
    "initWidget\0PasswdConfig"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CSoftUpgradeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       3,    2,   97,    2, 0x06 /* Public */,
       5,    1,  102,    2, 0x06 /* Public */,
       6,    1,  105,    2, 0x06 /* Public */,
       7,    0,  108,    2, 0x06 /* Public */,
       8,    0,  109,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  110,    2, 0x08 /* Private */,
      10,    0,  111,    2, 0x08 /* Private */,
      11,    1,  112,    2, 0x08 /* Private */,
      14,    0,  115,    2, 0x08 /* Private */,
      15,    1,  116,    2, 0x08 /* Private */,
      15,    0,  119,    2, 0x28 /* Private | MethodCloned */,
      17,    0,  120,    2, 0x08 /* Private */,
      18,    1,  121,    2, 0x08 /* Private */,
      19,    0,  124,    2, 0x08 /* Private */,
      20,    0,  125,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    4,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CSoftUpgradeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CSoftUpgradeWidget *_t = static_cast<CSoftUpgradeWidget *>(_o);
        switch (_id) {
        case 0: _t->dataChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->itemRemoved((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->itemAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->brandAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->brandChanged(); break;
        case 5: _t->datasaved(); break;
        case 6: _t->addItem(); break;
        case 7: _t->removeItem(); break;
        case 8: _t->itemChanged((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 9: _t->SaveContentToFile(); break;
        case 10: _t->SaveContentToDatabase((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->SaveContentToDatabase(); break;
        case 12: _t->addSubBrand(); break;
        case 13: _t->currentBrandChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->initWidget(); break;
        case 15: _t->PasswdConfig(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CSoftUpgradeWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftUpgradeWidget::dataChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (CSoftUpgradeWidget::*_t)(const QString & , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftUpgradeWidget::itemRemoved)) {
                *result = 1;
            }
        }
        {
            typedef void (CSoftUpgradeWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftUpgradeWidget::itemAdded)) {
                *result = 2;
            }
        }
        {
            typedef void (CSoftUpgradeWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftUpgradeWidget::brandAdded)) {
                *result = 3;
            }
        }
        {
            typedef void (CSoftUpgradeWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftUpgradeWidget::brandChanged)) {
                *result = 4;
            }
        }
        {
            typedef void (CSoftUpgradeWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSoftUpgradeWidget::datasaved)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject CSoftUpgradeWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CSoftUpgradeWidget.data,
      qt_meta_data_CSoftUpgradeWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *CSoftUpgradeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CSoftUpgradeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CSoftUpgradeWidget.stringdata))
        return static_cast<void*>(const_cast< CSoftUpgradeWidget*>(this));
    if (!strcmp(_clname, "Ui::CSoftUpgredeWidget"))
        return static_cast< Ui::CSoftUpgredeWidget*>(const_cast< CSoftUpgradeWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CSoftUpgradeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void CSoftUpgradeWidget::dataChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CSoftUpgradeWidget::itemRemoved(const QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CSoftUpgradeWidget::itemAdded(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CSoftUpgradeWidget::brandAdded(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CSoftUpgradeWidget::brandChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void CSoftUpgradeWidget::datasaved()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE

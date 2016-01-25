/****************************************************************************
** Meta object code from reading C++ file 'CStatisticsWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "../../CStatisticsWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CStatisticsWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CStatisticsWidget_t {
    QByteArrayData data[20];
    char stringdata[218];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CStatisticsWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CStatisticsWidget_t qt_meta_stringdata_CStatisticsWidget = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 10),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 10),
QT_MOC_LITERAL(4, 41, 3),
QT_MOC_LITERAL(5, 45, 10),
QT_MOC_LITERAL(6, 56, 4),
QT_MOC_LITERAL(7, 61, 7),
QT_MOC_LITERAL(8, 69, 10),
QT_MOC_LITERAL(9, 80, 17),
QT_MOC_LITERAL(10, 98, 22),
QT_MOC_LITERAL(11, 121, 3),
QT_MOC_LITERAL(12, 125, 19),
QT_MOC_LITERAL(13, 145, 16),
QT_MOC_LITERAL(14, 162, 4),
QT_MOC_LITERAL(15, 167, 7),
QT_MOC_LITERAL(16, 175, 10),
QT_MOC_LITERAL(17, 186, 12),
QT_MOC_LITERAL(18, 199, 8),
QT_MOC_LITERAL(19, 208, 9)
    },
    "CStatisticsWidget\0updateData\0\0updateItem\0"
    "key\0removeItem\0nRow\0addItem\0exportData\0"
    "updateInstallData\0QPair<QString,QString>\0"
    "num\0updateUnInstallData\0updateAllNumData\0"
    "name\0version\0installNum\0unInstallNum\0"
    "initData\0exportAll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CStatisticsWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    1,   65,    2, 0x0a /* Public */,
       5,    2,   68,    2, 0x0a /* Public */,
       7,    1,   73,    2, 0x0a /* Public */,
       8,    0,   76,    2, 0x0a /* Public */,
       9,    2,   77,    2, 0x0a /* Public */,
      12,    2,   82,    2, 0x0a /* Public */,
      13,    4,   87,    2, 0x0a /* Public */,
      18,    0,   96,    2, 0x0a /* Public */,
      19,    0,   97,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    4,    6,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,    4,   11,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,    4,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Int, QMetaType::Int,   14,   15,   16,   17,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CStatisticsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CStatisticsWidget *_t = static_cast<CStatisticsWidget *>(_o);
        switch (_id) {
        case 0: _t->updateData(); break;
        case 1: _t->updateItem((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->removeItem((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->addItem((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->exportData(); break;
        case 5: _t->updateInstallData((*reinterpret_cast< QPair<QString,QString>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->updateUnInstallData((*reinterpret_cast< QPair<QString,QString>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->updateAllNumData((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 8: _t->initData(); break;
        case 9: _t->exportAll(); break;
        default: ;
        }
    }
}

const QMetaObject CStatisticsWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CStatisticsWidget.data,
      qt_meta_data_CStatisticsWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *CStatisticsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CStatisticsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CStatisticsWidget.stringdata))
        return static_cast<void*>(const_cast< CStatisticsWidget*>(this));
    if (!strcmp(_clname, "Ui::CStatisticsWidget"))
        return static_cast< Ui::CStatisticsWidget*>(const_cast< CStatisticsWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CStatisticsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

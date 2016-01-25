/****************************************************************************
** Meta object code from reading C++ file 'CSMonitorClient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CSMonitorClient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CSMonitorClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CSMonitorClient_t {
    QByteArrayData data[29];
    char stringdata[425];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CSMonitorClient_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CSMonitorClient_t qt_meta_stringdata_CSMonitorClient = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 23),
QT_MOC_LITERAL(2, 40, 0),
QT_MOC_LITERAL(3, 41, 6),
QT_MOC_LITERAL(4, 48, 7),
QT_MOC_LITERAL(5, 56, 5),
QT_MOC_LITERAL(6, 62, 17),
QT_MOC_LITERAL(7, 80, 33),
QT_MOC_LITERAL(8, 114, 6),
QT_MOC_LITERAL(9, 121, 9),
QT_MOC_LITERAL(10, 131, 19),
QT_MOC_LITERAL(11, 151, 5),
QT_MOC_LITERAL(12, 157, 10),
QT_MOC_LITERAL(13, 168, 12),
QT_MOC_LITERAL(14, 181, 13),
QT_MOC_LITERAL(15, 195, 15),
QT_MOC_LITERAL(16, 211, 16),
QT_MOC_LITERAL(17, 228, 18),
QT_MOC_LITERAL(18, 247, 19),
QT_MOC_LITERAL(19, 267, 27),
QT_MOC_LITERAL(20, 295, 6),
QT_MOC_LITERAL(21, 302, 29),
QT_MOC_LITERAL(22, 332, 27),
QT_MOC_LITERAL(23, 360, 15),
QT_MOC_LITERAL(24, 376, 14),
QT_MOC_LITERAL(25, 391, 4),
QT_MOC_LITERAL(26, 396, 8),
QT_MOC_LITERAL(27, 405, 4),
QT_MOC_LITERAL(28, 410, 14)
    },
    "CSMonitorClient\0sig_showIndependentItem\0"
    "\0strKey\0version\0bShow\0activedByTrayIcon\0"
    "QSystemTrayIcon::ActivationReason\0"
    "reason\0maximized\0setWidgetStyleSheet\0"
    "style\0hideOrShow\0setMinimumed\0"
    "updateArrived\0setServerXmlUrl\0"
    "strUpdateXmlPath\0strHelpInfoXmlPath\0"
    "strVersionCheckFile\0downloadUpdateXMLErrorOccur\0"
    "errMsg\0downloadHelpInfoXMLErrorOccur\0"
    "downloadCheckFileErrorOccur\0downloadStarted\0"
    "SMonitor::Type\0type\0category\0name\0"
    "slot_checkFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CSMonitorClient[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   86,    2, 0x08 /* Private */,
       9,    0,   89,    2, 0x08 /* Private */,
      10,    1,   90,    2, 0x08 /* Private */,
      12,    0,   93,    2, 0x08 /* Private */,
      13,    0,   94,    2, 0x08 /* Private */,
      14,    0,   95,    2, 0x08 /* Private */,
      15,    3,   96,    2, 0x08 /* Private */,
      19,    1,  103,    2, 0x08 /* Private */,
      21,    1,  106,    2, 0x08 /* Private */,
      22,    1,  109,    2, 0x08 /* Private */,
      23,    3,  112,    2, 0x08 /* Private */,
      28,    0,  119,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool,    3,    4,    5,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   16,   17,   18,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, 0x80000000 | 24, QMetaType::QString, QMetaType::QString,   25,   26,   27,
    QMetaType::Void,

       0        // eod
};

void CSMonitorClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CSMonitorClient *_t = static_cast<CSMonitorClient *>(_o);
        switch (_id) {
        case 0: _t->sig_showIndependentItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->activedByTrayIcon((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 2: _t->maximized(); break;
        case 3: _t->setWidgetStyleSheet((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->hideOrShow(); break;
        case 5: _t->setMinimumed(); break;
        case 6: _t->updateArrived(); break;
        case 7: _t->setServerXmlUrl((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 8: _t->downloadUpdateXMLErrorOccur((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->downloadHelpInfoXMLErrorOccur((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->downloadCheckFileErrorOccur((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->downloadStarted((*reinterpret_cast< SMonitor::Type(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 12: _t->slot_checkFile(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CSMonitorClient::*_t)(QString , QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CSMonitorClient::sig_showIndependentItem)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject CSMonitorClient::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_CSMonitorClient.data,
      qt_meta_data_CSMonitorClient,  qt_static_metacall, 0, 0}
};


const QMetaObject *CSMonitorClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CSMonitorClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CSMonitorClient.stringdata))
        return static_cast<void*>(const_cast< CSMonitorClient*>(this));
    return QFrame::qt_metacast(_clname);
}

int CSMonitorClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void CSMonitorClient::sig_showIndependentItem(QString _t1, QString _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

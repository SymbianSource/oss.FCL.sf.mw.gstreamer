/****************************************************************************
** Meta object code from reading C++ file 'folderview.h'
**
** Created: Tue Feb 23 15:59:43 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "folderview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'folderview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FolderView[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      40,   34,   11,   11, 0x0a,
      66,   11,   11,   11, 0x0a,
      79,   11,   11,   11, 0x0a,
      92,   11,   11,   11, 0x0a,
     105,   11,   11,   11, 0x0a,
     119,   11,   11,   11, 0x0a,
     133,   11,   11,   11, 0x0a,
     147,   11,   11,   11, 0x0a,
     160,   11,   11,   11, 0x0a,
     168,   11,   11,   11, 0x0a,
     177,   11,   11,   11, 0x0a,
     184,   11,   11,   11, 0x0a,
     203,  199,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FolderView[] = {
    "FolderView\0\0folderViewTriggered()\0"
    "index\0item_pressed(QModelIndex)\0"
    "record_AMR()\0record_WAV()\0record_RAW()\0"
    "record_G711()\0record_G729()\0record_ILBC()\0"
    "record_AAC()\0pause()\0resume()\0stop()\0"
    "timertimeout()\0err\0showNote(int)\0"
};

const QMetaObject FolderView::staticMetaObject = {
    { &HbView::staticMetaObject, qt_meta_stringdata_FolderView,
      qt_meta_data_FolderView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FolderView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FolderView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FolderView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FolderView))
        return static_cast<void*>(const_cast< FolderView*>(this));
    return HbView::qt_metacast(_clname);
}

int FolderView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HbView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: folderViewTriggered(); break;
        case 1: item_pressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: record_AMR(); break;
        case 3: record_WAV(); break;
        case 4: record_RAW(); break;
        case 5: record_G711(); break;
        case 6: record_G729(); break;
        case 7: record_ILBC(); break;
        case 8: record_AAC(); break;
        case 9: pause(); break;
        case 10: resume(); break;
        case 11: stop(); break;
        case 12: timertimeout(); break;
        case 13: showNote((*reinterpret_cast< const int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'editablewrapper.h'
**
** Created: Sun Sep 28 10:50:35 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "editablewrapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editablewrapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_EditableWrapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   17,   16,   16, 0x05,
      56,   52,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_EditableWrapper[] = {
    "EditableWrapper\0\0pObject\0"
    "selected(EditableWrapper*)\0str\0"
    "showHint(QString)\0"
};

const QMetaObject EditableWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EditableWrapper,
      qt_meta_data_EditableWrapper, 0 }
};

const QMetaObject *EditableWrapper::metaObject() const
{
    return &staticMetaObject;
}

void *EditableWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditableWrapper))
	return static_cast<void*>(const_cast< EditableWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int EditableWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selected((*reinterpret_cast< EditableWrapper*(*)>(_a[1]))); break;
        case 1: showHint((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void EditableWrapper::selected(EditableWrapper * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EditableWrapper::showHint(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

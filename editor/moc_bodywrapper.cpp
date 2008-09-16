/****************************************************************************
** Meta object code from reading C++ file 'bodywrapper.h'
**
** Created: Tue Sep 16 23:13:50 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bodywrapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bodywrapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_BodyWrapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      10,   10, // properties
       0,    0, // enums/sets

 // properties: name, type, flags
      20,   12, 0x0a195103,
      32,   25, 0x06195103,
      37,   25, 0x06195103,
      49,   25, 0x06195103,
      61,   25, 0x06195103,
      75,   25, 0x06195103,
      95,   90, 0x01195103,
     104,   25, 0x06195103,
     113,   25, 0x06195103,
     122,   25, 0x06195103,

       0        // eod
};

static const char qt_meta_stringdata_BodyWrapper[] = {
    "BodyWrapper\0QString\0Name\0double\0Mass\0"
    "MassCenterX\0MassCenterY\0LinearDamping\0"
    "AngularDamping\0bool\0IsBullet\0TextureX\0"
    "TextureY\0TextureScale\0"
};

const QMetaObject BodyWrapper::staticMetaObject = {
    { &EditableWrapper::staticMetaObject, qt_meta_stringdata_BodyWrapper,
      qt_meta_data_BodyWrapper, 0 }
};

const QMetaObject *BodyWrapper::metaObject() const
{
    return &staticMetaObject;
}

void *BodyWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BodyWrapper))
	return static_cast<void*>(const_cast< BodyWrapper*>(this));
    return EditableWrapper::qt_metacast(_clname);
}

int BodyWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EditableWrapper::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getName(); break;
        case 1: *reinterpret_cast< double*>(_v) = getMass(); break;
        case 2: *reinterpret_cast< double*>(_v) = getMassCenterX(); break;
        case 3: *reinterpret_cast< double*>(_v) = getMassCenterY(); break;
        case 4: *reinterpret_cast< double*>(_v) = getLinearDamping(); break;
        case 5: *reinterpret_cast< double*>(_v) = getAngularDamping(); break;
        case 6: *reinterpret_cast< bool*>(_v) = getIsBullet(); break;
        case 7: *reinterpret_cast< double*>(_v) = getTextureX(); break;
        case 8: *reinterpret_cast< double*>(_v) = getTextureY(); break;
        case 9: *reinterpret_cast< double*>(_v) = getTextureScale(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setMass(*reinterpret_cast< double*>(_v)); break;
        case 2: setMassCenterX(*reinterpret_cast< double*>(_v)); break;
        case 3: setMassCenterY(*reinterpret_cast< double*>(_v)); break;
        case 4: setLinearDamping(*reinterpret_cast< double*>(_v)); break;
        case 5: setAngularDamping(*reinterpret_cast< double*>(_v)); break;
        case 6: setIsBullet(*reinterpret_cast< bool*>(_v)); break;
        case 7: setTextureX(*reinterpret_cast< double*>(_v)); break;
        case 8: setTextureY(*reinterpret_cast< double*>(_v)); break;
        case 9: setTextureScale(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

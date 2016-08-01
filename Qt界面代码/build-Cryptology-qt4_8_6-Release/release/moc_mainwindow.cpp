/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Cryptology/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      31,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      32,   11,   11,   11, 0x08,
      53,   11,   11,   11, 0x08,
      75,   11,   11,   11, 0x08,
      96,   11,   11,   11, 0x08,
     118,   11,   11,   11, 0x08,
     143,   11,   11,   11, 0x08,
     170,   11,   11,   11, 0x08,
     187,   11,   11,   11, 0x08,
     204,   11,   11,   11, 0x08,
     221,   11,   11,   11, 0x08,
     230,   11,   11,   11, 0x08,
     239,   11,   11,   11, 0x08,
     248,   11,   11,   11, 0x08,
     257,   11,   11,   11, 0x08,
     266,   11,   11,   11, 0x08,
     275,   11,   11,   11, 0x08,
     284,   11,   11,   11, 0x08,
     293,   11,   11,   11, 0x08,
     302,   11,   11,   11, 0x08,
     312,   11,   11,   11, 0x08,
     322,   11,   11,   11, 0x08,
     340,   11,   11,   11, 0x08,
     361,   11,   11,   11, 0x08,
     382,   11,   11,   11, 0x08,
     401,   11,   11,   11, 0x08,
     421,   11,   11,   11, 0x08,
     442,   11,   11,   11, 0x08,
     460,   11,   11,   11, 0x08,
     481,   11,   11,   11, 0x08,
     505,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_Caesar_clicked()\0"
    "on_Keyword_clicked()\0on_Vigenere_clicked()\0"
    "on_Autokey_clicked()\0on_Playfair_clicked()\0"
    "on_Permutation_clicked()\0"
    "on_C_Permutation_clicked()\0on_RC4_clicked()\0"
    "on_DES_clicked()\0on_RSA_clicked()\0"
    "slot_1()\0slot_2()\0slot_3()\0slot_4()\0"
    "slot_5()\0slot_6()\0slot_7()\0slot_8()\0"
    "slot_9()\0slot_10()\0slot_11()\0"
    "on_file_clicked()\0on_Encrypt_clicked()\0"
    "on_Decrypt_clicked()\0on_Clean_clicked()\0"
    "on_Socket_clicked()\0on_connect_clicked()\0"
    "on_Send_clicked()\0on_Receive_clicked()\0"
    "on_disconnect_clicked()\0on_DH_clicked()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->on_Caesar_clicked(); break;
        case 1: _t->on_Keyword_clicked(); break;
        case 2: _t->on_Vigenere_clicked(); break;
        case 3: _t->on_Autokey_clicked(); break;
        case 4: _t->on_Playfair_clicked(); break;
        case 5: _t->on_Permutation_clicked(); break;
        case 6: _t->on_C_Permutation_clicked(); break;
        case 7: _t->on_RC4_clicked(); break;
        case 8: _t->on_DES_clicked(); break;
        case 9: _t->on_RSA_clicked(); break;
        case 10: _t->slot_1(); break;
        case 11: _t->slot_2(); break;
        case 12: _t->slot_3(); break;
        case 13: _t->slot_4(); break;
        case 14: _t->slot_5(); break;
        case 15: _t->slot_6(); break;
        case 16: _t->slot_7(); break;
        case 17: _t->slot_8(); break;
        case 18: _t->slot_9(); break;
        case 19: _t->slot_10(); break;
        case 20: _t->slot_11(); break;
        case 21: _t->on_file_clicked(); break;
        case 22: _t->on_Encrypt_clicked(); break;
        case 23: _t->on_Decrypt_clicked(); break;
        case 24: _t->on_Clean_clicked(); break;
        case 25: _t->on_Socket_clicked(); break;
        case 26: _t->on_connect_clicked(); break;
        case 27: _t->on_Send_clicked(); break;
        case 28: _t->on_Receive_clicked(); break;
        case 29: _t->on_disconnect_clicked(); break;
        case 30: _t->on_DH_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'NetworkClient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../NetworkClient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13NetworkClientE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkClient::qt_create_metaobjectdata<qt_meta_tag_ZN13NetworkClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkClient",
        "connected",
        "",
        "disconnected",
        "registerResponse",
        "success",
        "error",
        "loginResponse",
        "token",
        "logoutResponse",
        "errorOccurred",
        "quickModeGameStart",
        "sessionId",
        "totalRounds",
        "quickModeQuestionReceived",
        "round",
        "questionId",
        "content",
        "QJsonObject",
        "options",
        "difficulty",
        "quickModeAnswerResult",
        "correct",
        "correctAnswer",
        "score",
        "gameOver",
        "quickModeGameOver",
        "finalScore",
        "status",
        "win",
        "quickModeLifelineResult",
        "remainingOptions",
        "removedOptions",
        "remaining",
        "oneVNRoomCreated",
        "roomId",
        "oneVNRoomJoined",
        "oneVNRoomUpdate",
        "QJsonArray",
        "members",
        "oneVNGameStart1VN",
        "oneVNQuestion1VNReceived",
        "timeLimit",
        "oneVNAnswerResult1VN",
        "totalScore",
        "eliminated",
        "timeout",
        "oneVNElimination",
        "userId",
        "oneVNGameOver1VN",
        "winnerId",
        "leaderboard",
        "onReadyRead",
        "onSocketError",
        "QAbstractSocket::SocketError",
        "onSocketStateChanged",
        "QAbstractSocket::SocketState",
        "state"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'registerResponse'
        QtMocHelpers::SignalData<void(bool, const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'loginResponse'
        QtMocHelpers::SignalData<void(bool, const QString &, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 }, { QMetaType::QString, 8 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'logoutResponse'
        QtMocHelpers::SignalData<void(bool)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'quickModeGameStart'
        QtMocHelpers::SignalData<void(qint64, int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 }, { QMetaType::Int, 13 },
        }}),
        // Signal 'quickModeQuestionReceived'
        QtMocHelpers::SignalData<void(qint64, int, qint64, const QString &, const QJsonObject &, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 }, { QMetaType::Int, 15 }, { QMetaType::LongLong, 16 }, { QMetaType::QString, 17 },
            { 0x80000000 | 18, 19 }, { QMetaType::QString, 20 },
        }}),
        // Signal 'quickModeAnswerResult'
        QtMocHelpers::SignalData<void(qint64, int, bool, const QString &, int, bool)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 }, { QMetaType::Int, 15 }, { QMetaType::Bool, 22 }, { QMetaType::QString, 23 },
            { QMetaType::Int, 24 }, { QMetaType::Bool, 25 },
        }}),
        // Signal 'quickModeGameOver'
        QtMocHelpers::SignalData<void(qint64, int, int, const QString &, bool)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 }, { QMetaType::Int, 27 }, { QMetaType::Int, 13 }, { QMetaType::QString, 28 },
            { QMetaType::Bool, 29 },
        }}),
        // Signal 'quickModeLifelineResult'
        QtMocHelpers::SignalData<void(qint64, int, const QStringList &, const QStringList &, int)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 }, { QMetaType::Int, 15 }, { QMetaType::QStringList, 31 }, { QMetaType::QStringList, 32 },
            { QMetaType::Int, 33 },
        }}),
        // Signal 'oneVNRoomCreated'
        QtMocHelpers::SignalData<void(qint64)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 35 },
        }}),
        // Signal 'oneVNRoomJoined'
        QtMocHelpers::SignalData<void(bool, const QString &)>(36, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'oneVNRoomUpdate'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 38, 39 },
        }}),
        // Signal 'oneVNGameStart1VN'
        QtMocHelpers::SignalData<void(qint64, qint64, int)>(40, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 }, { QMetaType::LongLong, 35 }, { QMetaType::Int, 13 },
        }}),
        // Signal 'oneVNQuestion1VNReceived'
        QtMocHelpers::SignalData<void(int, int, const QString &, qint64, const QString &, const QJsonObject &, int)>(41, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { QMetaType::Int, 13 }, { QMetaType::QString, 20 }, { QMetaType::LongLong, 16 },
            { QMetaType::QString, 17 }, { 0x80000000 | 18, 19 }, { QMetaType::Int, 42 },
        }}),
        // Signal 'oneVNAnswerResult1VN'
        QtMocHelpers::SignalData<void(bool, int, int, bool, bool)>(43, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 22 }, { QMetaType::Int, 24 }, { QMetaType::Int, 44 }, { QMetaType::Bool, 45 },
            { QMetaType::Bool, 46 },
        }}),
        // Signal 'oneVNAnswerResult1VN'
        QtMocHelpers::SignalData<void(bool, int, int, bool)>(43, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::Bool, 22 }, { QMetaType::Int, 24 }, { QMetaType::Int, 44 }, { QMetaType::Bool, 45 },
        }}),
        // Signal 'oneVNElimination'
        QtMocHelpers::SignalData<void(qint64, int)>(47, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 48 }, { QMetaType::Int, 15 },
        }}),
        // Signal 'oneVNGameOver1VN'
        QtMocHelpers::SignalData<void(qint64, const QJsonArray &)>(49, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 50 }, { 0x80000000 | 38, 51 },
        }}),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 54, 6 },
        }}),
        // Slot 'onSocketStateChanged'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketState)>(55, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 56, 57 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkClient, qt_meta_tag_ZN13NetworkClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13NetworkClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13NetworkClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13NetworkClientE_t>.metaTypes,
    nullptr
} };

void NetworkClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->registerResponse((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->loginResponse((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 4: _t->logoutResponse((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->quickModeGameStart((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->quickModeQuestionReceived((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[6]))); break;
        case 8: _t->quickModeAnswerResult((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[6]))); break;
        case 9: _t->quickModeGameOver((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[5]))); break;
        case 10: _t->quickModeLifelineResult((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5]))); break;
        case 11: _t->oneVNRoomCreated((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 12: _t->oneVNRoomJoined((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 13: _t->oneVNRoomUpdate((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 14: _t->oneVNGameStart1VN((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 15: _t->oneVNQuestion1VNReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[6])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[7]))); break;
        case 16: _t->oneVNAnswerResult1VN((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[5]))); break;
        case 17: _t->oneVNAnswerResult1VN((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4]))); break;
        case 18: _t->oneVNElimination((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 19: _t->oneVNGameOver1VN((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[2]))); break;
        case 20: _t->onReadyRead(); break;
        case 21: _t->onSocketError((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 22: _t->onSocketStateChanged((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketState>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketState >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)()>(_a, &NetworkClient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)()>(_a, &NetworkClient::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(bool , const QString & )>(_a, &NetworkClient::registerResponse, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(bool , const QString & , const QString & )>(_a, &NetworkClient::loginResponse, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(bool )>(_a, &NetworkClient::logoutResponse, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(const QString & )>(_a, &NetworkClient::errorOccurred, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , int )>(_a, &NetworkClient::quickModeGameStart, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , int , qint64 , const QString & , const QJsonObject & , const QString & )>(_a, &NetworkClient::quickModeQuestionReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , int , bool , const QString & , int , bool )>(_a, &NetworkClient::quickModeAnswerResult, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , int , int , const QString & , bool )>(_a, &NetworkClient::quickModeGameOver, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , int , const QStringList & , const QStringList & , int )>(_a, &NetworkClient::quickModeLifelineResult, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 )>(_a, &NetworkClient::oneVNRoomCreated, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(bool , const QString & )>(_a, &NetworkClient::oneVNRoomJoined, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(const QJsonArray & )>(_a, &NetworkClient::oneVNRoomUpdate, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , qint64 , int )>(_a, &NetworkClient::oneVNGameStart1VN, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(int , int , const QString & , qint64 , const QString & , const QJsonObject & , int )>(_a, &NetworkClient::oneVNQuestion1VNReceived, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(bool , int , int , bool , bool )>(_a, &NetworkClient::oneVNAnswerResult1VN, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , int )>(_a, &NetworkClient::oneVNElimination, 18))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkClient::*)(qint64 , const QJsonArray & )>(_a, &NetworkClient::oneVNGameOver1VN, 19))
            return;
    }
}

const QMetaObject *NetworkClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13NetworkClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void NetworkClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NetworkClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkClient::registerResponse(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void NetworkClient::loginResponse(bool _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}

// SIGNAL 4
void NetworkClient::logoutResponse(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void NetworkClient::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void NetworkClient::quickModeGameStart(qint64 _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void NetworkClient::quickModeQuestionReceived(qint64 _t1, int _t2, qint64 _t3, const QString & _t4, const QJsonObject & _t5, const QString & _t6)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3, _t4, _t5, _t6);
}

// SIGNAL 8
void NetworkClient::quickModeAnswerResult(qint64 _t1, int _t2, bool _t3, const QString & _t4, int _t5, bool _t6)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2, _t3, _t4, _t5, _t6);
}

// SIGNAL 9
void NetworkClient::quickModeGameOver(qint64 _t1, int _t2, int _t3, const QString & _t4, bool _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 10
void NetworkClient::quickModeLifelineResult(qint64 _t1, int _t2, const QStringList & _t3, const QStringList & _t4, int _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 11
void NetworkClient::oneVNRoomCreated(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void NetworkClient::oneVNRoomJoined(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2);
}

// SIGNAL 13
void NetworkClient::oneVNRoomUpdate(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void NetworkClient::oneVNGameStart1VN(qint64 _t1, qint64 _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1, _t2, _t3);
}

// SIGNAL 15
void NetworkClient::oneVNQuestion1VNReceived(int _t1, int _t2, const QString & _t3, qint64 _t4, const QString & _t5, const QJsonObject & _t6, int _t7)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1, _t2, _t3, _t4, _t5, _t6, _t7);
}

// SIGNAL 16
void NetworkClient::oneVNAnswerResult1VN(bool _t1, int _t2, int _t3, bool _t4, bool _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 16, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 18
void NetworkClient::oneVNElimination(qint64 _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 18, nullptr, _t1, _t2);
}

// SIGNAL 19
void NetworkClient::oneVNGameOver1VN(qint64 _t1, const QJsonArray & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 19, nullptr, _t1, _t2);
}
QT_WARNING_POP

/****************************************************************************
** Meta object code from reading C++ file 'OneVNWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../OneVNWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OneVNWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11OneVNWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto OneVNWindow::qt_create_metaobjectdata<qt_meta_tag_ZN11OneVNWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OneVNWindow",
        "onCreateRoomClicked",
        "",
        "onJoinRoomClicked",
        "onStartGameClicked",
        "onLeaveRoomClicked",
        "onAnswerClicked",
        "onQuestionTimeout",
        "updateTimer",
        "onRoomCreated",
        "roomId",
        "onRoomJoined",
        "success",
        "error",
        "onRoomUpdate",
        "QJsonArray",
        "members",
        "onGameStart1VN",
        "sessionId",
        "totalRounds",
        "onQuestion1VNReceived",
        "round",
        "difficulty",
        "questionId",
        "content",
        "QJsonObject",
        "options",
        "timeLimit",
        "onAnswerResult1VN",
        "correct",
        "score",
        "totalScore",
        "eliminated",
        "onElimination",
        "userId",
        "onGameOver1VN",
        "winnerId",
        "leaderboard"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onCreateRoomClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onJoinRoomClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartGameClicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLeaveRoomClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAnswerClicked'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onQuestionTimeout'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTimer'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomCreated'
        QtMocHelpers::SlotData<void(qint64)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 10 },
        }}),
        // Slot 'onRoomJoined'
        QtMocHelpers::SlotData<void(bool, const QString &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 12 }, { QMetaType::QString, 13 },
        }}),
        // Slot 'onRoomUpdate'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'onGameStart1VN'
        QtMocHelpers::SlotData<void(qint64, qint64, int)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 18 }, { QMetaType::LongLong, 10 }, { QMetaType::Int, 19 },
        }}),
        // Slot 'onQuestion1VNReceived'
        QtMocHelpers::SlotData<void(int, int, const QString &, qint64, const QString &, const QJsonObject &, int)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 21 }, { QMetaType::Int, 19 }, { QMetaType::QString, 22 }, { QMetaType::LongLong, 23 },
            { QMetaType::QString, 24 }, { 0x80000000 | 25, 26 }, { QMetaType::Int, 27 },
        }}),
        // Slot 'onAnswerResult1VN'
        QtMocHelpers::SlotData<void(bool, int, int, bool)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 29 }, { QMetaType::Int, 30 }, { QMetaType::Int, 31 }, { QMetaType::Bool, 32 },
        }}),
        // Slot 'onElimination'
        QtMocHelpers::SlotData<void(qint64, int)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 34 }, { QMetaType::Int, 21 },
        }}),
        // Slot 'onGameOver1VN'
        QtMocHelpers::SlotData<void(qint64, const QJsonArray &)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 36 }, { 0x80000000 | 15, 37 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OneVNWindow, qt_meta_tag_ZN11OneVNWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject OneVNWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11OneVNWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11OneVNWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11OneVNWindowE_t>.metaTypes,
    nullptr
} };

void OneVNWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OneVNWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onCreateRoomClicked(); break;
        case 1: _t->onJoinRoomClicked(); break;
        case 2: _t->onStartGameClicked(); break;
        case 3: _t->onLeaveRoomClicked(); break;
        case 4: _t->onAnswerClicked(); break;
        case 5: _t->onQuestionTimeout(); break;
        case 6: _t->updateTimer(); break;
        case 7: _t->onRoomCreated((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 8: _t->onRoomJoined((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->onRoomUpdate((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 10: _t->onGameStart1VN((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 11: _t->onQuestion1VNReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[6])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[7]))); break;
        case 12: _t->onAnswerResult1VN((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4]))); break;
        case 13: _t->onElimination((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 14: _t->onGameOver1VN((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *OneVNWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OneVNWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11OneVNWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int OneVNWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP

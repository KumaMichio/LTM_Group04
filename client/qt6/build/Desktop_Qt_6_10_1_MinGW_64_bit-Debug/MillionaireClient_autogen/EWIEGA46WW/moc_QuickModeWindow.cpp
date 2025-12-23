/****************************************************************************
** Meta object code from reading C++ file 'QuickModeWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QuickModeWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QuickModeWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15QuickModeWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto QuickModeWindow::qt_create_metaobjectdata<qt_meta_tag_ZN15QuickModeWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QuickModeWindow",
        "onAnswerClicked",
        "",
        "onLifelineClicked",
        "onTimeout",
        "onGameStart",
        "sessionId",
        "totalRounds",
        "onQuestionReceived",
        "round",
        "questionId",
        "content",
        "QJsonObject",
        "options",
        "difficulty",
        "onAnswerResult",
        "correct",
        "correctAnswer",
        "score",
        "gameOver",
        "onGameOver",
        "finalScore",
        "status",
        "win",
        "onLifelineResult",
        "remainingOptions",
        "removedOptions",
        "remaining"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onAnswerClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLifelineClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTimeout'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGameStart'
        QtMocHelpers::SlotData<void(qint64, int)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 6 }, { QMetaType::Int, 7 },
        }}),
        // Slot 'onQuestionReceived'
        QtMocHelpers::SlotData<void(qint64, int, qint64, const QString &, const QJsonObject &, const QString &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 6 }, { QMetaType::Int, 9 }, { QMetaType::LongLong, 10 }, { QMetaType::QString, 11 },
            { 0x80000000 | 12, 13 }, { QMetaType::QString, 14 },
        }}),
        // Slot 'onAnswerResult'
        QtMocHelpers::SlotData<void(qint64, int, bool, const QString &, int, bool)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 6 }, { QMetaType::Int, 9 }, { QMetaType::Bool, 16 }, { QMetaType::QString, 17 },
            { QMetaType::Int, 18 }, { QMetaType::Bool, 19 },
        }}),
        // Slot 'onGameOver'
        QtMocHelpers::SlotData<void(qint64, int, int, const QString &, bool)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 6 }, { QMetaType::Int, 21 }, { QMetaType::Int, 7 }, { QMetaType::QString, 22 },
            { QMetaType::Bool, 23 },
        }}),
        // Slot 'onLifelineResult'
        QtMocHelpers::SlotData<void(qint64, int, const QStringList &, const QStringList &, int)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 6 }, { QMetaType::Int, 9 }, { QMetaType::QStringList, 25 }, { QMetaType::QStringList, 26 },
            { QMetaType::Int, 27 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QuickModeWindow, qt_meta_tag_ZN15QuickModeWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QuickModeWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15QuickModeWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15QuickModeWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15QuickModeWindowE_t>.metaTypes,
    nullptr
} };

void QuickModeWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QuickModeWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onAnswerClicked(); break;
        case 1: _t->onLifelineClicked(); break;
        case 2: _t->onTimeout(); break;
        case 3: _t->onGameStart((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->onQuestionReceived((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[6]))); break;
        case 5: _t->onAnswerResult((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[6]))); break;
        case 6: _t->onGameOver((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[5]))); break;
        case 7: _t->onLifelineResult((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5]))); break;
        default: ;
        }
    }
}

const QMetaObject *QuickModeWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QuickModeWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15QuickModeWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int QuickModeWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP

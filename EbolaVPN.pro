QT       += core gui
QT       += network
CONFIG -= embed_manifest_exe
#QT +=      core6compat
RESOURCES += \
    resources.qrc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32 {
    QMAKE_MANIFEST = $$PWD/app.manifest
}
CONFIG += c++17
QMAKE_CXXFLAGS_RELEASE -= -O3
win32:RC_ICONS += $$PWD/icon.ico
SOURCES += \
    main.cpp \
    ebolavpn.cpp \
    openvpn.cpp \
    sslsocket.cpp \
    tlsserver.cpp

HEADERS += \
    ebolavpn.h \
    openvpn.h \
    sslsocket.h \
    tlsserver.h

FORMS += \
    ebolavpn.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

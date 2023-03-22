QT       += core gui
QT       += network
#QT +=      core6compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_MANIFEST = $$PWD/app.manifest
CONFIG += c++11
QMAKE_CXXFLAGS_RELEASE -= -O3
win32:RC_ICONS += $$PWD/icon.ico
#LIBS += "C:\Program Files\OpenSSL-Win64\lib\VC\static\libcrypto64MD.lib"
#LIBS += "C:\Program Files\OpenSSL-Win64\lib\VC\static\libssl64MD.lib"
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

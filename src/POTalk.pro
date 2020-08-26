QT += multimedia concurrent charts quick

CONFIG += c++17

CONFIG(debug, debug|release) {
    #debug config goes here
} else {
    #release config goes here
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Disable all APIs deprecated before Qt 6.0.0
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

RC_ICONS = ../img/icon.ico

HEADERS += \
    audioin.h \
    audioout.h \
    socket.h \
    waveformprovider.h

SOURCES += \
    main.cpp \
    audioin.cpp \
    audioout.cpp \
    socket.cpp \
    waveformprovider.cpp

RESOURCES += qml/qml.qrc

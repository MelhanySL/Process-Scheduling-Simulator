QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    resources/configurationwindow.cpp \
    resources/main.cpp \
    resources/mainwindow.cpp \
    resources/process.cpp \
    resources/scheduler.cpp \
    resources/simulationwindow.cpp

HEADERS += \
    headers/configurationwindow.h \
    headers/mainwindow.h \
    headers/process.h \
    headers/scheduler.h \
    headers/simulationwindow.h

FORMS += \
    screens/configurationwindow.ui \
    screens/mainwindow.ui \
    screens/simulationwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

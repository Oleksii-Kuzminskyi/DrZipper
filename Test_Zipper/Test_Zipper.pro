QT = core gui widgets concurrent testlib

CONFIG += c++17 cmdline

QMAKE_CXXFLAGS_DEBUG += -fsanitize=address
QMAKE_LFLAGS_DEBUG += -fsanitize=address

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        ../zipping/Resource.cpp

INCLUDEPATH += ../zipping/

HEADERS += ../zipping/Resource.h

LIBS += -lzip


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

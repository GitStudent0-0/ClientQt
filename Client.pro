QT = core
QT += network xml
CONFIG += c++17 cmdline
CONFIG += link_pkgconfig
PKGCONFIG += log4cplus

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Client.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
  ../Загрузки/Telegram Desktop/doc_2026-10-02_08-57-27.clang-format

HEADERS += \
  Client.h

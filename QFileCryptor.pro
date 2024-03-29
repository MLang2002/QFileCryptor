QT       += core gui gui-private
# gui-private is a secret in Qt, which means you cannot get source code from Qt Assist.
# QZipReader and QZipWriter are included in gui-private, which can be used in compress a file/directory.

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = Safe.ico         # 设置图标，必须放在工程路径下
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fileencryption.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    fileencryption.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

include(QAESEncryption/QAESEncryption.pri)
INCLUDEPATH += $$PWD/QAESEncryption/

include(utility/utility.pri)
INCLUDEPATH += $$PWD/utility/

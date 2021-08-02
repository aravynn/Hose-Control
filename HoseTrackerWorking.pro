QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AddEditCustomer.cpp \
    AddEditTemplate.cpp \
    addedithose.cpp \
    addfittingtemplate.cpp \
    adminpage.cpp \
    company.cpp \
    companysql.cpp \
    csvimport.cpp \
    customercenter.cpp \
    databutton.cpp \
    fittingtemplate.cpp \
    graphview.cpp \
    homepage.cpp \
    hose.cpp \
    hosecenter.cpp \
    hosetemplates.cpp \
    hosetest.cpp \
    htdirectorycontrol.cpp \
    main.cpp \
    hosetracker.cpp \
    pdfcreator.cpp \
    sqldatabase.cpp \
    templatecenter.cpp \
    testcenter.cpp \
    testresult.cpp \
    timeconvert.cpp \
    pdf-vulcaneware/metrics.cpp \
    pdf-vulcaneware/pdf.cpp \
    user.cpp \
    webconnector.cpp

HEADERS += \
    TrackerEnums.h \
    TrackerStructs.h \
    company.h \
    databutton.h \
    fittingtemplate.h \
    graphview.h \
    hose.h \
    hosetemplates.h \
    hosetest.h \
    hosetracker.h \
    htdirectorycontrol.h \
    pdfcreator.h \
    sqldatabase.h \
    testthread.h \
    timeconvert.h \
    pdf-vulcaneware/metrics.h \
    pdf-vulcaneware/pdf.h \
    user.h \
    webconnector.h

FORMS += \
    hosetracker.ui

RC_ICONS = hc_icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/esi-dll/ -lESI-USB-API

INCLUDEPATH += $$PWD/esi-dll
DEPENDPATH += $$PWD/esi-dll

RESOURCES += \
    pdf_images.qrc

win32: LIBS += -L$$PWD/mysql/lib/vs14/ -lmysqlcppconn8

INCLUDEPATH += $$PWD/mysql/lib/vs14
DEPENDPATH += $$PWD/mysql/lib/vs14

INCLUDEPATH += $$PWD/mysql/lib/
DEPENDPATH += $$PWD/mysql/lib/

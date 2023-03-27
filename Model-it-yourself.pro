QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Essentials/GUI/GUIelements/backgroundtaskvisualization.cpp \
    Essentials/GUI/GUIpages/backgroundtaskspage.cpp \
    Essentials/GUI/GUIpages/dataseteditorpage.cpp \
    Essentials/GUI/GUIpages/tabwidgetpage.cpp \
    Essentials/corecontroller.cpp \
    Essentials/dataset_tablemodel.cpp \
    Essentials/logrelay.cpp \
    Essentials/logsmanager.cpp \
    Implementations/csvdataimportandexportmanager.cpp \
    main.cpp

HEADERS += \
    Essentials/GUI/GUIelements/backgroundtaskvisualization.h \
    Essentials/GUI/GUIpages/backgroundtaskspage.h \
    Essentials/GUI/GUIpages/dataseteditorpage.h \
    Essentials/GUI/GUIpages/tabwidgetpage.h \
    Essentials/corecontroller.h \
    Essentials/dataset_tablemodel.h \
    Essentials/logrelay.h \
    Essentials/logsmanager.h \
    Implementations/csvdataimportandexportmanager.h \
    Interfaces/backgroundtaskenabledobject.h \
    Interfaces/dataimportandexportmanager.h \
    Interfaces/logconnectedobject.h

FORMS += \
    Essentials/corecontroller.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    LICENSE \
    README.md

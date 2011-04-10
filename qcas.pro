QT += xml
SOURCES += gui/WizardMatrix.cpp \
    gui/WizardEquation.cpp \
    MainWindow.cpp \
    main.cpp \
    gui/WizardCatalog.cpp \
    CasManager.cpp \
    gui/qtmmlwidget.cpp \
    gui/FormalSheet.cpp \
    gui/FormalLine.cpp \
    gui/FormalLineWidgets.cpp \
    gui/spreadsheet.cpp \
    gui/CentralTabWidget.cpp
HEADERS += gui/WizardMatrix.h \
    gui/WizardEquation.h \
    MainWindow.h \
    gui/WizardCatalog.h \
    CasManager.h \
    gui/qtmmlwidget.h \
    gui/FormalSheet.h \
    gui/FormalLine.h \
    gui/FormalLineWidgets.h \
    gui/spreadsheet.h \
    gui/CentralTabWidget.h
LIBS += -lgiac \
    -lgmp
OTHER_FILES += 
RESOURCES += qcas.qrc

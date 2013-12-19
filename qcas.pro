# #####################################################################
# build of qcas against  libgiac
# #####################################################################
QT += core \
     gui \
     xml
TARGET = qcas
TEMPLATE = app
QMAKE_CFLAGS_DEBUG += -DHAVE_CONFIG_H  -fno-strict-aliasing -Wno-unused-parameter
QMAKE_CFLAGS_RELEASE += -DHAVE_CONFIG_H  -fno-strict-aliasing -Wno-unused-parameter

QMAKE_CXXFLAGS_DEBUG += -DHAVE_CONFIG_H \
    -fno-strict-aliasing \
    -Wno-unused-parameter \
    -DGIAC_GENERIC_CONSTANTS -fpermissive
QMAKE_CXXFLAGS_RELEASE += -DHAVE_CONFIG_H \
    -fno-strict-aliasing \
    -Wno-unused-parameter \
    -DGIAC_GENERIC_CONSTANTS -fpermissive
DEPENDPATH += . \
    qt 
 INCLUDEPATH += . \
    qt \
    giac 
win32{
    CONFIG+=rtti
    QMAKE_CXXFLAGS+=-D__MINGW_H
    QMAKE_LFLAGS+=-static-libgcc -static-libstdc++
    INCLUDEPATH+=pthread
    win32:LIBS+=-lgiac -lntl -lgsl -lgslcblas -lmpfr -lgmp -lpthread
}
unix{
    LIBS += -ldl -lgiac -lgmp
}


# Input
HEADERS += qt/output.h \
    qt/MainWindow.h \
    qt/CasManager.h \
    qt/geometry.h \
    qt/config.h \
    qt/gui/WizardMatrix.h \
    qt/gui/WizardEquation.h \
    qt/gui/WizardCatalog.h \
    qt/gui/WizardAlgo.h \
    qt/gui/spreadsheet.h \
    qt/gui/qtmmlwidget.h \
    qt/gui/FormalSheet.h \
    qt/gui/FormalLineWidgets.h \
    qt/gui/FormalLine.h \
    qt/gui/CentralTabWidget.h \
#Header for giac
           giac/alg_ext.h \
           giac/cocoa.h \
           giac/config.h \
           giac/csturm.h \
           giac/derive.h \
           giac/desolve.h \
           giac/dispatch.h \
           giac/ezgcd.h \
           giac/first.h \
           giac/fraction.h \
           giac/gauss.h \
           giac/gausspol.h \
           giac/gen.h \
           giac/giac.h \
           giac/giacintl.h \
           giac/giacPCH.h \
           giac/global.h \
           giac/gmp_replacements.h \
           giac/help.h \
           giac/identificateur.h \
           giac/ifactor.h \
           giac/index.h \
           giac/input_lexer.h \
           giac/input_parser.h \
           giac/intg.h \
           giac/intgab.h \
           giac/isom.h \
           giac/lexer.h \
           giac/lexer_tab_int.h \
           giac/lin.h \
           giac/maple.h \
           giac/mathml.h \
           giac/misc.h \
           giac/modfactor.h \
           giac/modpoly.h \
           giac/monomial.h \
           giac/moyal.h \
           giac/pari.h \
           giac/path.h \
           giac/permu.h \
           giac/plot.h \
           giac/plot3d.h \
           giac/poly.h \
           giac/prog.h \
           giac/quater.h \
           giac/risch.h \
           giac/rpn.h \
           giac/series.h \
           giac/solve.h \
           giac/static.h \
           giac/static_extern.h \
           giac/static_help.h \
           giac/static_lexer.h \
           giac/static_lexer_.h \
           giac/subst.h \
           giac/sym2poly.h \
           giac/symbolic.h \
           giac/tex.h \
           giac/threaded.h \
           giac/ti89.h \
           giac/tinymt32.h \
           giac/tinymt32_license.h \
           giac/TmpFGLM.H \
           giac/TmpLESystemSolver.H \
           giac/unary.h \
           giac/usual.h \
           giac/vecteur.h \
           giac/vector.h \
    qt/gui/prefdialog.h \
    qt/gui/plotfunctiondialog.h \
    pthread-win32/semaphore.h \
    pthread-win32/sched.h \
    pthread-win32/pthread.h

SOURCES += qt/output.cpp \ # qt/Window.cpp \
    qt/MainWindow.cpp \
    qt/main.cpp \
    qt/config.cpp \
    qt/CasManager.cpp \
    qt/gui/WizardMatrix.cpp \
    qt/gui/WizardEquation.cpp \
    qt/gui/WizardCatalog.cpp \
    qt/gui/WizardAlgo.cpp \
    qt/gui/spreadsheet.cpp \
    qt/gui/qtmmlwidget.cpp \
    qt/gui/FormalSheet.cpp \
    qt/gui/FormalLineWidgets.cpp \
    qt/gui/FormalLine.cpp \
    qt/gui/CentralTabWidget.cpp \ # qt/gui/src/qtmmlwidget.cpp \
    qt/gui/prefdialog.cpp \
    qt/geometry.cpp \
    qt/gui/plotfunctiondialog.cpp
#
OTHER_FILES += \
    qt/doc/fr/menu.html \
    qt/doc/fr/memento.html \
    qt/doc/fr/memento_algo.html \
    qt/doc/fr/xcasmenu.html \
    qt/doc/en/menu.html \
    qt/doc/en/memento.html \
    qt/doc/en/xcasmenu.html \
    qt/doc/el/menu.html \
    qt/doc/el/memento.html \
    qt/doc/el/xcasmenu.html \
    qt/doc/es/menu.html \
    qt/doc/es/memento.html \
    qt/doc/es/xcasmenu.html \
    qt/doc/zh/menu.html \
    qt/doc/zh/memento.html \
    qt/doc/zh/xcasmenu.html \
    qt/doc/de/menu.html \
    qt/doc/de/memento.html \
    qt/doc/de/xcasmenu.html \
    qt/images/stop.png \
    qt/images/spreadsheet.png \
    qt/images/segment.png \
    qt/images/programming.png \
    qt/images/process-stop.png \
    qt/images/previous.png \
    qt/images/point.png \
    qt/images/open.png \
    qt/images/next.png \
    qt/images/new.png \
    qt/images/midpoint.png \
    qt/images/matrix.png \
    qt/images/line.png \
    qt/images/inter.png \
    qt/images/help.png \
    qt/images/halfline.png \
    qt/images/formal.png \
    qt/images/f1.png \
    qt/images/exit.png \
    qt/images/evaluate.png \
    qt/images/evaluate-buisy.png \
    qt/images/equation.png \
    qt/images/edit-undo.png \
    qt/images/edit-redo.png \
    qt/images/edit-paste.png \
    qt/images/edit-find.png \
    qt/images/home.png \
    qt/images/edit-cut.png \
    qt/images/edit-copy.png \
    qt/images/document-save-as.png \
    qt/images/document-save.png \
    qt/images/document-new.png \
    qt/images/circle3pt.png \
    qt/images/circle2pt.png \
    qt/images/book.png \
    qt/images/add.png \
    qt/images/cas.png \
    qt/images/ampoule.png \
    qt/images/general.png \
    qt/images/french.png \
    qt/images/english.png \
    qt/images/spain.png \
    qt/images/greece.png \
    qt/images/china.png \
    qt/images/pointxy.png \
    qt/images/zoom-out.png \
    qt/images/zoom-in.png \
    qt/images/ortho.png \
    qt/images/source.png \
    qt/images/tex.png \
    qt/images/circleRadius.png \
    qt/images/right.png \
    qt/images/select.png \
    qt/images/function.png \
    qt/images/configure.png \
    qt/images/bezier.png \
    qt/images/vector.png \
    qt/images/regularpolygon.png \
    qt/images/polygon.png \
    qt/images/perpenbisector.png \
    qt/images/parallel.png \
    qt/images/openpolygon.png \
    qt/images/bot.png \
    qt/images/bisector.png \
    qt/images/perpendicular.png \
    qt/images/move.png \
    qt/images/translation.png \
    qt/images/reflection.png \
    qt/images/point-symmetry.png \
    qt/images/arc3pt.png \
    qt/images/rotation.png \
    qt/images/delete.png \
    qt/images/tangent.png \
    qt/images/similarity.png \
    qt/images/linebyequation.png \
    qt/images/angle.png \
    qt/images/numericcursor.png \
    qt/images/formalcursor.png \
    qt/images/svg.png \
    qt/images/png.png \
    qt/images/homothety.png \
    pthread-win32/libpthreadGC2.a \
    qt/images/mathml.png \
    qt/aide_cas \
    qt/images/icon.png \
    android/AndroidManifest.xml \
    android/version.xml \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/res/values-ro/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-de/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-id/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-fa/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-ms/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values-pl/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-es/strings.xml
RESOURCES += qt/qcas.qrc
TRANSLATIONS = qt/lang/qcas_en.ts qt/lang/qcas_es.ts qt/lang/qcas_el.ts qt/lang/qcas_zh.ts
FORMS=MainWindow.ui


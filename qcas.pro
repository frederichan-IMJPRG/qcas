# #####################################################################
# Automatically generated by qmake (2.01a) ven. mars 9 22:27:33 2012
# #####################################################################
QT += core \
     gui \
     xml
TARGET = qcas
TEMPLATE = app
QMAKE_CXXFLAGS_DEBUG += -DHAVE_CONFIG_H \
    -DUSE_GMP_REPLACEMENTS \
    -fno-strict-aliasing \
    -Wno-unused-parameter \
    -DGIAC_GENERIC_CONSTANTS -fpermissive
QMAKE_CXXFLAGS_RELEASE += -DHAVE_CONFIG_H \
    -DUSE_GMP_REPLACEMENTS \
    -fno-strict-aliasing \
    -Wno-unused-parameter \
    -DGIAC_GENERIC_CONSTANTS -fpermissive
DEPENDPATH += . \
    qt \
    giac \
    libtommath
INCLUDEPATH += . \
    qt \
    giac \
    libtommath
LIBS += -ldl

# Input
HEADERS += qt/output.h \
    qt/MainWindow.h \
    qt/CasManager.h \
    qt/geometry.h \
    qt/config.h \
    qt/gui/WizardMatrix.h \
    qt/gui/WizardEquation.h \
    qt/gui/WizardCatalog.h \
    qt/gui/spreadsheet.h \
    qt/gui/qtmmlwidget.h \
    qt/gui/FormalSheet.h \
    qt/gui/FormalLineWidgets.h \
    qt/gui/FormalLine.h \
    qt/gui/CentralTabWidget.h \
 # qt/gui/src/qtmmlwidget.h \
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
#Hearder for libtommath
           libtommath/tommath.h \
           libtommath/tommath_class.h \
           libtommath/tommath_superclass.h\
#    qt/gui/Interactive2d.h \
    qt/gui/prefdialog.h \
    qt/gui/plotfunctiondialog.h

SOURCES += qt/output.cpp \ # qt/Window.cpp \
    qt/MainWindow.cpp \
    qt/main.cpp \
    qt/config.cpp \
    qt/CasManager.cpp \
    qt/gui/WizardMatrix.cpp \
    qt/gui/WizardEquation.cpp \
    qt/gui/WizardCatalog.cpp \
    qt/gui/spreadsheet.cpp \
    qt/gui/qtmmlwidget.cpp \
    qt/gui/FormalSheet.cpp \
    qt/gui/FormalLineWidgets.cpp \
    qt/gui/FormalLine.cpp \
    qt/gui/CentralTabWidget.cpp \ # qt/gui/src/qtmmlwidget.cpp \
#Sources for giac
    giac/alg_ext.cc \
           giac/cocoa.cc \
           giac/csturm.cc \
           giac/derive.cc \
           giac/desolve.cc \
           giac/ezgcd.cc \
           giac/firstcc.cc \
           giac/gauss.cc \
           giac/gausspol.cc \
           giac/gen.cc \
           giac/global.cc \
           giac/help.cc \
           giac/identificateur.cc \
           giac/ifactor.cc \
           giac/index.cc \
           giac/input_lexer.cc \
           giac/input_parser.cc \
           giac/intg.cc \
           giac/intgab.cc \
           giac/isom.cc \
           giac/lin.cc \
           giac/maple.cc \
           giac/mathml.cc \
           giac/misc.cc \
           giac/modfactor.cc \
           giac/modpoly.cc \
           giac/moyal.cc \
           giac/pari.cc \
           giac/permu.cc \
           giac/plot.cc \
           giac/plot3d.cc \
           giac/prog.cc \
           giac/quater.cc \
           giac/risch.cc \
           giac/rpn.cc \
           giac/series.cc \
           giac/solve.cc \
           giac/subst.cc \
           giac/sym2poly.cc \
           giac/symbolic.cc \
           giac/tex.cc \
           giac/threaded.cc \
           giac/ti89.cc \
           giac/tinymt32.cc \
           giac/TmpFGLM.C \
           giac/TmpLESystemSolver.C \
           giac/unary.cc \
           giac/usual.cc \
           giac/vecteur.cc \
#Sources for libtommath
           libtommath/bn_error.c \
           libtommath/bn_fast_mp_invmod.c \
           libtommath/bn_fast_mp_montgomery_reduce.c \
           libtommath/bn_fast_s_mp_mul_digs.c \
           libtommath/bn_fast_s_mp_mul_high_digs.c \
           libtommath/bn_fast_s_mp_sqr.c \
           libtommath/bn_mp_2expt.c \
           libtommath/bn_mp_abs.c \
           libtommath/bn_mp_add.c \
           libtommath/bn_mp_add_d.c \
           libtommath/bn_mp_addmod.c \
           libtommath/bn_mp_and.c \
           libtommath/bn_mp_clamp.c \
           libtommath/bn_mp_clear.c \
           libtommath/bn_mp_clear_multi.c \
           libtommath/bn_mp_cmp.c \
           libtommath/bn_mp_cmp_d.c \
           libtommath/bn_mp_cmp_mag.c \
           libtommath/bn_mp_cnt_lsb.c \
           libtommath/bn_mp_copy.c \
           libtommath/bn_mp_count_bits.c \
           libtommath/bn_mp_div.c \
           libtommath/bn_mp_div_2.c \
           libtommath/bn_mp_div_2d.c \
           libtommath/bn_mp_div_3.c \
           libtommath/bn_mp_div_d.c \
           libtommath/bn_mp_dr_is_modulus.c \
           libtommath/bn_mp_dr_reduce.c \
           libtommath/bn_mp_dr_setup.c \
           libtommath/bn_mp_exch.c \
           libtommath/bn_mp_expt_d.c \
           libtommath/bn_mp_exptmod.c \
           libtommath/bn_mp_exptmod_fast.c \
           libtommath/bn_mp_exteuclid.c \
           libtommath/bn_mp_fread.c \
           libtommath/bn_mp_fwrite.c \
           libtommath/bn_mp_gcd.c \
           libtommath/bn_mp_get_int.c \
           libtommath/bn_mp_grow.c \
           libtommath/bn_mp_init.c \
           libtommath/bn_mp_init_copy.c \
           libtommath/bn_mp_init_multi.c \
           libtommath/bn_mp_init_set.c \
           libtommath/bn_mp_init_set_int.c \
           libtommath/bn_mp_init_size.c \
           libtommath/bn_mp_invmod.c \
           libtommath/bn_mp_invmod_slow.c \
           libtommath/bn_mp_is_square.c \
           libtommath/bn_mp_jacobi.c \
           libtommath/bn_mp_karatsuba_mul.c \
           libtommath/bn_mp_karatsuba_sqr.c \
           libtommath/bn_mp_lcm.c \
           libtommath/bn_mp_lshd.c \
           libtommath/bn_mp_mod.c \
           libtommath/bn_mp_mod_2d.c \
           libtommath/bn_mp_mod_d.c \
           libtommath/bn_mp_montgomery_calc_normalization.c \
           libtommath/bn_mp_montgomery_reduce.c \
           libtommath/bn_mp_montgomery_setup.c \
           libtommath/bn_mp_mul.c \
           libtommath/bn_mp_mul_2.c \
           libtommath/bn_mp_mul_2d.c \
           libtommath/bn_mp_mul_d.c \
           libtommath/bn_mp_mulmod.c \
           libtommath/bn_mp_n_root.c \
           libtommath/bn_mp_neg.c \
           libtommath/bn_mp_or.c \
           libtommath/bn_mp_prime_fermat.c \
           libtommath/bn_mp_prime_is_divisible.c \
           libtommath/bn_mp_prime_is_prime.c \
           libtommath/bn_mp_prime_miller_rabin.c \
           libtommath/bn_mp_prime_next_prime.c \
           libtommath/bn_mp_prime_rabin_miller_trials.c \
           libtommath/bn_mp_prime_random_ex.c \
           libtommath/bn_mp_radix_size.c \
           libtommath/bn_mp_radix_smap.c \
           libtommath/bn_mp_rand.c \
           libtommath/bn_mp_read_radix.c \
           libtommath/bn_mp_read_signed_bin.c \
           libtommath/bn_mp_read_unsigned_bin.c \
           libtommath/bn_mp_reduce.c \
           libtommath/bn_mp_reduce_2k.c \
           libtommath/bn_mp_reduce_2k_l.c \
           libtommath/bn_mp_reduce_2k_setup.c \
           libtommath/bn_mp_reduce_2k_setup_l.c \
           libtommath/bn_mp_reduce_is_2k.c \
           libtommath/bn_mp_reduce_is_2k_l.c \
           libtommath/bn_mp_reduce_setup.c \
           libtommath/bn_mp_rshd.c \
           libtommath/bn_mp_set.c \
           libtommath/bn_mp_set_int.c \
           libtommath/bn_mp_shrink.c \
           libtommath/bn_mp_signed_bin_size.c \
           libtommath/bn_mp_sqr.c \
           libtommath/bn_mp_sqrmod.c \
           libtommath/bn_mp_sqrt.c \
           libtommath/bn_mp_sub.c \
           libtommath/bn_mp_sub_d.c \
           libtommath/bn_mp_submod.c \
           libtommath/bn_mp_to_signed_bin.c \
           libtommath/bn_mp_to_signed_bin_n.c \
           libtommath/bn_mp_to_unsigned_bin.c \
           libtommath/bn_mp_to_unsigned_bin_n.c \
           libtommath/bn_mp_toom_mul.c \
           libtommath/bn_mp_toom_sqr.c \
           libtommath/bn_mp_toradix.c \
           libtommath/bn_mp_toradix_n.c \
           libtommath/bn_mp_unsigned_bin_size.c \
           libtommath/bn_mp_xor.c \
           libtommath/bn_mp_zero.c \
           libtommath/bn_prime_tab.c \
           libtommath/bn_reverse.c \
           libtommath/bn_s_mp_add.c \
           libtommath/bn_s_mp_exptmod.c \
           libtommath/bn_s_mp_mul_digs.c \
           libtommath/bn_s_mp_mul_high_digs.c \
           libtommath/bn_s_mp_sqr.c \
           libtommath/bn_s_mp_sub.c \
           libtommath/bncore.c \
    qt/gui/prefdialog.cpp \
    qt/geometry.cpp \
    qt/gui/plotfunctiondialog.cpp
# -lgmp
OTHER_FILES += \ 
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
    qt/images/equation.png \
    qt/images/edit-undo.png \
    qt/images/edit-redo.png \
    qt/images/edit-paste.png \
    qt/images/edit-find.png \
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
    qt/images/general.png \
    qt/images/french.png \
    qt/images/english.png \
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
    qt/images/perpendicular.png
RESOURCES += qt/qcas.qrc

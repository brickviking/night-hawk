############################################################################
# NightHawk - By Jason Nunn(JsNO) - Sept 1996
#
############################################################################


DEFS = @DEFS@ @ADD_DEFS@ -DLIBEXEC_DIR="\"$(libexecdir)\""
# what a misnomer is X_LIBS? ;-)
CXXFLAGS = @CXXFLAGS@ $(X_CFLAGS)
LDFLAGS = $(X_LIBS)
LIBS = $(X_EXT_LIBS) @LIBS@


bin_PROGRAMS = nighthawk
noinst_PROGRAMS = @NIGHTHAWK_TOOLS@
EXTRA_PROGRAMS = ned xpm_view

EXTRA_DIST = \
defs.h \
icon_bm.h \
misc.h \
nighthawk_defs.h \
tdoor.h \
tdroid.h \
tdroid_1xx.h \
tdroid_2xx.h \
tdroid_3xx.h \
tdroid_4xx.h \
tdroid_5xx.h \
tdroid_6xx.h \
tdroid_7xx.h \
tdroid_8xx.h \
tdroid_9xx.h \
tedit_floor.h \
tentity.h \
tfloor.h \
tlaser.h \
tparadroid.h \
tpower_bay.h \
tship.h \
tship_demo.h \
scroll.inc

nighthawk_SOURCES = floor.cc misc.cc object.cc ship.cc nighthawk.cc
ned_SOURCES = floor.cc misc.cc ned.cc object.cc
xpm_view_SOURCES = misc.cc floor.cc object.cc xpm_view.cc
#tools: ned xpm_view

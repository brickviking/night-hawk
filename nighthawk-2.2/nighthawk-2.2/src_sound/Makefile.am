############################################################################
# nighthawk sound engine
############################################################################

DEFS = @DEFS@ $(ADD_DEFS) $(HOSTDEF)

libexec_PROGRAMS = @NIGHTHAWK_SOUND@
EXTRA_PROGRAMS = nighthawk_sound
EXTRA_DIST = COPYING.FunktrackerGOLD Make.common README README.FunktrackerGOLD
CLEANFILES = host_safe

nighthawk_sound_SOURCES = \
dsp_mixxer.c \
dsp_mixxer.h \
funkload.c \
funkload.h \
funktracker.c \
funktracker.h \
funktracker_defs.h \
nighthawk_sound.c \
nighthawk_sound.h
nighthawk_sound_DEPENDENCIES = host_safe


-include $(srcdir)/Make.common

host_safe:
	@if [ $(HOST_SAFE) = '1' ]; then \
	  echo "host is safe" >host_safe; \
	else \
	  echo "***********************************************"; \
	  echo "This is not an INTEL based machine. As the Funktracker"; \
	  echo "format is in Intel byte sex (and i haven't yet made"; \
	  echo "provisions for other processors), trying to play"; \
	  echo "funktracker songs on this machine will be dangerous."; \
	  echo ""; \
	  echo "Refer to INSTALL file."; \
	  echo "***********************************************"; \
	  false; \
	fi

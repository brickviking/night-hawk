############################################################################
# NightHawk - By Jason Nunn(JsNO) - Sept 1996
#
############################################################################

#INSTALL_DIR     = .
PREFIX           = debian/tmp
INSTALL_DIR      = /usr/games
INSTALL_LIB_DIR  = /usr/lib/games/nighthawk
SCORES           = /var/lib/games/nighthawk.scores

############################################################################
VERSION         = 1.0
CC		= g++
OPTIONS 	= -Wall -O2 -pipe \
                        -DINSTALL_DIR='"$(INSTALL_DIR)"' \
                        -DINSTALL_LIB_DIR='"$(INSTALL_LIB_DIR)"' \
                        -DSCORES='"$(SCORES)"' \
			-DVERSION='"$(VERSION)"'

INCLUDE		=
LIBS		= -L/usr/X11R6/lib -lXpm -lX11
EVERYTHING	= $(OPTIONS) $(INCLUDE) $(LIBS)

###############################
all: nighthawk

tools: ned xpm_view

###############################
install:
	make all
	install -d $(PREFIX)$(INSTALL_LIB_DIR) $(PREFIX)$(INSTALL_DIR)
	install -o root -g games -m2755 nighthawk $(PREFIX)$(INSTALL_DIR)/nighthawk
	cp -R nighthawk_data/* $(PREFIX)$(INSTALL_LIB_DIR)
	make clean

uninstall:
	rm -rf  $(PREFIX)

###############################
OBJE		= misc.o floor.o object.o ned.o

ned: $(OBJE)
	$(CC) $(OBJE) $(EVERYTHING) -o ned
	strip ned

###############################
OBJV		= misc.o floor.o object.o xpm_view.o

xpm_view: $(OBJV)
	$(CC) $(OBJV) $(EVERYTHING) -o xpm_view
	strip xpm_view

###############################
OBJ		= misc.o floor.o object.o ship.o nighthawk.o

nighthawk: $(OBJ)
	$(CC) $(OBJ) $(EVERYTHING) -o nighthawk
	strip nighthawk

###############################
clean:
	rm -f *.o ned xpm_view nighthawk core

###############################

INCLUDE_FILES	= *.h

misc.o: misc.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c misc.cc

floor.o: floor.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c floor.cc

object.o: object.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c object.cc

ship.o: ship.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c ship.cc

ned.o: ned.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c ned.cc

xpm_view.o: xpm_view.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c xpm_view.cc

nighthawk.o: nighthawk.cc $(INCLUDE_FILES)
	$(CC) $(OPTIONS) -c nighthawk.cc

include ../config.user

MKPROTOS	= util/mkprotos2

######################################################################
all:
	${MAKE} protos
	${MAKE} nighthawk

######################################################################
prep:
	${MAKE} all
	cp -f nighthawk ..
	${MAKE} clean

######################################################################
DEPS_LIB	= xpm_loader.cc misc.cc opengl.cc bm_loader.cc \
		score.cc intro.cc game.cc main.cc

protos:
	(cd util ; ${MAKE})
	$(MKPROTOS) $(DEPS_LIB) > protos.p

######################################################################
OBJS		= xpm_loader.o misc.o opengl.o bm_loader.o score.o intro.o \
		power_bay.o door.o floor.o entity.o laser.o droid.o \
		paradroid.o droid_xxx.o ship.o \
		game.o main.o

nighthawk: $(OBJS)
	$(CPP) $(CC_FLAGS) $(OBJS) -o nighthawk $(LIB_FLAGS)
	strip nighthawk

######################################################################
install:
	mkdir $(INSTALL_DIR)
	cp -f nighthawk $(INSTALL_DIR)
	cp -faR ../data $(INSTALL_DIR)

remove:
	rm -r $(INSTALL_DIR)

######################################################################
clean:
	rm -f ts *.o *.p nighthawk test
	(cd util ; ${MAKE} clean)

######################################################################
%.o: %.c
	$(CC) $(CC_FLAGS) -o $@ -c $<

%.o: %.cc
	$(CPP) $(CC_FLAGS) -o $@ -c $<

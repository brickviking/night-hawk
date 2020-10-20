include ../config.user

######################################################################
all:
	make nighthawk

######################################################################
prep:
	make all
	cp -f nighthawk ..
	cp -f ned ..
	make clean

######################################################################
OBJS_NH		= \
	linked_data.o \
	misc.o \
	scores.o \
	sprite_loader.o \
	opengl.o \
	xpm_loader.o \
	png_loader.o \
	openal.o \
	power_bay.o \
	door.o \
	floor.o \
	entity.o \
	laser.o \
	droid.o \
	droid_xxx.o \
	paradroid.o \
	ship.o \
	intro.o \
	game.o \
	main.o

nighthawk: $(OBJS_NH)
	$(CPP) $(CC_FLAGS) $(OBJS_NH) -o nighthawk $(LIB_FLAGS)
#	strip nighthawk

######################################################################
OBJS_NED	= \
	linked_data.o \
	misc.o \
	scores.o \
	sprite_loader.o \
	opengl.o \
	xpm_loader.o \
	png_loader.o \
	openal.o \
	power_bay.o \
	door.o \
	floor.o \
	edit_floor.o \
	ned.o

ned: $(OBJS_NED)
	$(CPP) $(CC_FLAGS) $(OBJS_NED) -o ned $(LIB_FLAGS)
#	strip ned

######################################################################
install:
	mkdir $(INSTALL_DIR)
	cp -f nighthawk $(INSTALL_DIR)
	cp -faR ../data $(INSTALL_DIR)

remove:
	rm -r $(INSTALL_DIR)

######################################################################
clean:
	rm -f *.o nighthawk ned

######################################################################
%.o: %.c
	$(CC) $(CC_FLAGS) -o $@ -c $<

%.o: %.cc
	$(CPP) $(CC_FLAGS) -o $@ -c $<

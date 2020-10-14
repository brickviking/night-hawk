GCC=gcc
CFLAGS=-Wall -O2

all:
	${MAKE} mkprotos2

OBJ	= mkprotos2.o

mkprotos2: $(OBJ)
	$(GCC) $(CFLAGS) -o mkprotos2 $(OBJ)
	strip mkprotos2

clean:
	rm -f *.o core mkprotos2

%.o: %.c
	$(GCC) $(CFLAGS) -o $@ -c $<

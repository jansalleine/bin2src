GCC = gcc
FLAGS = -Wall -v
FLAGS_STATIC = -static
DEBUG?=
RM = rm -f
CP = cp -v -f

OBJECTS=bin2src.c bin2src.h

all: bin2src

bin2src.o: $(OBJECTS)
	$(GCC) $(FLAGS) $(DEBUG) -c -o $@ $<
	@echo $(OBJECTS)

bin2src: bin2src.o
	$(GCC) $(FLAGS) $(DEBUG) -o $@ $<
	$(CP) $@ ../bin/

clean:
	$(RM) bin2src bin2src.o
	$(RM) ../bin/bin2src

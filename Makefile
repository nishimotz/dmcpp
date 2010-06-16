CC=g++

L1= `pkg-config --cflags --libs opencv` 
L2= `libjulius-config --cflags --libs` 
L3= `libsent-config --cflags --libs` 
CFLAGS= -g -O2

all: dm gram

dm: dm.cpp
	$(CC) $(CFLAGS) -o dm dm.cpp $(L1) $(L2) $(L3)

gram: gram.txt
	rake

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) dm

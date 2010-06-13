CC=g++
CFLAGS=-g -O2 

CPPFLAGS= -I/usr/local/include
LDFLAGS= -L/usr/local/lib -ljulius -ldl -lpthread -lsent -lasound -lz -lm 

all: dm gram

dm: dm.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o dm dm.cpp $(LDFLAGS)

gram: gram.txt
	rake

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) dm

CC=g++

L1= `pkg-config --cflags --libs opencv` 
L2= `libjulius-config --cflags --libs` 
L3= `libsent-config --cflags --libs` 
CFLAGS= -g -O2

all: dm gram adinpost.jpi

dm: dm.cpp application.o
	$(CC) $(CFLAGS) -o dm dm.cpp $(L1) $(L2) $(L3) application.o

application.o: application.cpp
	$(CC) $(CFLAGS) -c application.cpp 

gram: gram.txt
	rake

adinpost.jpi: adinpost.cpp
	$(CC) $(CFLAGS) -shared -o adinpost.jpi adinpost.cpp $(L2) $(L3)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) dm

CC=g++

C1= `pkg-config --cflags opencv` 
C2= `libjulius-config --cflags` 
C3= `libsent-config --cflags` 
L1= `pkg-config --cflags --libs opencv` 
L2= `libjulius-config --cflags --libs` 
L3= `libsent-config --cflags --libs` 
CFLAGS= -g -O2
OBJS = application.o facerecog.o sprecog.o

all: dm gram adinpost.jpi

dm: dm.cpp $(OBJS)
	$(CC) $(CFLAGS) -o dm dm.cpp $(L1) $(L2) $(L3) $(OBJS)

application.o: application.cpp application.h
	$(CC) $(CFLAGS) -c application.cpp 

facerecog.o: facerecog.cpp facerecog.h
	$(CC) $(CFLAGS) -c facerecog.cpp $(C1)

sprecog.o: sprecog.cpp sprecog.h
	$(CC) $(CFLAGS) -c sprecog.cpp $(C2) $(C3)

gram: gram.txt
	rake

adinpost.jpi: adinpost.cpp
	$(CC) $(CFLAGS) -shared -o adinpost.jpi adinpost.cpp $(L2) $(L3)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) dm

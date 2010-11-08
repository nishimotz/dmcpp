CC=g++

C1 = `pkg-config --cflags opencv` 
C2 = `libjulius-config --cflags` 
C3 = `libsent-config --cflags` 
L1 = `pkg-config --libs opencv` 
L2 = `libjulius-config --libs` 
L3 = `libsent-config --libs` 
CFLAGS = -g -O2
OBJS = application.o facerecog.o sprecog.o dm.o

all: dm gram adinpost.jpi

dm: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(L1) $(L2) $(L3) -o dm 

dm.o: dm.cpp
	$(CC) $(CFLAGS) -c dm.cpp $(C1) $(C2) $(C3)

application.o: application.cpp application.h
	$(CC) $(CFLAGS) -c application.cpp 

facerecog.o: facerecog.cpp facerecog.h
	$(CC) $(CFLAGS) $(C1) -c facerecog.cpp

sprecog.o: sprecog.cpp sprecog.h
	$(CC) $(CFLAGS) $(C2) $(C3) -c sprecog.cpp 

gram: gram.txt
	rake

adinpost.jpi: adinpost.cpp
	$(CC) $(CFLAGS) -shared -o adinpost.jpi adinpost.cpp $(L2) $(L3)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) dm

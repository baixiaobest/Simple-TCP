CC= g++
CFLAGS= -I.
OBJS= Receiver.o Header.o

target: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o target

Receiver.o: Receiver.cpp Header.h
	$(CC) -c $(CFLAGS) Receiver.cpp

Header.o: Header.h Header.cpp
	$(CC) -c $(CFLAGS) Header.cpp



clean:
	\rm *.o

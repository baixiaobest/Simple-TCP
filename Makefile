CC= g++
CFLAGS= -I. -g
OBJS_RECEIVER= Receiver.o Header.o GobackN.o
OBJS_SENDER= Sender.o Header.o GobackN.o

all: receiver sender

receiver: $(OBJS_RECEIVER)
	$(CC) $(CFLAGS) $(OBJS_RECEIVER) -o receiver

sender: $(OBJS_SENDER)
	$(CC) $(CFLAGS) $(OBJS_SENDER) -o sender

Receiver.o: Receiver.cpp Header.h
	$(CC) -c $(CFLAGS) Receiver.cpp

Sender.o: Sender.cpp Header.h
	$(CC) -c $(CFLAGS) Sender.cpp

Header.o: Header.h Header.cpp
	$(CC) -c $(CFLAGS) Header.cpp

GobackN.o: GobackN.h GobackN.cpp Header.h
	$(CC) -c $(CFLAGS) GobackN.cpp

clean:
	\rm *.o ./receiver ./sender

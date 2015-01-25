PORT=59449  # you probably want to change this

CFLAGS= -DPORT=$(PORT) -g -Wall 

all: dbserver dbclient testenv

dbclient: dbclient.o wrapsock.o filesync.o
	gcc ${CFLAGS} -o $@ $^

dbserver: dbserver.o filedata.o wrapsock.o filesync.o
	gcc ${CFLAGS} -o $@ $^

filesync.o: filesync.h message.h

filedata.o: filedata.c filedata.h message.h

wrapsock.o: wrapsock.c wrapsock.h

#testfiledata : testfiledata.o filedata.o
#	gcc ${CFLAGS} -o $@ $^

%.o : %.c
	gcc ${CFLAGS} -c $<

testenv: dbclient dbserver
	cp -f dbclient testenvironment/client
	cp -f dbserver testenvironment/server

clean: 
	rm *.o testfiledata 

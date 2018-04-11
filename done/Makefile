all:  week04 week05
	
error.o: error.c  

hashtable.o: hashtable.c

week04.o : week04.c

week04: hashtable.o error.o week04.o
	gcc week04.o hashtable.o error.o -lcheck -lm -lrt -pthread -lsubunit -o test-hashtable

week05: pps-launch-server pps-client-get pps-client-put

pps-launch-server: pps-launch-server.o

pps-client-get: pps-client-get.o

pps-client-put: pps-client-put.o

pps-launch-server.o: pps-launch-server.c

pps-client-get.o: pps-client-get.c

pps-client-put.o: pps-client-put.c


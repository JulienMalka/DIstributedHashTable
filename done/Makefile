CFLAGS= -g -std=c99
all:  week05
	#Reformat code of every file in folder
	astyle -A8 *.c

error.o: error.c

hashtable.o: hashtable.c

system.o: system.c

client.o: client.c

node.o: node.c

node_list.o: node_list.c

network.o: network.c

week05: pps-launch-server pps-client-get pps-client-put

pps-launch-server: pps-launch-server.o system.o network.o hashtable.o

pps-client-get: pps-client-get.o client.o node.o network.o system.o node_list.o

pps-client-put: pps-client-put.o client.o node.o network.o system.o node_list.o

pps-launch-server.o: pps-launch-server.c system.h network.h hashtable.h

pps-client-get.o: pps-client-get.c

pps-client-put.o: pps-client-put.c

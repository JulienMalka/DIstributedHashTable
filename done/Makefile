CFLAGS= -g -std=c99 -pedantic -Wall -Wextra

all:  week05 week08 week09
	#Reformat code of every file in folder
	#@astyle -A8 *.c

#error.o: error.c

#hashtable.o: hashtable.c

#system.o: system.c

#client.o: client.c

#node.o: node.c

#network-utils.o : network-utils.c

#pps-list-nodes.o : pps-list-nodes.c

#pps-client-find.o : pps-client-find.c

#util.o : util.c

#node_list.o: node_list.c

#network.o: network.c
#network-utils.o

week05: pps-launch-server pps-client-get pps-client-put test-hashtable

week08: pps-dump-node pps-list-nodes

week09: pps-client-cat pps-client-substr pps-client-find

test-hashtable : test-hashtable.c
	gcc -o test-hashtable test-hashtable.c node_list.o error.o node.o system.o -lcheck -lm -lrt -pthread -lsubunit

pps-launch-server: pps-launch-server.o system.o network.o hashtable.o network-utils.o util.o

pps-client-get: pps-client-get.o client.o node.o network.o system.o node_list.o util.o network-utils.o hashtable.o args.o

pps-client-put: pps-client-put.o client.o node.o network.o system.o node_list.o network-utils.o hashtable.o args.o util.o

pps-list-nodes : pps-list-nodes.o system.o network.o client.o node_list.o node.o network-utils.o hashtable.o args.o client.o util.o

pps-dump-node: pps-dump-node.o network.o system.o node.o hashtable.o network-utils.o args.o client.o node_list.o args.o util.o

pps-client-find : pps-client-find.o client.o node.o network.o system.o node_list.o util.o network-utils.o hashtable.o args.o util.o

pps-client-cat: pps-client-cat.o network.o client.o node_list.o system.o node.o network-utils.o hashtable.o args.o util.o

pps-client-substr: pps-client-substr.o network.o client.o node_list.o system.o node.o network-utils.o hashtable.o args.o util.o

#pps-client-cat.o: pps-client-cat.c

#pps-client-substr.o : pps-client-substr.c

#pps-dump-node.o: pps-dump-node.c

#pps-launch-server.o: pps-launch-server.c
#system.h network.h hashtable.h

#pps-client-get.o: pps-client-get.c

#pps-client-put.o: pps-client-put.c

CFLAGS = -g -pedantic -Wall -Wextra -std=c99

LDLIBS += -lcrypto -lcheck -lm -lrt -pthread -lsubunit

all:  week05 week08 week09 performance
	#Reformat code of every file in folder
	#@astyle -A8 *.c

week05: pps-launch-server pps-client-get pps-client-put test-hashtable

week08: pps-dump-node pps-list-nodes

week09: pps-client-cat pps-client-substr pps-client-find

test-hashtable: test-hashtable.o

pps-launch-server: pps-launch-server.o system.o network.o hashtable.o network-utils.o util.o ring.o node.o node_list.o

pps-client-get: pps-client-get.o client.o network.o system.o node_list.o util.o network-utils.o node.o hashtable.o args.o ring.o

pps-client-put: pps-client-put.o client.o node.o network.o system.o node_list.o network-utils.o hashtable.o args.o util.o ring.o

pps-list-nodes: pps-list-nodes.o system.o network.o client.o node_list.o node.o network-utils.o hashtable.o args.o client.o util.o ring.o

pps-dump-node: pps-dump-node.o network.o system.o node.o hashtable.o network-utils.o args.o client.o node_list.o args.o util.o ring.o

pps-client-find : pps-client-find.o client.o node.o network.o system.o node_list.o util.o network-utils.o hashtable.o args.o util.o ring.o

pps-client-cat: pps-client-cat.o network.o client.o node_list.o system.o node.o network-utils.o hashtable.o args.o util.o ring.o

pps-client-substr: pps-client-substr.o network.o client.o node_list.o system.o node.o network-utils.o hashtable.o args.o util.o ring.o

performance: scenario_1

scenario_1: scenario_1.o network.o client.o node_list.o system.o node.o network-utils.o hashtable.o args.o util.o ring.o

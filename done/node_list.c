#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "node_list.h"
#include <ctype.h>

node_list_t *node_list_new()
{
	node_list_t* new = malloc(sizeof(node_list_t));
	new->size = 0;
	new->nodes = calloc(32, sizeof(node_t));
	return new;
}


node_list_t *get_nodes()
{
	
	node_list_t* nodes = node_list_new();
	
    FILE* file = fopen(PPS_SERVERS_LIST_FILENAME, "r");
    
    if(file == NULL) {
        //gestion de l'erreur
    } 

	int current_char = fgetc(file);
	
	char ip[15];
	int index = 0;
	int port;
	
        while(!feof(file)) {

			if (isspace(current_char)){
				index = -1;
				fscanf(file, "%d", &port);
				
				node_t node;
				node_init(&node, ip, port, 0);
				node_list_add(nodes, node);
				
				while(current_char != '\n') current_char = fgetc(file);                              
			}
			
			ip[index] = current_char;
			
			index += 1;
			current_char = fgetc(file);
        }

	return nodes;
}

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "node_list.h"
#include <ctype.h>

node_list_t *node_list_new()
{
	node_list_t* new = malloc(sizeof(node_list_t));
	new->size = 0;
	new->allocated_size = 32;
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
	int port = 1;

        while(!feof(file)) {
			printf("current char = %c\n", current_char);

			if (isspace(current_char)){
				printf("Found space and index = %d\n", index);
				current_char = fgetc(file);
				fscanf(file, "%d", &port);
				
				//take the n first chars, discard else
				char real_ip[index];
				
				for (int i = 0; i < index; i++){
					real_ip[i] = ip[i];
				}
				
	//			real_ip = {'1','2','7','.','0','.','0','.','1'};

				node_t node;
				printf("current ip = %s\n", real_ip);
				node_init(&node, real_ip, port, 0);
				node_list_add(nodes, node);

				while(current_char != '\n') {
					current_char = fgetc(file);
				}
				index = 0;
	//			current_char = fgetc(file);
			}

			ip[index] = current_char;

			index += 1;
			current_char = fgetc(file);
        }

	return nodes;
}

error_code node_list_add(node_list_t *list, node_t node){

	list->size++;
	if(list->size > list->allocated_size){

		node_t* nodes = realloc(list->nodes, list->allocated_size+32);
		if(nodes==NULL){
			return ERR_NOMEM;
			}else{
		list->allocated_size+=32;
	}
	}

	list->nodes[list->size-1] = node;
	return ERR_NONE;

}



void node_list_free(node_list_t *list){

for(int i=0; i<list->size;i++){
	node_end(&list->nodes[i]);
	}

	free(list);
}

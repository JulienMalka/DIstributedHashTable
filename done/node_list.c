#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "node_list.h"
#include <ctype.h>



/**
 * @brief creates a new, empty, node_list_t
 * @return (a pointer to) the new list of nodes
 */
node_list_t *node_list_new()
{
    node_list_t* new = malloc(sizeof(node_list_t));
    new->size = 0;
    new->allocated_size = 32;
    new->nodes = calloc(32, sizeof(node_t));
    return new;
}

/**
 * @brief parse the PPS_SERVERS_LIST_FILENAME file and return the corresponding list of nodes
 * @return the list of nodes initialized from the server file (PPS_SERVERS_LIST_FILENAME)
 */
node_list_t *get_nodes()
{

    node_list_t* nodes = node_list_new();

    FILE* file = fopen(PPS_SERVERS_LIST_FILENAME, "r");

    if(file == NULL) {
        return NULL;
    }

    int current_char = fgetc(file);

    char ip[15];
    int index = 0;
    uint16_t port = 0;

    while(!feof(file)) {

        if (isspace(current_char)) {
            if (fscanf(file, "%hu", &port) != 1)
				return NULL;
            current_char = fgetc(file);

            //take the n first chars, discard else
            char* real_ip = calloc(index, sizeof(char));

            for (int i = 0; i < index; i++) {
                real_ip[i] = ip[i];
            }

            node_t node;
            if (node_init(&node, real_ip, port, 0) != ERR_NONE)
				return NULL;
            node_list_add(nodes, node);
			free(real_ip);
            while(current_char != '\n') {
                current_char = fgetc(file);
            }
            index = 0;
            current_char = fgetc(file);
        }

        ip[index] = current_char;

        index += 1;
        current_char = fgetc(file);
    }

    return nodes;
}
/**
 * @brief add a node to a list of nodes
 * @param list list of nodes where to add to (modified)
 * @param node the node to be added
 * @return some error code indicating whether addition fails or not
 */
error_code node_list_add(node_list_t *list, node_t node)
{

	if (list == NULL)
		return ERR_BAD_PARAMETER;

    list->size++;
    if(list->size > list->allocated_size) {

        node_t* nodes = realloc(list->nodes, list->allocated_size+32);
        if(nodes==NULL) {
            return ERR_NOMEM;
        } else {
            list->allocated_size+=32;
        }
    }

    list->nodes[list->size-1] = node;
    return ERR_NONE;
}


/**
 * @brief free the given list of nodes
 * @param list list of nodes to clean
 */
void node_list_free(node_list_t *list)
{
    for(int i=0; i<list->size; i++) {
        node_end(&list->nodes[i]);
    }

	free(list->nodes);
    free(list);
}

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "node_list.h"
#include <ctype.h>



/**
 * @brief creates a new, empty, node_list_t
 * @return (a pointer to) the new list of nodes or NULL if memory allocation fails
 */
node_list_t *node_list_new()
{
    node_list_t* new = malloc(sizeof(node_list_t));
    if (new == NULL)
		return NULL;
    new->size = 0;
    new->allocated_size = ALLOCATED_MEMORY_LIST;
    new->nodes = calloc(ALLOCATED_MEMORY_LIST, sizeof(node_t));
    if (new->nodes == NULL)
		/* CORRECTEUR: Should clean up new first [-0.2 memory leak] */
		return NULL;
    return new;
}

/**
 * @brief parse the PPS_SERVERS_LIST_FILENAME file and return the corresponding list of nodes
 * @return the list of nodes initialized from the server file (PPS_SERVERS_LIST_FILENAME) or NULL if there is an error
 */
node_list_t *get_nodes()
{

    node_list_t* nodes = node_list_new();

    FILE* file = fopen(PPS_SERVERS_LIST_FILENAME, "r");

    if(file == NULL) {
        return NULL;
    }

    int current_char = fgetc(file);

    /* 15 is the maximum size for an ip adress + terminating \0 */
    char ip[16];
    size_t index = 0;
    uint16_t port = 0;

    while(!feof(file)) {

        if (isspace(current_char)) {
            if (fscanf(file, "%hu", &port) != 1)
                return NULL;
            current_char = fgetc(file);

            //take the n first chars + terminating \0, discard else
            char* real_ip = calloc(index + 1, sizeof(char));

            for (size_t i = 0; i < index; i++) {
                real_ip[i] = ip[i];
            }
            /* CORRECTEUR: Consider using strncpy here... */

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

    if(list->size > list->allocated_size) {

        node_t* nodes = realloc(list->nodes, list->allocated_size + 32);
        /* CORRECTEUR: Magic numbers (should probably be ALLOCATED_MEMORY_LIST?)
         * [-0.5 malus]
         */
        if(nodes == NULL) {
            return ERR_NOMEM;
        } else {
            list->allocated_size += 32;
        }
        /* CORRECTEUR: This `else` is a bit superfluous, as the `if` part
         * returns from the function. It's also a bit irritating, because the
         * `+= 32` (magic number, btw) should go together with the line below.
         * [-0.1 style]
         */

        list->nodes = nodes;
    }

    list->size++;

    list->nodes[list->size - 1] = node;
    return ERR_NONE;
}

/**
 * @brief free the given list of nodes
 * @param list list of nodes to clean
 */
void node_list_free(node_list_t *list)
{
    for(size_t i=0; i<list->size; i++) {
        node_end(&list->nodes[i]);
    }

    free(list->nodes);
    list = NULL;
    /* CORRECTEUR: this line does nothing. Also, why not actually free the list
     * structure itself? [-0.5 correctness]
     */
}

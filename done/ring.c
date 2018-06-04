#include <stdlib.h>
#include <stdio.h>
#include "node_list.h"
#include "ring.h"
#include "node.h"


ring_t *ring_alloc()
{
    return node_list_new();
}

error_code ring_init(ring_t *ring)
{
    node_list_t *node_list = get_nodes();

    /* If parsing of servers.txt fails, get_nodes returns NULL */
    if (node_list == NULL)
        return ERR_BAD_PARAMETER;

    node_list_sort(node_list, node_cmp_sha);

    *ring = *node_list;

    return ERR_NONE;
}


void ring_free(ring_t *ring)
{
    node_list_free(ring);
}

node_list_t *ring_get_nodes_for_key(const ring_t *ring, size_t wanted_list_size, pps_key_t key)
{

    if (wanted_list_size == 0)
        return NULL;

    /*Compute SHA of given key*/
    unsigned char sha[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char *) key, strlen(key), sha);

    node_list_t *nodes = node_list_new();

    if (nodes == NULL)
    	printf("nodes == NULL");

    size_t iterator = 0;

    /* Compute the place of the key in the ring */
    while (strcmp((const char *) sha, (const char *) ring->nodes[iterator].sha) > 0) {
		iterator++;

		if (iterator >= ring->size){
			iterator = 0;
			break;
		}

	}

    if (node_list_add(nodes, ring->nodes[iterator]) != ERR_NONE)
        return NULL;

    size_t left = wanted_list_size - 1;

    int looped = 0;

    while (left != 0) {
        if (!node_list_contains(nodes, ring->nodes[iterator])) {
            if (node_list_add(nodes, ring->nodes[iterator]) != ERR_NONE)
                return NULL;
            left--;
        }
        iterator++;

        if (iterator >= ring->size) {
            if (looped) break;
            else looped += 1;
            iterator = 0;
        }
    }

    return nodes;
}


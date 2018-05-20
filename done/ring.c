#include <stdlib.h>
#include "node_list.h"
#include "ring.h"


ring_t *ring_alloc(){
    return node_list_new();
}

error_code ring_init(ring_t *ring){
    node_list_t* node_list =  get_nodes();

    /* If parsing of servers.txt fails, get_nodes returns NULL*/
    if (node_list == NULL)
        return ERR_BAD_PARAMETER;

    node_list_sort(node_list, node_cmp_sha);

    ring = node_list;

    return ERR_NONE;
}


void ring_free(ring_t *ring) {
    node_list_free(ring);
    ring = NULL;
}

node_list_t *ring_get_nodes_for_key(const ring_t *ring, size_t wanted_list_size, pps_key_t key){



    return NULL;
}


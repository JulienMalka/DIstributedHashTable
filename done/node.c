#include <stdio.h>
#include "node.h"
#include "error.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

/**
 * @brief node initialization function
 * @param node node to be initalized (modified)
 * @param ip server IP address
 * @param port server port
 * @param node_id after week 11 (included), specify the server node id in a key ring. Unused before week 11.
 * @return some error code
 */
error_code node_init(node_t *node, const char *ip, uint16_t port, size_t _unused node_id)
{
     const size_t sha_len = 20;
     char sha[sha_len];
     node->id = node_id;
     char* port_str = calloc(8, sizeof(char));
     sprintf(port_str, "%d", port);
     char* id_str = calloc(2, sizeof(char));
     sprintf(id_str, "%lu", node_id);

     char dest[strlen(ip) + 1 + strlen(port_str) + 1 + strlen(id_str) + 1];
     char* space = " ";
     strcat(dest, ip);
     strcat(dest, space);
     strcat(dest, port_str);
     strcat(dest, space);
     strcat(dest, id_str);
//     SHA1(dest,strlen(dest), sha);
     return get_server_addr(ip, port, &node->addr);
}


/**
 * @brief all what needs to be done when a node is removed.
 *        Actually useless (=empty) in the current version of the project, but remains here as a placeholder
 * @param node the node that is removed, passed by reference as it might be modified.
 */
void node_end(node_t* _unused node)
{
    //Void for now
}


int node_cmp_sha(const node_t *first, const node_t *second)
{
return strcmp(first->sha, second->sha);
}

#include <stdio.h>
#include "node.h"
#include "error.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>
#include "openssl/sha.h"

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
    unsigned char *sha = calloc(SHA_DIGEST_LENGTH, sizeof(char));
    node->id = node_id;
    char *port_str = calloc(8, sizeof(char));
    sprintf(port_str, "%d", port);
    char *id_str = calloc(2, sizeof(char));
    sprintf(id_str, "%lu", node_id);

    char dest[strlen(ip) + 1 + strlen(port_str) + 1 + strlen(id_str) + 1];
    memset(dest, '\0', 1);
    char space[1];
    memset(space, ' ', 1);
    strcat(dest, ip);
    strcat(dest, space);
    strcat(dest, port_str);
    strcat(dest, space);
    strcat(dest, id_str);

    SHA1((const unsigned char *) dest, strlen(dest), sha);

    node->sha = sha;

    free(port_str);
    free(id_str);

    return get_server_addr(ip, port, &node->addr);
}


/**
 * @brief all what needs to be done when a node is removed.
 *        Actually useless (=empty) in the current version of the project, but remains here as a placeholder
 * @param node the node that is removed, passed by reference as it might be modified.
 */
void node_end(node_t *_unused node)
{
    if (node == NULL)
        return;
    else
        free(node->sha);
    node->sha = NULL;
    node = NULL;
}


int node_cmp_sha(const node_t *first, const node_t *second)
{
    return strcmp((const char *) first->sha, (const char *) second->sha);
}

int node_cmp_server_addr(const node_t *n1, const node_t *n2)
{
    if (n1->addr.sin_addr.s_addr == n2->addr.sin_addr.s_addr)
        return n1->addr.sin_port > n2->addr.sin_port;
    else return n1->addr.sin_addr.s_addr > n2->addr.sin_addr.s_addr;
}

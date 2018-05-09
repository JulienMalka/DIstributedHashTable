#include "node.h"
#include "error.h"
#include "system.h"



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
    return get_server_addr(ip, port, node);
}


/**
 * @brief all what needs to be done when a node is removed.
 *        Actually useless (=empty) in the current version of the project, but remains here as a placeholder
 * @param node the node that is removed, passed by reference as it might be modified.
 */
void node_end(node_t *node)
{
    //Void for now
}

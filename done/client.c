#include <stdio.h>
#include "client.h"

/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */
void client_end(client_t *client)
{
    node_end();
}

error_code client_init(client_init_args_t)
{
    client_init_args_t.client->name = client_init_args_t.name;
    node_t node;
    const char ip = PPS_DEFAULT_IP;
    node_init(&node, &ip, PPS_DEFAULT_PORT, 0);
    client_init_args_t.client->server = node;
    client_init_args_t.client->socket = get_socket(1);
    return 0;
}

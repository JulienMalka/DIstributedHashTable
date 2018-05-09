#include <stdio.h>
#include "client.h"
#include "config.h"
#include "system.h"
#include "node.h"
#include "network.h"
/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */

void client_end(client_t *client)
{
    node_list_free(&(client->server));
    /* CORRECTEUR: Should close the socket here [-0.25 correctness] */
}

error_code client_init(client_init_args_t client_init_args)
{

    client_init_args.client->name = client_init_args.name;
    node_list_t* nodes = node_list_new();
    nodes = get_nodes();
    client_init_args.client->server = *nodes;
    client_init_args.client->socket = get_socket(1);
    /* CORRECTEUR: *Any* sort of error handling is missing [-0.5 robustness] */
    return 0;
}

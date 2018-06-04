#include <stdio.h>
#include "client.h"
#include "config.h"
#include "system.h"
#include "node.h"
#include "network.h"
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */
void client_end(client_t *client)
{
    close(client->socket);
    free(client->args);
    ring_free(&client->server);
}

/**
 * @brief properly initialize a client
 * @param client_init_args
 *
 */
error_code client_init(client_init_args_t client_init_args)
{

    /* name of the client is name of the executable */
    client_init_args.client->name = *client_init_args.argv[0];

    ++*client_init_args.argv;

    client_init_args.size_args--;

    /* check if there is enough arguments - special case SIZE_MAX for pps-client-cat */
    if (client_init_args.required != SIZE_MAX && client_init_args.size_args < client_init_args.required) {
        return ERR_BAD_PARAMETER;
    }
    size_t adress_start = argv_size(*client_init_args.argv);

    args_t *args_opt = parse_opt_args(client_init_args.optionnal, client_init_args.argv);

    size_t nb_parsed;
    if (args_opt == NULL) {
        return ERR_BAD_PARAMETER;
    } else {
        nb_parsed = adress_start - argv_size(*client_init_args.argv);
        client_init_args.client->args = args_opt;
    }

    /* check if there is exactly the number of mandatory arguments - special case SIZE_MAX for pps-client-cat */
    if (client_init_args.required != SIZE_MAX && client_init_args.size_args - nb_parsed != client_init_args.required) {
        return ERR_BAD_PARAMETER;
    }

    ring_t *ring = ring_alloc();
    ring_init(ring);

    if (ring == NULL)
        return ERR_BAD_PARAMETER;


    if (client_init_args.client->args->N > ring->size)
        client_init_args.client->args->N = ring->size;


    client_init_args.client->server = *ring;
    int s = get_socket(1);
    if(s == -1){
    return ERR_NETWORK;  
    }
    client_init_args.client->socket = get_socket(1);

    return ERR_NONE;
}

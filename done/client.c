#include <stdio.h>
#include "client.h"

/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */
void client_end(client_t *client){
  node_end();
}


/**
 * @brief does all the work to be done at the beginning of life of a client
 * @param client the client to initialize
 * @return some error code, if any.
 */
error_code client_init(client_init_args_t client_init){
  client = *client_init.client;
  client.name = client_init.name;
  node_t *node = NULL;
  node_init(node, PPS_DEFAULT_IP, PPS_DEFAULT_PORT, NULL);
  client.server = node;
  client.socket = get_socket(1);

}

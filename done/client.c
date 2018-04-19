#include <stdio.h>
#include "client.h"
#include "config.h"
#include "system.h"
/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */

void client_end(client_t *client){
  node_end(&(client->server));
}

error_code client_init(client_init_args_t client_init_args){

client_init_args.client->name = client_init_args.name;
node_init(&client_init_args.client->server, PPS_DEFAULT_IP, PPS_DEFAULT_PORT, 0);
client_init_args.client->socket = get_socket(1);
return 0;
}

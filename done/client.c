#include <stdio.h>
#include "client.h"

/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */
void client_end(client_t *client){
  node_end();
}

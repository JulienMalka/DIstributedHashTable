// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"
#include "client.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{

  client_t client;
  client_init_args_t client_i;
  client_i.client = &client;
  client_i.argv = &argv;
  client_i.required = 2;
  client_i.optionnal = (TOTAL_SERVERS | PUT_NEEDED);
  client_i.size_args = argc;

  error_code error_init = client_init(client_i);

  if(error_init!=ERR_NONE){

    printf("Bad command line argument\n");
  }


        char* value = argv[1];
        char* key = argv[0];
        //printf("key : %s\n", argv[0]);
        //printf("value : %s\n", argv[1]);

        error_code error = network_put(*client_i.client, key, value);
        if (error == ERR_NONE) {
            printf("OK\n");
        } else {
            printf("FAIL\n");
        }


    return 0;
}

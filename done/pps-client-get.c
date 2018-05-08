// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"
#include "client.h"
#include "stdlib.h"
#include "util.h"

int main(int argc, char* argv[])
{

    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.argv = &argv;
    client_i.required = 1;
    client_i.optionnal = (TOTAL_SERVERS | GET_NEEDED);
    client_i.size_args = argc;

    error_code error_init = client_init(client_i);


if(error_init!=ERR_NONE){

  printf("Bad command line argument\n");
}
    pps_value_t get_value;

        //printf("key : %s\n", argv[0]);
        char* key = argv[0];


        error_code error = network_get(*client_i.client, key, &get_value);

        if(error == ERR_NONE) {
            printf("OK %s\n", get_value);
            free_const_ptr(get_value);
        } else {

            printf("FAIL\n");
        }




    return 0;
}

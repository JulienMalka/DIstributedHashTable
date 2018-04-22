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

int main(void)
{

    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.name = "test";
    client_init(client_i);

    pps_value_t get_value;

    while(1) {


        char* key = malloc(MAX_MSG_ELEM_SIZE);
        int number=-1;

        do {
            number = scanf("%s", key);
        } while (number==-1);



        error_code error = network_get(*client_i.client, key, &get_value);
        free(key);
        if(error == ERR_NONE) {
            printf("OK %s\n", get_value);
            free_const_ptr(get_value);
        } else {

            printf("FAIL\n");
        }
    }



    return 0;
}

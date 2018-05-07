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

    pps_value_t get_value_1;
    pps_value_t get_value_2;

    while(1) {


        char* key_1 = malloc(MAX_MSG_ELEM_SIZE);
        char* key_2 = malloc(MAX_MSG_ELEM_SIZE);
        int number=-1;

        do {
            number = scanf("%s %s", key_1, key_2);
        } while (number==-1);



        error_code error_1 = network_get(*client_i.client, key_1, &get_value_1);
        error_code error_2 = network_get(*client_i.client, key_2, &get_value_2);
        free(key_1);
        free(key_2);
        if(error_1 == ERR_NONE && error_2==ERR_NONE) {
            char* firstocc = strstr(get_value_1, get_value_2);
            int place;
            if(firstocc==NULL) {
                place = -1;
            } else {

                place = firstocc - get_value_1;
            }
            printf("OK %d\n", place);
            free_const_ptr(get_value_1);
            free_const_ptr(get_value_2);
        } else {

            printf("FAIL\n");
        }
    }



    return 0;
}

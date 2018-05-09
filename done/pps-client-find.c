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

    /* Set up client */
    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.argv = &argv;
    client_i.required = 2;
    client_i.optionnal = (TOTAL_SERVERS | PUT_NEEDED | GET_NEEDED);
    client_i.size_args = argc;

    error_code error_init = client_init(client_i);

    if (error_init!=ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    /* Parse the mandatory arguments */
    pps_value_t get_value_1;
    pps_value_t get_value_2;

    if (argv[0] != NULL && argv[1] != NULL) {
        get_value_1 = argv[0];
        get_value_2 = argv[1];
    } else {
        printf("FAIL\n");
        return -1;
    }

    char* key_1 = calloc(MAX_MSG_SIZE, sizeof(char));
    char* key_2 = calloc(MAX_MSG_SIZE, sizeof(char));


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




    return 0;
}

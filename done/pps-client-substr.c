#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "network.h"
#include "config.h"

int main(int argc, char* argv[])
{

    /* Set up client */
    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.argv = &argv;
    client_i.required = 4;
    client_i.optionnal = (TOTAL_SERVERS | PUT_NEEDED | GET_NEEDED);
    client_i.size_args = argc;

    error_code error_init = client_init(client_i);

    if(error_init!=ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    /* Parse mandatory arguments */
    char* key_from;
    char* key_to;
    int position;
    int length;

    if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL && argv[3] != NULL) {
        key_from = argv[0];
        position = strtol(argv[1], NULL, 10);
        length = strtol(argv[2], NULL, 10);
        key_to = argv[3];
    } else {
        printf("FAIL\n");
        return -1;
    }

    pps_value_t value_from;

    error_code error_get = network_get(client, key_from, &value_from);

    if (error_get != ERR_NONE) {
        printf("FAIL/n");
        return -1;
    }

    /*COMPUTE THE SUBSTRING*/
    if (position < 0) {
        position = strlen(value_from) + position;
    } else {
        position = position;
    }

    /*ERROR HANDLING*/
    if (length < 0 || length + position > strlen(value_from)) {
        printf("FAIL\n");
        return -1;
    }

    char substring[strlen(value_from)];

    for(int i = position; i < position + length; i++) {
        substring[i - position] = value_from[i];
    }

    if (position == 0)
        memset(substring, '\0', strlen(substring));

    error_code error_put = network_put(client, key_to, substring);

    if (error_put != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    printf("OK\n");

    return 1;
}

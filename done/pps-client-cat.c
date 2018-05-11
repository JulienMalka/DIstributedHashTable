#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#include "network.h"
#include "error.h"
#include "util.h"

int main(int argc, char* argv[])
{

    /* Client initialization and parsing optionnal arguments */
    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.argv = &argv;
    client_i.required = SIZE_MAX;
    client_i.optionnal = (TOTAL_SERVERS | PUT_NEEDED | GET_NEEDED);
    client_i.size_args = argc;

    error_code error_init = client_init(client_i);

    if (error_init != ERR_NONE) {
        printf("it failed in initialization\n");
        printf("FAIL\n");
        return -1;
    }
    
    /* Not enough mandatory arguments */
    if (argv_size(argv) < 2){
		printf("FAIL\n");
		return -1;
	}

    /* Parse the key */
    char* keys[300];
    size_t nbr_arg = 0;
    while(argv[nbr_arg] != NULL) {
        keys[nbr_arg] = argv[nbr_arg];
        nbr_arg++;
    }

    /* Compensate the last increment */
    nbr_arg--;

    /* The last key is the output key */
    char* output_key = keys[nbr_arg];

    pps_value_t value_from;
    error_code error_code = network_get(client, keys[0], &value_from);

    if (error_code != ERR_NONE) {
//        printf("it failed in the first get\n");
        printf("FAIL\n");
        return -1;
    }

    char* concat = value_from;
    
//    printf("nbr_arg = %lu\n", nbr_arg);

    /* Get the respective values and concatenate them */
    for (size_t i = 1; i < nbr_arg; i++) {

        error_code = network_get(client, keys[i], &value_from);

//		printf("i = %lu get the value %s and current concat = %s\n",i ,value_from, concat);

        if (error_code != ERR_NONE) {
            printf("FAIL\n");
            return -1;
        }

        strcat(concat, value_from);
    }
    
//    printf("put the string %s at the key value %s\n", concat, output_key); 

    error_code = network_put(client, output_key, concat);

    if (error_code != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    printf("OK\n");


    return 0;
}

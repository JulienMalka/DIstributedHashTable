#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#include "network.h"
#include "error.h"
#include "util.h"

/**
 * @brief On n arguments, concatenate the n-1 associated values into a single string, then stores it as value at n_th key
 * @return 0 on normal exit, -1 otherwise
 */
int main(int argc, char *argv[])
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
        printf("FAIL\n");
        return -1;
    }

    /* Not enough mandatory arguments */
    if (argv_size(argv) < 2) {
        client_end(&client);
        printf("FAIL\n");
        return -1;
    }

    /* Parse the key */
    char *keys[MAX_MANDATORY_ARGUMENTS];
    size_t nbr_arg = 0;
    while (argv[nbr_arg] != NULL) {
        keys[nbr_arg] = argv[nbr_arg];
        nbr_arg++;
    }

    /* Compensate the last increment */
    nbr_arg--;

    /* The last key is the output key */
    char *output_key = keys[nbr_arg];

    pps_value_t value_from;
    error_code error_code = network_get(client, keys[0], &value_from);

    if (error_code != ERR_NONE) {
        client_end(&client);
        printf("FAIL\n");
        return -1;
    }

    pps_value_t concat_temp = value_from;
    char *concat = calloc(strlen(value_from) + 1, sizeof(char));
    strcpy(concat, concat_temp);
    free_const_ptr(concat_temp);
    /* CORRECTEUR: concat_temp points to the same string as value_from, so
     * freeing here causes... headache [-0.5 correctness]
     * Just `concat = strdup(value_from);`
     */

    /* Get the respective values and concatenate them */
    for (size_t i = 1; i < nbr_arg; i++) {

        error_code = network_get(client, keys[i], &value_from);

        if (error_code != ERR_NONE) {
            client_end(&client);
            printf("FAIL\n");
            return -1;
        }

        concat = realloc(concat, (strlen(concat) + strlen(value_from)) * sizeof(char));
        /* CORRECTEUR: +1 for the final \0 [-0.25] */
        strcat(concat, value_from);
    }

    error_code = network_put(client, output_key, concat);

    if (error_code != ERR_NONE) {
        client_end(&client);
        printf("FAIL\n");
        return -1;
    }

    client_end(&client);
    printf("OK\n");

    return 0;
}

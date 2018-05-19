#include <stdio.h>
#include <stdlib.h>

//for is digit
#include "ctype.h"

#include "client.h"
#include "network.h"
#include "config.h"

/**
 * @brief checks if string corresponds to a valid number
 * @param str string
 * @return 1 if so, 0 otherwise
 */
int isValidNumber(const char* str)
{
    if (str == NULL || *str == '\0' || isspace(*str))
        return 0;
    char * p;
    strtod(str, &p);
    return *p == '\0';
}

/**
 * @brief Compute a substring of given position and length of a value associated to a given key and stores it with a given key
 * As follows : pps-client-substr [-n N] [-w W] [-r R] [--] <input-key> <position> <length> <output-key>
 * @return 0 on normal exit, -1 otherwise
 */ 
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
    size_t length;

    if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL && argv[3] != NULL) {
        /* Checks if second and third argument are valid numbers */
        if (isValidNumber(argv[1]) && isValidNumber(argv[2])) {
            key_from = argv[0];
            position = (int) strtol(argv[1], NULL, 10);
            length = (size_t) strtol(argv[2], NULL, 10);
            key_to = argv[3];
        } else {
            printf("FAIL\n");
            return -1;
        }
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
        position = (int) (strlen(value_from) + position);
    }

    /*ERROR HANDLING*/
    if (length + position > strlen(value_from)) {
        printf("FAIL\n");
        return -1;
    }

    /* array */
    char substring[strlen(value_from)];

    if (length == 0)
        memset(substring, '\0', strlen(substring));
    else {
        for(int i = position; i < position + length; i++) {
            substring[i - position] = value_from[i];
        }
        substring[length] = '\0';
    }

    error_code error_put = network_put(client, key_to, substring);

    if (error_put != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    printf("OK\n");

    client_end(&client);

    return 0;
}

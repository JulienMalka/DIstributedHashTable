#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "args.h"

void parse_option(size_t supported_arg, char ***rem_argv, size_t* value)
{

    if (supported_arg) {

        ++*rem_argv;

        /* If option value isn't defined, throw an error */
        if (*rem_argv == NULL || !strcmp(**rem_argv, "--") || !isdigit(***rem_argv)) {
            fprintf(stderr, "ERREUR: pas d'argument pour l'option \n");
        } else {
            /* Convert char to int */
            *value = ***rem_argv - '0';
        }

    } else {
        fprintf(stderr, "ERREUR: option non prise en compte \n");
    }
}

args_t *parse_opt_args(size_t supported_args, char ***rem_argv)
{

    /* |*rem_argv| est le pointeur sur un élément du tableau de string */

//	printf("segfault happens after 0\n");

    /* return structure */
    /* With default values */
    args_t* parsed = malloc(sizeof(args_t));
    parsed->N = 3;
    parsed->R = 2;
    parsed->W = 2;

//	printf("segfault happens after 1\n");
    int parsed_n = 0;
    while(**rem_argv != NULL) {

        /* Check for -n option*/
        if (!strcmp(**rem_argv, "-n")) {
            parse_option(supported_args & TOTAL_SERVERS, rem_argv, &parsed->N);
            parsed_n++;
            /* Check for -r option */
        } else if (!strcmp(**rem_argv, "-r")) {
            parse_option(supported_args & GET_NEEDED, rem_argv, &parsed->R);
            parsed_n++;
            /* Check for -w option */
        } else if (!strcmp(**rem_argv, "-w")) {
            parse_option(supported_args & PUT_NEEDED, rem_argv, &parsed->W);
            parsed_n++;
        } else if (!strcmp(**rem_argv, "--")) {
            ++*rem_argv;
            return parsed;
        } else {

            if(parsed_n == 0) {
                free(parsed);
                return NULL;
            } else {
                return parsed;
            }
        }

        //printf("current string = %s\n", **rem_argv);

        ++*rem_argv;
//		printf("increamting interator\n");
    }

    return parsed;
}

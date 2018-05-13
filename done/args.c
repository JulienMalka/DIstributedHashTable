#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "args.h"

/**
 * @brief helper function for parse_opt_args()
 * @param supported_arg checks if arg is supported
 * @param rem_argv pointer on current string
 * @param value pointer to a value of the struct
 */
void parse_option(size_t supported_arg, char ***rem_argv, size_t* value)
{
    if (supported_arg) {

        ++*rem_argv;

        /* If option value isn't defined, throw an error */
        if (*rem_argv == NULL || !strcmp(**rem_argv, "--") || !isdigit(***rem_argv)) {
        } else {
            /* Converts char to int */
            *value = ***rem_argv - '0';
        }
    } 
}

args_t *parse_opt_args(size_t supported_args, char ***rem_argv)
{

    /* |*rem_argv|  i the pointer on a string of the string array rem_argv */

    /* Structure with default values */
    args_t* parsed = malloc(sizeof(args_t));
    parsed->N = 3;
    parsed->R = 2;
    parsed->W = 2;

    /* Counts the number of parsed arguments*/
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
            /* Check for end of optionnal arguments */
        } else if (!strcmp(**rem_argv, "--")) {
            ++*rem_argv;
            return parsed;
        } else {
            /* if no arguments parsed, returns NULL, otherwise returns what was parsed until now */
            if(parsed_n == 0) {
                free(parsed);
                return NULL;
            } else {
                return parsed;
            }
        }

        ++*rem_argv;
    }

    return parsed;
}

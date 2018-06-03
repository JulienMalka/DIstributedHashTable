#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "args.h"
#include "error.h"

/**
 * @brief helper function for parse_opt_args()
 * @param supported_arg checks if arg is supported
 * @param rem_argv pointer on current string
 * @param value pointer to a value of the struct
 * @return error_code ERR_NONE, ERR_BAD_PARAMETER otherwise
 */
error_code parse_option(size_t supported_arg, char ***rem_argv, size_t *value)
{
    if (supported_arg) {

        ++*rem_argv;

        /* If option value isn't defined, throw an error */
        if (*rem_argv == NULL || !strcmp(**rem_argv, "--")) {
            return ERR_BAD_PARAMETER;
        } else {
            /* Converts char* to int with base 10*/
            long val = strtol(**rem_argv, NULL, 10);

            if (val > 0) {
                *value = (size_t) val;
            } else return ERR_BAD_PARAMETER;
        }
    } else
        return ERR_NOT_FOUND;

    return ERR_NONE;
}

int check_bounds(args_t args){
    return !(args.N < args.R || args.N < args.W);
}

args_t *parse_opt_args(size_t supported_args, char ***rem_argv)
{

    /* |*rem_argv|  i the pointer on a string of the string array rem_argv */

    /* Structure with default values */
    args_t *parsed = malloc(sizeof(args_t));
    parsed->N = 3;
    parsed->R = 2;
    parsed->W = 2;

    /* Counts the number of parsed arguments*/
    int parsed_n = 0;
    while (**rem_argv != NULL) {

        /* Check for -n option*/
        if (!strcmp(**rem_argv, "-n")) {
            if (parse_option(supported_args & TOTAL_SERVERS, rem_argv, &parsed->N) != ERR_NONE)
                return NULL;

            parsed_n++;
            /* Check for -r option */
        } else if (!strcmp(**rem_argv, "-r")) {
            if (parse_option(supported_args & GET_NEEDED, rem_argv, &parsed->R) != ERR_NONE)
                return NULL;

            parsed_n++;
            /* Check for -w option */
        } else if (!strcmp(**rem_argv, "-w")) {
            if (parse_option(supported_args & PUT_NEEDED, rem_argv, &parsed->W) != ERR_NONE)
                return NULL;

            parsed_n++;
            /* Check for end of optional arguments */
        } else if (!strcmp(**rem_argv, "--")) {
            ++*rem_argv;
            if (!check_bounds(*parsed))
                return NULL;
            return parsed;
        } else {
            if (!check_bounds(*parsed))
                return NULL;
            return parsed;
        }

        ++*rem_argv;
    }

    if (!check_bounds(*parsed))
        return NULL;

    return parsed;
}

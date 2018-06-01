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
error_code parse_option(size_t supported_arg, char ***rem_argv, size_t* value)
{
    if (supported_arg) {

        ++*rem_argv;

        /* If option value isn't defined, throw an error */
        if (*rem_argv == NULL || !strcmp(**rem_argv, "--")) {
			return ERR_BAD_PARAMETER;
        } else {

            /* Converts char to int */	
            *value = ***rem_argv - '0';
        }
    } else 
		return ERR_BAD_PARAMETER;
		/* CORRECTEUR: If it's not a supported option, it could still be a
		 * positional argument, so not really an error.
		 * [-0.5 correctness]
		 */
    
    return ERR_NONE; 
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
            /* Check for end of optionnal arguments */
        } else if (!strcmp(**rem_argv, "--")) {
            ++*rem_argv;
            return parsed;
        } else {
            /* if no arguments parsed, returns NULL, otherwise returns what was parsed until now */
            /* CORRECTEUR: Yes, consider returning an argument structure in all
             * cases. [-0.5 correctness]
             */
            if(parsed_n == 0) {
                free(parsed);
                return NULL;
            } else {
                return parsed;
            }
        }

        ++*rem_argv;
    }
    /* CORRECTEUR: Should check that N >= R,W [-1 correctness] */

    return parsed;
}

/* CORRECTEUR: Overall, what happens if you accept -n and -w (e.g. for
 * pps-client-put), and the user invokes it as `pps-client-put -n 1 -r 3`?
 * Hint: that should be valid.
 */
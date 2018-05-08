#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "args.h"

args_t *parse_opt_args(size_t supported_args, char ***rem_argv){
	
	/* |*rem_argv| est le pointeur sur le premier élement du tableau de string */
	
	/* return structure */
	args_t* parsed = malloc(sizeof(args_t)); 
	
	while(*rem_argv != NULL){
		
		/* Check for -n option*/
		if (!strcmp(**rem_argv, "-n")){
			
			if (supported_args & TOTAL_SERVERS){
				
				printf("-n option found\n");
				
			++*rem_argv;
			
			/* If option value isn't defined, throw an error */
			if (*rem_argv == NULL || !strcmp(**rem_argv, "--") || !isdigit(***rem_argv)){
				fprintf(stderr, "ERREUR: pas d'argument pour l'option -n\n");
				return NULL;
			} else {
				parsed->N = ***rem_argv;
			}
			} else {
				fprintf(stderr, "ERREUR: option non prise en compte");
				return NULL;
			}
			
		/* Check for -r option */	
		} else if (!strcmp(**rem_argv, "-r")) {
			
			if (supported_args & GET_NEEDED){
			++rem_argv;
			
			/* If option value isn't defined, throw an error */
			if (*rem_argv == NULL || !strcmp(**rem_argv, "--")){	
				fprintf(stderr, "ERREUR: pas d'argument pour l'option -r\n");
				return NULL;				
			} else {
				parsed->R = ***rem_argv;
			}
			} else {
				fprintf(stderr, "ERREUR: option non prise en compte");
				return NULL;
			}
			
		/* Check for -w option */		
		} else if (!strcmp(**rem_argv, "-w")) {
			
			if (supported_args & GET_NEEDED){
			++rem_argv;
			
			/* If option value isn't defined, throw an error */
			if (*rem_argv == NULL || !strcmp(**rem_argv, "--")){	
				fprintf(stderr, "ERREUR: pas d'argument pour l'option -w\n");
				return NULL;				
			} else {
				parsed->W = ***rem_argv;
			}
			} else {
				fprintf(stderr, "ERREUR: option non prise en compte");
				return NULL;
			}
		}
	}
	
	return parsed;
}

#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "network.h"
#include "config.h"

int main(void){
	
	client_t client;
	client_init_args_t client_i;
	client_i.client = &client;
	client_i.name = "test";
	client_init(client_i);
	
	while(1){
		
		char key_from[MAX_MSG_ELEM_SIZE];
		char key_to[MAX_MSG_ELEM_SIZE];
		int position;
		int length;
				
		int ok = 1;
		while (ok) {
			int error = scanf("%s %d %d %s", key_from, &position, &length, key_to);
			if (error != 1) ok = 0;
			else {
				printf("FAIL\n");
			}
		}
		
		pps_value_t value_from;
		
		error_code error_get = network_get(client, key_from, &value_from);
		
		if (error_get != ERR_NONE){
			printf("FAIL/n");
			continue;
		}		

		
	}
	
} 

// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"
#include <stdlib.h>

int main(void){

	client_t client;
	client_init_args_t client_i;
	client_i.client = &client;
	client_i.name = "test";
	client_init(client_i);


	while(1){

		char* value = malloc(MAX_MSG_ELEM_SIZE);
		char* key = malloc(MAX_MSG_ELEM_SIZE);
		int ok = 1;

		while (ok) {
			int error = scanf("%s %s", key, value);
			if (error != 1) ok = 0;
			else {
				printf("FAIL\n");
			}
		}


		printf("Sending put request key : %s value : %s\n", key, value);
		error_code error = network_put(*client_i.client, key, value);
		free(key);
		free(value);
		if (error == ERR_NONE){
			printf("OK\n");
		} else {
			printf("FAIL\n");
		}
	}

	return 0;
}

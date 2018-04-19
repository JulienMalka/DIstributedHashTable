// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"

int main(void){

	client_t client;
	client_init_args_t client_i;
	client_i.client = &client;
	client_i.name = "test";
	client_init(client_i);

	while(1){

		pps_value_t value;
		pps_key_t key;
		int ok = 1;

		while (ok){
			int error = scanf("%c %d", &key, &value);
			if (error != 1) ok = 0;
			else {
				printf("input user failed");
				printf("FAIL\n");
			}
		}

		error_code error = network_put(*client_i.client, key, value);
		if(error == ERR_NONE){
			printf("OK\n");
		}else {
			printf("Failed to send info");
			printf("FAIL\n");
		}
	}

	return 0;
}

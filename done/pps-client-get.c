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

	pps_value_t get_value;

	while(1){

		pps_key_t key;
		int ok = 1;

		while (ok){
			int error = scanf("%c", &key);
			if (!error) ok = 0;
			else printf("FAIL\n");
		}


		error_code error = network_get(*client_i.client, key, &get_value);
		if(error == ERR_NONE){
		printf("OK %d", get_value);
	}else{

		printf("FAIL\n")
	}
	}



	return 0;
}

// standard includes 
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"

int main(void){
	
	client_init_args_t client_i;
	client_init(client_i);

	while(1){
		
		pps_value_t value;
		pps_key_t key;
		int ok = 1;
				
		while (ok){		
			int error = scanf("%c %d", &key, &value);
			if (error != 1) ok = 0;
			else printf("FAIL\n");
		}
		printf("OK\n");
		
		network_put(*client_i.client, key, value);
	}
	
	return 0;
}

// standard includes 
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"

int main(void){
	
	int s;
	s = get_socket(0);
	
	//Bind server to the address:port
	bind_server(s, PPS_DEFAULT_IP, PPS_DEFAULT_PORT);
	
	client_t client;
	
	
	while(1){
		
		pps_value_t value;
		pps_key_t key;
		int ok = 1;
				
		while (ok == 1){		
			int error = scanf("%c %d", &key, &value);
			if (error != 1) ok = 0;
			else printf("FAIL\n");
		}
		printf("OK\n");
		
		network_put(client, key, value);
	}
	
	
	
	return 0;
}

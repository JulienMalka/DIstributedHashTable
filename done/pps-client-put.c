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
	
	while(1){
		
		
		
		
	}
	
	
	
	return 0;
}

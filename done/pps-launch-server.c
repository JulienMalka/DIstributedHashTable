
// standard includes 
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "config.h"
#include "error.h"


int main(void){
	
	//Set up socket
	int s;
	s = get_socket(0);
	
	//Bind server to the address:port
	bind_server(s, PPS_DEFAULT_IP, PPS_DEFAULT_PORT);
	
	//Receive messages forever
	while(1){
		struct sockaddr_in cli_addr;
		socklen_t addr_len = sizeof(cli_addr);
		memset(&cli_addr, 0, addr_len);
		
		unsigned char* in_msg;
        ssize_t in_msg_len = recvfrom(s, &in_msg, sizeof(in_msg), 0,
                                      (struct sockaddr *) &cli_addr, &addr_len);
        
        if (in_msg_len != sizeof(in_msg)) { // Wrong message size.
            printf("Received invalid message");
            continue;
        }     
      
		// Write Request
		if (in_msg_len == 5){
			
		}
		
		// Read Request
		if (in_msg_len == 1){
			
		}
	}
	
	return 1;
	
}


// standard includes 
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
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
		node_t cli_addr;
		socklen_t addr_len = sizeof(cli_addr);
		memset(&cli_addr, 0, addr_len);
		
		pps_key_t in_msg;
        ssize_t in_msg_len = recvfrom(s, &in_msg, sizeof(in_msg), 0,
                                      (struct sockaddr *) &cli_addr, &addr_len);
        
        if (in_msg_len != sizeof(in_msg)) { // Wrong message size.
            printf("Received invalid message");
            continue;
        }
      
		// Write Request
		if (in_msg_len == 5){
			sendto(s, 0, 0, 0,
               (struct sockaddr *) &cli_addr, sizeof(cli_addr));
		}
		
		// Read Request
		if (in_msg_len == 1){
			
		pps_key_t request;
		request = ntohl(in_msg);	
			
		client_t client;
		client.server = cli_addr;
		client.socket = s;
			
		pps_value_t get;
		network_get(client, request, &get);
			
		unsigned int out_msg;
		out_msg = htonl(get);
			
		sendto(s, &out_msg, sizeof(out_msg), 0,
              (struct sockaddr *) &cli_addr, sizeof(cli_addr));
		}
	}
	
	return 1;
	
}

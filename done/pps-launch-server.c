
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
	
	Htable_t h_table;
	
	int ok = 1;
	char ip[15]; //max 15 characters in an ip adress
	int port;
	while(ok){
		printf("IP port? ");
		int error = scanf(" %s %d", ip, &port);
		if (error != 1) ok = 0;
		else {
			printf("FAIL\n");
		}
	}

	//Bind server to the address:port
	bind_server(s, ip, port);

	//Receive messages forever
	while(1){
		node_t cli_addr;
		socklen_t addr_len = sizeof(cli_addr);
		memset(&cli_addr, 0, addr_len);
		unsigned char in_msg[5];
		printf("preparing to receive something\n");
        ssize_t in_msg_len = recvfrom(s, in_msg, sizeof(in_msg), 0,
                                      (struct sockaddr *) &cli_addr, &addr_len);
        if (in_msg_len != 1 && in_msg_len != 5) { // Wrong message size.
			printf("in_msg_len = %lu\n", in_msg_len);
			printf("sizeof(in:msg) = %lu\n", sizeof(in_msg));
            printf("Received invalid message\n");
            continue;
        }
        
        printf("Received something\n");
        printf("%lu\n", sizeof(in_msg));

		// Write Request
		if (in_msg_len == 5){
			pps_key_t key = in_msg[0];
			pps_value_t value = ntohl(in_msg[1] << 24) | (in_msg[2] << 16) | (in_msg[3] << 8) | in_msg[4];
			
			add_Htable_value(h_table, key, value);
			
		printf("write request = (%c, %d)... sending response\n", key, value);
			
			sendto(s, 0, 0, 0,
               (struct sockaddr *) &cli_addr, sizeof(cli_addr));
		}

		// Read Request
		if (in_msg_len == 1){

		pps_key_t request = in_msg[0];
		printf("%c\n", request);

		pps_value_t get = get_Htable_value(h_table, request);
		
		printf("read request = %c ... sending response = %d\n", request, get);

		unsigned int out_msg;
		out_msg = htonl(get);

		sendto(s, &out_msg, 4, 0,
              (struct sockaddr *) &cli_addr, sizeof(cli_addr));
		}
	}

	return 1;

}

#include <stdio.h>
#include <stdlib.h>

// for basic socket communication
#include <sys/socket.h>

#include "error.h"
#include "system.h"
#include "network.h"
#include "config.h"

void parse_and_print_response(char* in_msg, size_t length);

int main(void){
	
	//Set up socket
    int s = get_socket(0);
	
	while(1){
		
		int ok = 1;
		char ip[15]; /* max 15 characters in an ip adress */
		int port;
		
		node_t node;
		
		while(ok) {
			int error = scanf(" %s %d", ip, &port);
			error_code error_init = node_init(&node, ip, port, 0);
			if (error != 1 && error_init == ERR_NONE) 
				ok = 0;
			else {
				printf("FAIL\n");
			}
		}
			
		/* Send packet to node */
		if (send_packet(s, "\0", 1, node) != ERR_NONE){			
			//error handling
			printf("FAIL\n");
			continue;
		}
		
		/*Wait to receive the response*/	
		node_t response_server;
		char in_msg[MAX_MSG_SIZE];
		socklen_t addr_len = sizeof(response_server);
		ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr *) &response_server, &addr_len);
		parse_and_print_response(in_msg, in_msg_len);
		
	}
	
}

void parse_and_print_response(char* in_msg, size_t length){
	
	char key[MAX_MSG_SIZE];
	int key_index = 0;
	char value[MAX_MSG_SIZE];
	int value_index = 0;
	
	int parsing_key = 1;
	
	char iterator;
	
	for (int i = 0; i < length; i++){
		iterator = in_msg[i];
		
		if (parsing_key && iterator != '\0'){			
			key[key_index] = iterator;		
			key_index++;
		} else if (parsing_key && iterator == '\0'){		
			parsing_key = 0;
			key_index = 0;	
		} else if (!parsing_key && iterator != '\0'){
			value[value_index] = iterator;
			value_index++;
		} else if (!parsing_key && iterator == '\0'){
			printf("%s %s\n", key, value);
			parsing_key = 1;
			value_index = 0;
		}
							
	}
	
	printf("%s %s\n", key, value);
	
	
}

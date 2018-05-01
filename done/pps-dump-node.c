#include <stdio.h>
#include <stdlib.h>

// for basic socket communication
#include <sys/socket.h>

#include "error.h"
#include "system.h"
#include "network.h"
#include "config.h"
#include "hashtable.h"

kv_pair_t* parse_and_print_response(char* in_msg, size_t length);

void print_kv_pair_list(kv_pair_t* kv_pair_list, size_t length);

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
			
		printf("SENDING PACKET TO %s %d\n", ip, port);	
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
		
		printf("RECEIVED RESPONSE FROM SERVER\n");
		
		kv_pair_t* kv_pair_list = parse_and_print_response(in_msg, in_msg_len);
		
		if (kv_pair_list == NULL){
			printf("FAIL\n");
			continue;
		}
		
		print_kv_pair_list(kv_pair_list, sizeof(kv_pair_list));
		
			
	}
	
}

void print_kv_pair_list(kv_pair_t* kv_pair_list, size_t size){
	
	for (int i = 0; i < size; i++){
		printf("%s %s", kv_pair_list[i].key, kv_pair_list[i].value);
	}

}

kv_pair_t* parse_and_print_response(char* in_msg, size_t length){
	
	size_t expected_nbr_kv_pair = (in_msg[3]) & (in_msg[2] << 8) & (in_msg[1] << 16) & (in_msg[0] << 24);
	
	kv_pair_t* kv_pair_list = calloc(expected_nbr_kv_pair, sizeof(kv_pair_t));
		
	char key[MAX_MSG_SIZE];
	int key_index = 0;
	char value[MAX_MSG_SIZE];
	int value_index = 0;
	
	size_t list_index = 0;
	
	int parsing_key = 1;
	
	char iterator;
	
	for (int i = 4; i < length; i++){
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
//			printf("%s %s\n", key, value);
						
			kv_pair_list[list_index] = create_kv_pair(key, value);
			list_index++;
									
			parsing_key = 1;
			value_index = 0;
		}
							
	}
		kv_pair_list[list_index] = create_kv_pair(key, value);
		list_index++;
	
	if (list_index != expected_nbr_kv_pair)
		return NULL;
	
//	printf("%s %s\n", key, value);
	
	return kv_pair_list;
}

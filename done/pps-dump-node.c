#include <stdio.h>
#include <stdlib.h>

// for basic socket communication
#include <sys/socket.h>

#include "error.h"
#include "system.h"
#include "network.h"
#include "config.h"
#include "hashtable.h"

void parse_and_print_response(char* in_msg, size_t length);

void print_kv_pair_list(kv_list_t kv_pair_list);

int main(void){
	
	//Set up socket
    int s = get_socket(1);
	
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
		char in_msg[MAX_MSG_SIZE];
		ssize_t in_msg_len = recv(s, in_msg, MAX_MSG_SIZE, 0);
		
		printf("RECEIVED RESPONSE FROM SERVER\n");
		
		parse_and_print_response(in_msg, in_msg_len);	
	}
	
}

void print_kv_pair_list(kv_list_t kv_pair_list){
	
	for (int i = 0; i < kv_pair_list.size; i++){
		printf("%s %s\n", kv_pair_list.list[i].key, kv_pair_list.list[i].value);
	}

}

/*
 * @brief Parse an incoming message to a list of key_value pairs
 * @param in_msg message to parse
 * @param length of the message
 * @return a list of kv_pair_t or NULL if the expected amount of key_value paris did not match
 */ 
void parse_and_print_response(char* in_msg, size_t length){
	
	if (length < 4){
		printf("length is under 4 FAIL\n");
		return;
	}
	
	/*The first four octets represent an unsigned value = number of expected pairs*/
	size_t expected_nbr_kv_pair = (in_msg[3]) & (in_msg[2] << 8) & (in_msg[1] << 16) & (in_msg[0] << 24);
	
	printf("expected number of kv pair = %lu\n", expected_nbr_kv_pair);
	
	kv_pair_t kv_pair_list[expected_nbr_kv_pair];
	
	kv_list_t kv_list;
	kv_list.list = kv_pair_list;
	kv_list.size = expected_nbr_kv_pair;
		
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
			
			/*If the number of key_value pairs exceeds, it fails*/
			if (list_index >= expected_nbr_kv_pair){
				printf("FAIL\n");
				
				/*Free the memory allocated by create_kv_pair*/
//				kv_list_free(&kv_list);				
				return;
			}
									
			parsing_key = 1;
			value_index = 0;
		}						
	}
		kv_pair_list[list_index] = create_kv_pair(key, value);
		list_index++;
	
	/*If the number of key_value pairs does not match i.e. is below expected amount, it fails*/
	if (list_index != expected_nbr_kv_pair){
		printf("FAIL\n");
		
		kv_list.size = list_index;
//		kv_list_free(&kv_list);		
		return;
	}
	
	print_kv_pair_list(kv_list);
	
	/*Free the memory allocated by create_kv_pair*/
//	kv_list_free(&kv_list);
	
	
//	printf("%s %s\n", key, value);
}

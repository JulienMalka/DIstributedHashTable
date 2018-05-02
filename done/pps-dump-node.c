#include <stdio.h>
#include <stdlib.h>

// for basic socket communication
#include <sys/socket.h>

#include "error.h"
#include "system.h"
#include "network.h"
#include "config.h"
#include "hashtable.h"

size_t parse_kv_pairs(char* in_msg, size_t length, kv_list_t* kv_list);

void print_kv_pair_list(kv_list_t kv_pair_list);

size_t parse_nbr_kv_pair(char* in_msg){
	return (in_msg[3]) | (in_msg[2] << 8) | (in_msg[1] << 16) | (in_msg[0] << 24);
}

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
		
		if (in_msg_len == -1){
			printf("FAIL\n");
			continue;
		}
		
		printf("RECEIVED RESPONSE FROM SERVER, in_msg = %c and length = %lu\n", in_msg[3], in_msg_len);
		
		
		kv_list_t* kv_list = malloc(sizeof(kv_list_t));
		kv_list->list = calloc(MAX_MSG_SIZE, sizeof(kv_pair_t));
		kv_list->size = parse_nbr_kv_pair(in_msg);
		
		size_t parsed_kv_pairs = parse_kv_pairs(&in_msg[4], in_msg_len - 4, kv_list);	
		
		printf("parsed_kv pairs = %lu\n", parsed_kv_pairs);
		
		print_kv_pair_list(*kv_list);
		
		printf("printed\n");
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
 * @return the number of key_value pairs matched
 */ 
size_t parse_kv_pairs(char* in_msg, size_t length, kv_list_t* kv_list){
	
	if (length < 4){
		printf("length is under 4 FAIL\n");
		return -1;
	}
		
	char key[MAX_MSG_SIZE];
	int key_index = 0;
	char value[MAX_MSG_SIZE];
	int value_index = 0;
	
	size_t list_index = 0;
	
	int parsing_key = 1;
	
	char iterator;
	
	for (int i = 0; i < length; i++){
		iterator = in_msg[i];
		printf("iterator i = %c\n", iterator);
		
		if (parsing_key && iterator != '\0'){			
			key[key_index] = iterator;		
			key_index++;
		} else if (parsing_key && iterator == '\0'){		
			parsing_key = 0;
			key[key_index] = '\0';
			key_index = 0;	
		} else if (!parsing_key && iterator != '\0'){
			value[value_index] = iterator;
			value_index++;
		} else if (!parsing_key && iterator == '\0'){
			
			value[value_index+1] = '\0';
						
			kv_list->list[list_index] = create_kv_pair(key, value);
			list_index++;
									
			parsing_key = 1;
			value_index = 0;
		}						
	}
		kv_list->list[list_index] = create_kv_pair(key, value);
	
	return list_index + 1;
	
}

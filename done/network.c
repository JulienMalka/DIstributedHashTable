#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"




error_code network_get(client_t client, pps_key_t key, pps_value_t *value){
	// Set up socket with a receive timeout of 1 s.
    int s = client.socket;
    
    // Load server address.
    node_t srv_addr = client.server;
    get_server_addr(PPS_DEFAULT_IP, PPS_DEFAULT_PORT, &srv_addr);

    pps_key_t request = key;

    // Prepare outgoing message with htonl.
    unsigned int out_msg;
    out_msg = htonl(request);

    // Send message.
    printf("Sending message to %s:%d: %d\n",
           PPS_DEFAULT_IP, PPS_DEFAULT_PORT, request);
    if (sendto(s, &out_msg, sizeof(out_msg), 0,
               (struct sockaddr *) &srv_addr, sizeof(srv_addr)) == -1)
      return ERR_NETWORK;

    // Receive response.
    pps_value_t in_msg;
    ssize_t in_msg_len = recv(s, &in_msg, sizeof(in_msg), 0);
    if (in_msg_len == sizeof(in_msg)) { 
		// Valid response.
        // Parse response with ntohl.
        pps_value_t response;
        response = ntohl(in_msg);
		*value = response;
	}	
    return ERR_NONE;
}

error_code network_put(client_t client, pps_key_t key, pps_value_t value){
	
    int s = client.socket;
    
    // Load server address.
    node_t srv_addr = client.server;
    get_server_addr(PPS_DEFAULT_IP, PPS_DEFAULT_PORT, &srv_addr);
    
    pps_key_t request = key;

    // Prepare outgoing message with htonl.
    unsigned int out_msg;
    out_msg = htonl(request);
    
	return ERR_NONE;
}

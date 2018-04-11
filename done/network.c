#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"

error_code network_get(client_t client, pps_key_t key, pps_value_t *value){

    pps_key_t request = key;

    // Prepare outgoing message with htonl.
    unsigned int out_msg;
    out_msg = htonl(request);

    // Send message.
    printf("Sending message to %s:%d: %d\n",
           PPS_DEFAULT_IP, PPS_DEFAULT_PORT, request);
    if (sendto(client.socket, &out_msg, sizeof(out_msg), 0,
               (struct sockaddr *) &client.server, sizeof(client.server)) == -1)
      return ERR_NETWORK;

    // Receive response.
    pps_value_t in_msg;
    ssize_t in_msg_len = recv(client.socket, &in_msg, sizeof(in_msg), 0);
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

	

    
	return ERR_NONE;
}

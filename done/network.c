#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"

error_code network_get(client_t client, pps_key_t key, pps_value_t *value){

  	unsigned char packet[1];

    // Prepare outgoing message with htonl.
    packet[0] = key;

    // Send message.


    for(int i=0; i<client.server.size;i++){

    if (sendto(client.socket, &packet, 1, 0,
               (struct sockaddr *) &client.server.nodes[i], sizeof(client.server.nodes[i])) == -1)
      return ERR_NETWORK;

    // Receive response.
    uint32_t in_msg = 0;
    ssize_t in_msg_len = recv(client.socket, &in_msg, sizeof(in_msg), 0);//, (struct sockaddr *) &client.server,(socklen_t *) sizeof(client.server));

    printf("obtained %ld with error %d, wanted %lu, val %d  or  %d",in_msg_len, errno, sizeof(in_msg), in_msg, ntohl(in_msg));

    if (in_msg_len == sizeof(in_msg)) {
		// Valid response.
        // Parse response with ntohl.
        pps_value_t response;
        printf("response is %d\n", ntohl(in_msg));
        response = ntohl(in_msg);
		*value = response;
    return ERR_NONE;
	}
}
    return ERR_NOT_FOUND;
}

error_code network_put(client_t client, pps_key_t key, pps_value_t value){

	unsigned char packet[5];
  int errors = 0;
  for(int i= 0; i<client.server.size;i++){
	// Prepare value with htonl.
    unsigned int value_formated;
    value_formated = htonl(value);


	packet[0] = key;
	packet[1] = value_formated >> 24;
	packet[2] = value_formated >> 16;
	packet[3] = value_formated >> 8;
	packet[4] = value_formated;

	if (sendto(client.socket, &packet, 5, 0,
          (struct sockaddr *) &client.server.nodes[i], sizeof(client.server.nodes[i])) == -1) errors +=1;

}

if(errors>=1){
  return ERR_NETWORK;
}else{
	return ERR_NONE;
}
}

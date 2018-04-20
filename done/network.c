#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"
#include <arpa/inet.h>
#include <stdlib.h>

error_code network_get(client_t client, pps_key_t key, pps_value_t *value)
{
  //unsigned char packet[1];

    // Prepare outgoing message with htonl.
    //packet[0] = key;

    // Send message.

    for(int i=0; i<client.server.size; i++) {
        int size_to_send = strlen(key);
        if (sendto(client.socket, &key, size_to_send, 0,
                   (struct sockaddr *) &client.server.nodes[i], sizeof(client.server.nodes[i])) == -1)
            return ERR_NETWORK;

        // Receive response.
        char* in_msg;
        ssize_t in_msg_len = recv(client.socket, &in_msg, sizeof(in_msg), 0);//, (struct sockaddr *) &client.server,(socklen_t *) sizeof(client.server));

        printf("obtained %ld with error %d, wanted %lu, val %s",in_msg_len, errno, sizeof(in_msg), in_msg);

        if (in_msg_len == sizeof(in_msg)) {
            // Valid response.
            // Parse response with ntohl.
            pps_value_t response;
            printf("response is %s\n", in_msg);
            response = in_msg;
            *value = response;
            return ERR_NONE;
        }
    }
    return ERR_NOT_FOUND;
}

error_code network_put(client_t client, pps_key_t key, pps_value_t value)
{

printf("KEY VALUE GOT BY NETWORK (%s,%s)", key, value);
    int errors = 0;
    for(int i= 0; i<client.server.size; i++) {
		char buffer[20];
        printf("Sending to %s : %d\n", inet_ntop(AF_INET, &client.server.nodes[i].sin_addr, buffer, 20), ntohs(client.server.nodes[i].sin_port));
        // Prepare value with htonl.
       // unsigned int value_formated;
      //  value_formated = htonl(value);


      char *result = calloc(strlen(key)+strlen(value)+1, sizeof(char));
      for(int i=0; i<strlen(key); i++){
          result[i] = key[i];
        }
        result[strlen(key)]='\0';
    for(int i=strlen(key)+1; i<strlen(key)+1+strlen(value); i++){
        result[i] = value[i-strlen(key)-1];
      }



      if (sendto(client.socket, result, strlen(key)+strlen(value)+1, 0,
                   (struct sockaddr *) &client.server.nodes[i], sizeof(client.server.nodes[i])) ==-1) errors +=1;

    }

    if(errors>=1) {
        return ERR_NETWORK;
    } else {
        return ERR_NONE;
    }
}

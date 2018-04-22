#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"
#include "network.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

error_code network_get(client_t client, pps_key_t key, pps_value_t *value)
{
    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
    for(int i=0; i<client.server.size; i++) {
        int size_to_send = strlen(key);
        error_code error = send_packet(client.socket, key, size_to_send, client.server.nodes[i]);
        if(error!=ERR_NONE) return error;

        char* in_msg = malloc(MAX_MSG_ELEM_SIZE);
        ssize_t in_msg_len = recv(client.socket, in_msg, MAX_MSG_ELEM_SIZE, 0);


        if (in_msg_len != -1) {
            if (in_msg_len==1 && in_msg[0]=='\0'){
              printf("Not found\n");
              free(in_msg);
              return ERR_NOT_FOUND;
            }
            printf("response is %s\n", in_msg);
            *value = in_msg;
            return ERR_NONE;
        }
    }
    printf("Not connection to servers\n");
    return ERR_NETWORK;
}

error_code network_put(client_t client, pps_key_t key, pps_value_t value)
{

M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
M_EXIT_IF_TOO_LONG(value, MAX_MSG_ELEM_SIZE, value.name);
if(key==NULL||value==NULL)return ERR_BAD_PARAMETER;

    int errors = 0;
    for(int i= 0; i<client.server.size; i++) {

      char* request = format_put_request(key, value);
      size_t request_len = strlen(key)+strlen(value)+1;
      error_code error_send = send_packet(client.socket, request, request_len, client.server.nodes[i]);
      free(request);
      int error_receive = recv(client.socket, NULL,0,0);
      if(error_send!=ERR_NONE ||error_receive==-1) errors++;
}
    if(errors>=1) {
        return ERR_NETWORK;
    } else {
        return ERR_NONE;
    }
}


error_code send_packet(int socket, const char* message, size_t size, node_t node){

if(message==NULL || size<0) return ERR_BAD_PARAMETER;

  int error = sendto(socket, message, size, 0,
             (struct sockaddr *) &node, sizeof(node));


  if(error==-1) return ERR_NETWORK;

  return ERR_NONE;
  }


  char* format_put_request(pps_key_t key, pps_value_t value){
    char *result = calloc(strlen(key)+strlen(value)+1, sizeof(char));
    for(int i=0; i<strlen(key); i++){
        result[i] = key[i];
      }
      result[strlen(key)]='\0';
      for(int i=strlen(key)+1; i<strlen(key)+1+strlen(value); i++){
      result[i] = value[i-strlen(key)-1];
      }

      return result;

  }


error_code parse_put_request(char* in_msg, size_t length, char* key, char* value){
    char* ret = memchr(in_msg, '\0', length);
    size_t size_key = strlen(in_msg);
    for(int i =0; i<size_key;i++){
     key[i] = in_msg[i];
   }
   key[size_key] = '\0';
   size_t size_value = length - size_key;

 for(int i=0; i<size_value; i++){
   printf("index %d of value = %c\n", i, ret[i+1]);
   value[i] = ret[i+1];
}

 value[size_value] = '\0';
return ERR_NONE;

}

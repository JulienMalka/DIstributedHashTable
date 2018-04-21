
// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>


int main(void)
{
    //Set up socket
    int s;
    s = get_socket(0);

    Htable_t h_table = construct_Htable(HTABLE_SIZE);

   int ok = 1;
    char ip[15]; //max 15 characters in an ip adress
    int port;
    while(ok) {
        printf("IP port? ");
        int error = scanf(" %s %d", ip, &port);
        if (error != 1) ok = 0;
        else {
            printf("FAIL\n");
        }
    }

    //Bind server to the address:port
  error_code error_bind =  bind_server(s, ip, port);

  if(error_bind!=ERR_NONE){
    printf("FAILED TO BIND IP"); //TO CHANGE
  }

    //Receive messages forever
    while(1) {
        node_t cli_addr;
        socklen_t addr_len = sizeof(cli_addr);
        memset(&cli_addr, 0, addr_len);

         char* in_msg = malloc(MAX_MSG_SIZE);
        printf("preparing to receive something\n");
        ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &addr_len);
        if (in_msg_len>MAX_MSG_SIZE) { // Wrong message size.
            printf("in_msg_len = %lu\n", in_msg_len);
            printf("sizeof(in:msg) = %lu\n", sizeof(in_msg));
            printf("Received invalid message\n");
            continue;
        }

        printf("Received something\n");
        printf("%lu\n", sizeof(in_msg));

       char tolook = '\0';

        // Write Request
        if (memchr(in_msg, tolook, in_msg_len)!=NULL) {
            printf("Received write request \n");
			       char* ret = memchr(in_msg, tolook, in_msg_len);
            size_t size_key = strlen(in_msg);
            char* key = calloc(size_key+1, sizeof(char));
            for(int i =0; i<size_key;i++){
			  printf("index %d of key = %c\n", i, in_msg[i]);
              key[i] = in_msg[i];
            }

            key[size_key] = '\0';
            printf("parsed key\n");
            size_t size_value = in_msg_len - size_key;
            printf("size of value = %lu and size of in_msg = %lu\n", size_value, in_msg_len);
            char* value = calloc(size_value+1, sizeof(char));
            for(int i=0; i<size_value; i++){
			  printf("index %d of value = %c\n", i, ret[i+1]);
              value[i] = ret[i+1];
            }

            value[size_value] = '\0';

			printf("Before adding to htable (key, value) = (%s, %s)\n", key, value);
            add_Htable_value(h_table, key, value);

            printf("write request = (%s, %s)... sending response\n", key, value);

            sendto(s, 0, 0, 0,
                   (struct sockaddr *) &cli_addr, sizeof(cli_addr));
        }

        // Read Request
        if (memchr(in_msg, 0, in_msg_len)==NULL) {

            char* request = in_msg;
            printf("%s\n", request);

            pps_value_t get = get_Htable_value(h_table, request);

            printf("read request = %s ... sending response = %s\n", request, get);



            sendto(s, get, 5, 0,
                   (struct sockaddr *) &cli_addr, sizeof(cli_addr));
        }
    }

    return 1;

}

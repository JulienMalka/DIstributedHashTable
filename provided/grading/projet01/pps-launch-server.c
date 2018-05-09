
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
    int s = get_socket(0);

  Htable_t h_table = construct_Htable(HTABLE_SIZE);

   int ok = 1;
   /* CORRECTEUR: `ok` is not a very descriptive (and rather misleading) name
    * [-0.1 style]
    */
    char ip[15]; //max 15 characters in an ip adress
    /* CORRECTEUR: 4*3 + 3*1 + 1 (terminating zero-byte) = 16 [-0.5 correctness]
     */
    int port;
    while(ok) {
        printf("IP port? ");
        int error = scanf(" %s %d", ip, &port);
        /* CORRECTEUR: What if the input is longer than 15 bytes?
         * [-2 robustness]
         */
        error_code error_bind =  bind_server(s, ip, port);
        if (error != 1 &&error_bind==ERR_NONE) ok = 0;
        else {
            printf("FAIL\n");
        }
    }

    //Bind server to the address:port


    //Receive messages forever
    while(1) {
        node_t cli_addr;
        socklen_t addr_len = sizeof(cli_addr);
        memset(&cli_addr, 0, addr_len);
        char* in_msg = malloc(MAX_MSG_SIZE);
        ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &addr_len);
        if (in_msg_len>MAX_MSG_SIZE) { // Wrong message size.
            printf("FAIL\n");
            continue;
        }



        // Write Request
        if (memchr(in_msg, '\0', in_msg_len)!=NULL) {
              size_t size_value = in_msg_len - strlen(in_msg);
              char* key = calloc(strlen(in_msg)+1, sizeof(char));
              char* value = calloc(size_value, sizeof(char));
              parse_put_request(in_msg, in_msg_len, key, value);
              add_Htable_value(h_table, key, value);
              sendto(s, 0, 0, 0,
                   (struct sockaddr *) &cli_addr, sizeof(cli_addr));
              /* CORRECTEUR: The second argument in `sendto` (`message`) should
               * be a pointer, not an number. If you wish to send empty data,
               * have it be `NULL`, for instance. [-2 correctness/compilation]
               */
        }

        // Read Request
        if (memchr(in_msg, 0, in_msg_len)==NULL) {
            char* request = in_msg;
            pps_value_t get = malloc(MAX_MSG_ELEM_SIZE);
            get = get_Htable_value(h_table, request);
            if(get!=NULL){

            sendto(s, get, MAX_MSG_ELEM_SIZE, 0,
                   (struct sockaddr *) &cli_addr, sizeof(cli_addr));
            /* CORRECTEUR: If the length argument is `MAX_MSG_ELEM_SIZE`, this
             * will send garbage at the end (because `get` is likely shorter
             * than `MAX_MSG_ELEM_SIZE`) [-1 correctness]
             */
                 }else{
                   char* msg = calloc(1, sizeof(char));
                   msg[0] = '\0';
                   sendto(s, msg, 1, 0,
                          (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                   /* CORRECTEUR: `msg` is not properly freed [-1 memory leak]
                    */


                 }
            /* CORRECTEUR: What if `sendto` fails? [-1 robustness] */
        }
    }

    return 1;

}

/* CORRECTEUR: Consider modularising your code a bit. This is a single, 70 lines
 * function that does *everything*. [-0.25 style]
 * CORRECTEUR: Generally inconsistent indentation [-0.5 style]
 */

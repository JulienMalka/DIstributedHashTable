
// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "network-utils.h"
#include "config.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>

/**
 * Main executable, launches a server on given ip and port
 */

int main(void)
{
    //Set up socket
    int s = get_socket(0);

    Htable_t h_table = construct_Htable(HTABLE_SIZE);
    /* CORRECTEUR: What if construct_Htable fails? [-1 robustness]
     */

    int ok = 1;
    /* CORRECTEUR: `ok` is not a terribly descriptive (and rather misleading)
     * name [-0.1 style]
     */
    char ip[16]; //max 15 characters in an ip adress
    int port;
    while(ok) {
        printf("IP port? ");
        int error = scanf(" %s %d", ip, &port);
        if(strlen(ip)>15) {
        /* CORRECTEUR: No, the ip *buffer* holds only 16 bytes, so the scanf can
         * potentially cause an overflow. Consider scanning with "%15s" (see
         * scanf(3) for more details).
         * [-0.5 robustness]
         */
            printf("FAIL\n");
        }
        /* CORRECTEUR: Might want to check the return value of scanf first.
         * `error` is used below, but... it's a bit convoluted (consider
         * checking if it's 2 instead).
         * [-0.5 robustness]
         */
        error_code error_bind =  bind_server(s, ip, port);

        if (error != 1 && error_bind == ERR_NONE)
            ok = 0;
        else {
            printf("FAIL\n");
        }
    }

    //Bind server to the address:port
    /* CORRECTEUR: ? That's done above, no? */


    //Receive messages forever
    while(!feof(stdin)) {

        node_t cli_addr;
        socklen_t addr_len = sizeof(cli_addr);
        memset(&cli_addr, 0, addr_len);
        char* in_msg = malloc(MAX_MSG_SIZE);
        ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &addr_len);
        if (in_msg_len > MAX_MSG_SIZE) { // Wrong message size.
            printf("FAIL\n");
            continue;
        }

        if(in_msg_len==0) {
            sendto(s, NULL, 0, 0, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
        }
        
        // Write Request
        else if (memchr(in_msg, '\0', in_msg_len)!=NULL) {

            if(in_msg_len == 1) {

                kv_list_t* node_dump = get_Htable_content(h_table);
                /* CORRECTEUR: What if get_Htable_content fails?
                 * [-1 robustness]
                 */

                size_t counter = 0;
                size_t size_packet =0;
                /* 65507 max packet size */
                char* packet = calloc(65507, sizeof(char));
                /* CORRECTEUR: Consider making this a macro/constant. */
                char header[4];
                header[0] = node_dump->size >> 24;
                header[1] = node_dump->size >> 16;
                header[2] = node_dump->size >> 8;
                header[3] = node_dump->size;
                /* CORRECTEUR: Consider using htonl and memcpy here - this is
                 * veeeeery fragile (and ignores compiler warnings)
                 * [-0.1 compilation]
                 */

                while(counter < node_dump->size) {

                    size_t size_kv = strlen(node_dump->list[counter].key) +1 + strlen(node_dump->list[counter].value);
                    if(size_packet + size_kv < 65507) {
                        char* kv_request;
                        if(counter == 0) {
                            char* key_new = calloc(4+strlen(node_dump->list[counter].key), sizeof(char));
                            for(int i=0; i<4; i++) {
                                key_new[i] = header[i];

                            }
                            /* CORRECTEUR: Again, consider using memcpy here. */
                            for(size_t i=0; i<strlen(node_dump->list[counter].key); i++) {

                                key_new[4+i] = node_dump->list[counter].key[i];
                            }
                            /* CORRECTEUR: memcpy... */

                            kv_request = format_put_request(key_new, node_dump->list[counter].value, 4+strlen(node_dump->list[counter].key) , -1);
                            for(size_t i = 0; i< 5 + strlen(node_dump->list[counter].key)+ strlen(node_dump->list[counter].value); i++) {

                                packet[i] =  kv_request[i];
                            }
                            /* CORRECTEUR: memcpy... */
                            size_packet+= 5 + strlen(node_dump->list[counter].key)+ strlen(node_dump->list[counter].value);
                            counter++;
                            continue;

                            /* CORRECTEUR: Magic values everywhere! [-2 malus]
                             */

                        } else {
                            kv_request = format_put_request(node_dump->list[counter].key, node_dump->list[counter].value, -1, -1);
                        }
                        packet = format_put_request(packet, kv_request, size_packet, strlen(node_dump->list[counter].key)+1 + strlen(node_dump->list[counter].value));

                        counter++;
                        size_packet+=size_kv+1;

                        /* CORRECTEUR: kv_request is never freed [-1] */

                    } else {

                        sendto(s, packet, size_packet, 0,
                               (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                        size_packet = 0;
                    }

                    /* CORRECTEUR: packet is never freed [-1] */
                }
                sendto(s, packet, size_packet, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));


            } else {

                size_t size_value = in_msg_len - strlen(in_msg);
                char* key = calloc(strlen(in_msg)+1, sizeof(char));
                char* value = calloc(size_value, sizeof(char));
                /* CORRECTEUR: What if one of those callocs fail?
                 * [-1 robustness]
                 */
                parse_put_request(in_msg, in_msg_len, key, value);
                if (add_Htable_value(h_table, key, value) == ERR_NONE)
					sendto(s, NULL, 0, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
            }
        }

        // Read Request
        else if (memchr(in_msg, 0, in_msg_len)==NULL) {
            char* request = in_msg;
            pps_value_t get = malloc(MAX_MSG_ELEM_SIZE);
            get = get_Htable_value(h_table, request);
            if(get != NULL) {

                sendto(s, get, MAX_MSG_ELEM_SIZE, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                /* CORRECTEUR: If the length argument is `MAX_MSG_ELEM_SIZE`, this
                 * will send garbage at the end (because `get` is likely shorter
                 * than `MAX_MSG_ELEM_SIZE`) [-1 correctness]
                 */
            } else {
                char* msg = calloc(1, sizeof(char));
                msg[0] = '\0';
                sendto(s, msg, 1, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                /* CORRECTEUR: `msg` is not properly freed [-1 memory leak]
                 */

            }
        }
    }

    return 1;

}
/* CORRECTEUR: Consider modularising your code. This is a single 140+ SLOC long
 * function that does *everything* [-0.5 style]
 * Also, some lines are waaaaaay too long. Consider using additional variables
 * to store intermediate results/arguments rather than computing them on the fly
 * when passing as arguments [-0.25 style]
 */


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


int main(void)
{
    //Set up socket
    int s = get_socket(0);

    Htable_t h_table = construct_Htable(HTABLE_SIZE);

    int ok = 1;
    char ip[15]; //max 15 characters in an ip adress
    int port;
    while(ok) {
        printf("IP port? ");
        int error = scanf(" %s %d", ip, &port);
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


        if(in_msg_len==0) {

            sendto(s, NULL, 0, 0, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
        }
        // Write Request
        else if (memchr(in_msg, '\0', in_msg_len)!=NULL) {

            if(in_msg_len==1) {

                kv_list_t* node_dump = get_Htable_content(h_table);

                int counter =0;
                size_t size_packet =0;
                char* packet = calloc(65507, sizeof(char));
                char header[4];
                header[0] = node_dump->size >> 24;
                header[1] = node_dump->size >> 16;
                header[2] = node_dump->size >> 8;
                header[3] = node_dump->size;

                while(counter < node_dump->size) {

                    size_t size_kv = strlen(node_dump->list[counter].key) +1 + strlen(node_dump->list[counter].value);
                    if(size_packet+size_kv<65507) {
                        char* kv_request;
                        if(counter==0) {
                            char* key_new = calloc(4+strlen(node_dump->list[counter].key), sizeof(char));
                            for(int i=0; i<4; i++) {
                                key_new[i] = header[i];

                            }
                            for(int i=0; i<strlen(node_dump->list[counter].key); i++) {

                                key_new[4+i] = node_dump->list[counter].key[i];
                            }

                            kv_request = format_put_request(key_new, node_dump->list[counter].value, 4+strlen(node_dump->list[counter].key) , -1);
                            for(int i = 0; i< 5 + strlen(node_dump->list[counter].key)+ strlen(node_dump->list[counter].value); i++) {

                                packet[i] =  kv_request[i];


                            }


                            size_packet+= 5 + strlen(node_dump->list[counter].key)+ strlen(node_dump->list[counter].value);
                            counter++;
                            continue;


                        } else {
                            kv_request = format_put_request(node_dump->list[counter].key, node_dump->list[counter].value, -1, -1);
                        }
                        packet = format_put_request(packet, kv_request, size_packet, strlen(node_dump->list[counter].key)+1 + strlen(node_dump->list[counter].value));

                        counter++;
                        size_packet+=size_kv+1;

                    } else {

                        sendto(s, packet, size_packet, 0,
                               (struct sockaddr *) &cli_addr, sizeof(cli_addr));



                        size_packet = 0;



                    }

                }
                sendto(s, packet, size_packet, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));


            } else {


                size_t size_value = in_msg_len - strlen(in_msg);
                char* key = calloc(strlen(in_msg)+1, sizeof(char));
                char* value = calloc(size_value, sizeof(char));
                parse_put_request(in_msg, in_msg_len, key, value);
                add_Htable_value(h_table, key, value);
                sendto(s, 0, 0, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
            }
        }

        // Read Request
        else if (memchr(in_msg, 0, in_msg_len)==NULL) {
            char* request = in_msg;
            pps_value_t get = malloc(MAX_MSG_ELEM_SIZE);
            get = get_Htable_value(h_table, request);
            if(get!=NULL) {

                sendto(s, get, MAX_MSG_ELEM_SIZE, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
            } else {
                char* msg = calloc(1, sizeof(char));
                msg[0] = '\0';
                sendto(s, msg, 1, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));


            }
        }
    }

    return 1;

}

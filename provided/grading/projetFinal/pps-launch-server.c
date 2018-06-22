
// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "network-utils.h"
#include "config.h"
#include "error.h"
#include "hashtable.h"
#include <string.h>
#include <stdlib.h>

/**
 * Main executable, launches a server on given ip and port
 */

int main(void)
{
    //Set up socket
    int s = get_socket(0);

    Htable_t *h_table = construct_Htable(HTABLE_SIZE);
    /* CORRECTEUR: What if construct_Htable fails? [-1 robustness]
     */

    int not_parsed = 1;
    char ip[16]; //max 15 characters in an ip adress
    int port;
    while (not_parsed) {
        printf("IP port? ");
        int count_rec = scanf(" %15s %d", ip, &port);
        if (strlen(ip) > 15) {
        /* CORRECTEUR: unnecessary check (this should never happen) */
            printf("FAIL\n");
        }
        /* CORRECTEUR: might want to check count_rec first, before trying to
         * call bind_server (which expects `ip` to be a correct string)
         * [-1]
         */
        error_code error_bind = bind_server(s, ip, port);

        if (count_rec == 2 && error_bind == ERR_NONE)
            not_parsed = 0;
        else {
            printf("FAIL\n");
        }
    }

    //Receive messages forever
    while (!feof(stdin)) {

        node_t cli_addr;
        socklen_t addr_len = sizeof(cli_addr);
        memset(&cli_addr, 0, addr_len);
        char *in_msg = malloc(MAX_MSG_SIZE);
        if (in_msg == NULL) {
            printf("FAIL\n");
            continue;
            /* CORRECTEUR: so... this will loop forever until malloc no longer
             * fails? [-1] ò_ô
             */
        }
        ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &addr_len);
        if (in_msg_len > MAX_MSG_SIZE) { // Wrong message size.
            printf("FAIL\n");
            continue;
        }

        if (in_msg_len == 0) {
            sendto(s, NULL, 0, 0, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
        }

        // Write Request
        else if (memchr(in_msg, '\0', in_msg_len) != NULL) {

            if (in_msg_len == 1) {

                kv_list_t *node_dump = get_Htable_content(*h_table);
                /* CORRECTEUR: What if get_Htable_content fails?
                 * [-1 robustness]
                 */

                size_t counter = 0;
                size_t size_packet = 0;
                /* 65507 max packet size */
                char *packet = calloc(MAX_MSG_SIZE, sizeof(char));
                if (packet == NULL) {
                    printf("FAIL\n");
                    continue;
                }
                char header[4];
                uint32_t converted_size = htonl((uint32_t) node_dump->size);
                memcpy(header, &converted_size, 4);


                while (counter < node_dump->size) {

                    size_t size_kv = strlen(node_dump->list[counter].key) + 1 + strlen(node_dump->list[counter].value);
                    if (size_packet + size_kv < MAX_MSG_SIZE) {
                        char *kv_request;
                        if (counter == 0) {
                            char *key_new = calloc(4 + strlen(node_dump->list[counter].key), sizeof(char));
                            for (int i = 0; i < 4; i++) {
                                key_new[i] = header[i];

                            }
                            for (size_t i = 0; i < strlen(node_dump->list[counter].key); i++) {

                                key_new[4 + i] = node_dump->list[counter].key[i];
                            }

                            kv_request = format_put_request(key_new, node_dump->list[counter].value,
                                                            4 + strlen(node_dump->list[counter].key), -1);
                            for (size_t i = 0; i < 5 + strlen(node_dump->list[counter].key) +
                                 strlen(node_dump->list[counter].value); i++) {

                                packet[i] = kv_request[i];
                            }
                            /* CORRECTEUR: for most of above: memcpy! */
                            size_packet +=
                                5 + strlen(node_dump->list[counter].key) + strlen(node_dump->list[counter].value);
                            /* CORRECTEUR: magic values! magic values everywhere!
                             * [-2 malus]
                             */

                            counter++;
                            continue;

                        } else {
                            kv_request = format_put_request(node_dump->list[counter].key,
                                                            node_dump->list[counter].value, -1, -1);
                        }
                        packet = format_put_request(packet, kv_request, size_packet,
                                                    strlen(node_dump->list[counter].key) + 1 +
                                                    strlen(node_dump->list[counter].value));
                        /* CORRECTEUR: memory leak (packet) [-1]
                         * CORRECTEUR: also, see comment in network-utils.c [-1]
                         */

                        counter++;
                        size_packet += size_kv + 1;
                        /* CORRECTEUR: kv_request is never freed [-1] */

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
                char *key = calloc(strlen(in_msg) + 1, sizeof(char));
                char *value = calloc(size_value, sizeof(char));
                if (key == NULL || value == NULL) {
                    printf("FAIL\n");
                    continue;
                }
                parse_put_request(in_msg, in_msg_len, key, value);
                if (add_Htable_value(*h_table, key, value) == ERR_NONE)
                    sendto(s, NULL, 0, 0,
                           (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                /* CORRECTEUR: key and value are never freed [-1] */
            }
        }

        // Read Request
        else if (memchr(in_msg, 0, in_msg_len) == NULL) {
            char *request = in_msg;
            pps_value_t get = malloc(MAX_MSG_ELEM_SIZE);
            if (get == NULL) {
                printf("FAIL\n");
                continue;
            }
            get = get_Htable_value(*h_table, request);
            if (get != NULL) {

                sendto(s, get, MAX_MSG_ELEM_SIZE, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                /* CORRECTEUR: If the length argument is `MAX_MSG_ELEM_SIZE`, this
                 * will send garbage at the end (because `get` is likely shorter
                 * than `MAX_MSG_ELEM_SIZE`) [-1 correctness]
                 */
            } else {
                char *msg = calloc(1, sizeof(char));
                msg[0] = '\0';
                sendto(s, msg, 1, 0,
                       (struct sockaddr *) &cli_addr, sizeof(cli_addr));
                /* CORRECTEUR: msg is never freed [-1] */

            }
        }
    }

    return 1;

}
/* CORRECTEUR: Consider modularising your code. This is a single 140+ SLOC long
 * function that does *everything* [-0.5 style]
 */

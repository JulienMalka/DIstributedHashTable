#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "network-utils.h"
#include "system.h"
#include "network.h"
#include "args.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "ring.h"


/**
 * @brief get a value from the network
 * @param client client to use
 * @param key key of what we want to find value
 * @param value value to write to, of size MAX_MSG_ELEM_SIZE + 1
 * @return an error code
 */
error_code network_get(client_t client, pps_key_t key, pps_value_t *value)
{

    int s = get_socket(1);
    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
    Htable_t *local_h_table = construct_Htable(HTABLE_SIZE);
    /* CORRECTEUR: what if construct_Htable fails? [-1] */

    node_list_t *nodes = ring_get_nodes_for_key(&client.server, client.args->N, key);

    int error_not_found = 0;
    for (size_t i = 0; i < client.args->N; i++) {
        size_t size_to_send = strlen(key);
        send_packet(s, key, size_to_send, nodes->nodes[i]);
    }


    char *in_msg = malloc(MAX_MSG_ELEM_SIZE);
    if (in_msg == NULL) {
        return ERR_NOMEM;
    }
    ssize_t in_msg_len = 0;
    while ((in_msg_len = recv(s, in_msg, MAX_MSG_ELEM_SIZE, 0)) != -1) {

        in_msg[in_msg_len] = '\0';

        if (in_msg_len != -1) {
            if (in_msg_len == 1 && in_msg[0] == '\0') {
                error_not_found++;
            } else {
                //	char* count = strcpy(
                pps_value_t count_val = get_Htable_value(*local_h_table, in_msg);
                char *count = calloc(MAX_MSG_ELEM_SIZE, sizeof(char));

                if (count_val == NULL) {

                    count[0] = 0;
                    count[1] = '\0';
                } else {

                    count = strcpy(count, count_val);
                }

                count[0]++;

                if (count[0] >= (int) client.args->R) {
                    *value = in_msg;
                    //        free(in_msg); (it's indeeed freed elsewhere)
                    return ERR_NONE;
                }
                add_Htable_value(*local_h_table, in_msg, count);

            }
        }

    }

    node_list_free(nodes);

    if (!error_not_found) return ERR_NETWORK;
    else return ERR_NOT_FOUND;
    /* CORRECTEUR: should free in_msg [-1] */
}


/**
 * @brief put a value in the network
 * @param client client to use
 * @param key key to retrieve value later
 * @param value value to add
 * @return an error code
 */
error_code network_put(client_t client, pps_key_t key, pps_value_t value)
{


    int s = get_socket(1);
    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
    M_EXIT_IF_TOO_LONG(value, MAX_MSG_ELEM_SIZE, value.name);
    if (key == NULL || value == NULL) return ERR_BAD_PARAMETER;

    node_list_t *nodes = ring_get_nodes_for_key(&client.server, client.args->N, key);


    for (size_t i = 0; i < client.args->N; i++) {

        char *request = format_put_request(key, value, -1, -1);
        size_t request_len = strlen(key) + strlen(value) + 1;
        send_packet(s, request, request_len, nodes->nodes[i]);
        free(request);
    }

    ssize_t error_receive;
    size_t success = 0;
    while ((error_receive = recv(s, NULL, 0, 0)) != -1) {

        if (error_receive != -1) success++;

        if (success >= client.args->W) {
            node_list_free(nodes);
            return ERR_NONE;
        }

    }

    node_list_free(nodes);
    return ERR_NETWORK;

}

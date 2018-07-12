#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "network-utils.h"
#include "system.h"
#include "network.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief get a value from the network
 * @param client client to use
 * @param key key of what we want to find value
 * @param value value to write to, of size MAX_MSG_ELEM_SIZE + 1
 * @return an error code
 */
error_code network_get(client_t client, pps_key_t key, pps_value_t *value)
{
    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
    Htable_t local_h_table = construct_Htable(HTABLE_SIZE);
    /* CORRECTEUR: What construct_Htable fails? [-1 robustness] */


    int error_not_found = 0;
    for(size_t i=0; i<client.args->N; i++) {
        int size_to_send = strlen(key);
        send_packet(client.socket, key, size_to_send, client.server.nodes[i]);
        /* CORRECTEUR: To complement my last comment on this: send_packet may
         * still fail, and if it does, there is no point trying to read
         * (although there will be likely a timeout and everything will work out
         * fine - but semantically, it's still not correct (and takes more
         * time)) [-0.5 robustness]
         */

        char* in_msg = malloc(MAX_MSG_ELEM_SIZE);
        /* CORRECTEUR: What if malloc fails? [-0.5 robustness] */
        ssize_t in_msg_len = recv(client.socket, in_msg, MAX_MSG_ELEM_SIZE, 0);
        /* CORRECTEUR: Consider using `recvfrom`... ah screw it - you'll have
         * done it anyway for the last submission (with parallel requests).
         * [also no points deducted because technically we don't care here]
         */
        in_msg[in_msg_len] = '\0';

        if (in_msg_len != -1) {
            if (in_msg_len==1 && in_msg[0]=='\0') {
                error_not_found++;
            } else {
                //	char* count = strcpy(
                char* count = get_Htable_value(local_h_table, in_msg);
                /* CORRECTEUR: This returns a pps_value_t (i.e. `char const *`),
                 * so you mustn't modify it directly [-0.5 compilation]
                 */
                if(count == NULL) {
                    char count_c = 0;
                    count = &count_c;
                    /* CORRECTEUR: `count_c` is not a NULL-terminated string,
                     * but just an ordinary pointer to a character - this will
                     * likely blow up somewhere... [-0.5 execution]
                     */
                }
                count[0]++;

                if(count[0] >= client.args->R) {
                    *value = in_msg;
                    //        free(in_msg); (it's indeeed freed elsewhere)
                    return ERR_NONE;
                }
                add_Htable_value(local_h_table, in_msg, count);

            }
        }

    }

    if(error_not_found==0) return ERR_NETWORK;
    else return ERR_NOT_FOUND;
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

    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
    M_EXIT_IF_TOO_LONG(value, MAX_MSG_ELEM_SIZE, value.name);
    if(key==NULL||value==NULL)return ERR_BAD_PARAMETER;

    size_t errors = 0;
    for(size_t i= 0; i<client.args->N; i++) {

        char* request = format_put_request(key, value, -1, -1);
        size_t request_len = strlen(key)+strlen(value)+1;
        error_code error_send = send_packet(client.socket, request, request_len, client.server.nodes[i]);
        free(request);
        int error_receive = recv(client.socket, NULL,0,0);
        if(error_send!=ERR_NONE ||error_receive==-1) errors++;
    }
    if(errors>client.args->N-client.args->W) {
        return ERR_NETWORK;
    } else {
        return ERR_NONE;
    }
}

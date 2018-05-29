#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"
//#include "network.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>



/**
 * @brief send a packet to a node
 * @param message the packet to send
 * @param socket socket to use
 * @param size size of the packet
 * @param node the node we want to send the packet to
 * @return an error code
 */
error_code send_packet(int socket, const char* message, size_t size, node_t node)
{

    if(message == NULL) return ERR_BAD_PARAMETER;

    ssize_t error = sendto(socket, message, size, 0,
                           (struct sockaddr *) &node, sizeof(node));


    if(error==-1) return ERR_NETWORK;

    return ERR_NONE;
}



/**
 * @brief format a put request with the caracter \0 correctly placed
 * @param value value to format
 * @param key key to format
 * @return the request correctly formated
 */
char* format_put_request(pps_value_t key, pps_value_t value, int skey, int svalue)
{

    int size_key;
    int size_value;
    if(skey==-1) {
        size_key = strlen(key);
    } else {
        size_key = skey;
    }
    if(svalue==-1) {
        size_value = strlen(value);
    } else {
        size_value=svalue;
    }
    char *result = calloc(size_key+size_value+1, sizeof(char));
    for(int i=0; i<size_key; i++) {
        result[i] = key[i];
    }
    result[size_key]='\0';
    for(int i=size_key+1; i<size_key+1+size_value; i++) {
        result[i] = value[i-size_key-1];
    }

    return result;
}

/**
 * @brief parse a put request
 * @param in_msg the packet to parse
 * @param length length of request
 * @param key pointer to the key
 * @param value pointer to the value
 * @return an error code
 */
error_code parse_put_request(char* in_msg, size_t length, char* key, char* value)
{
    char* ret = memchr(in_msg, '\0', length);
    size_t size_key = strlen(in_msg);
    for( size_t i =0; i<size_key; i++) {
        key[i] = in_msg[i];
    }
    key[size_key] = '\0';
    size_t size_value = length - size_key;

    for(size_t i = 0 ; i < size_value; i++) {

        value[i] = ret[i+1];
    }

    value[size_value] = '\0';
    return ERR_NONE;

}

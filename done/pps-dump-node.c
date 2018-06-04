#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"

// for basic socket communication
#include <sys/socket.h>

#include "error.h"
#include "system.h"
#include "network.h"
#include "config.h"
#include "hashtable.h"
#include "network-utils.h"

size_t parse_kv_pairs(const char *in_msg, ssize_t length, size_t starting_index, kv_list_t *kv_list);

void print_kv_pair_list(kv_list_t kv_pair_list);

/**
 * @brief Takes the four first bytes of a char array and converts them into a 32-bit unsigned char
 * @param in_msg to parse, in big-endian
 * @return 32-bit unsigned int
 */
size_t parse_nbr_kv_pair(const char *in_msg)
{
    size_t nbr = 0;
    memcpy(&nbr, in_msg, 4);
    return (size_t) ntohl((uint32_t) nbr);
}

/**
 * @brief Dump content of given node (ip, port)
 * As follows : pps-dump-node <IP> <Port>
 * @return 0 on normal exit, -1 otherwise
 */
int main(int argc, char *argv[])
{

    /* Client initialization and parsing optionnal arguments */
    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.argv = &argv;
    client_i.required = 2;
    client_i.optionnal = 0;
    client_i.size_args = argc;

    error_code error_init = client_init(client_i);

    if (error_init != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    char *ip;
    uint16_t port;

    /* parse ip and port */
    if (argv[0] != NULL && argv[1] != NULL) {
        ip = argv[0];
        port = (uint16_t) strtol(argv[1], NULL, 10);
    } else {
        client_end(&client);
        printf("FAIL\n");
        return -1;
    }

    /* Set up socket */
    int s = get_socket(1);

    node_t node;
    node_init(&node, ip, port, 0);

    /* Send packet to node */
    if (send_packet(s, "\0", 1, node) != ERR_NONE) {
        client_end(&client);
        //error handling
        printf("FAIL\n");
        return -1;
    }

    /* Wait to receive the response */
    char in_msg[MAX_MSG_SIZE];
    ssize_t in_msg_len = recv(s, in_msg, MAX_MSG_SIZE, 0);

    if (in_msg_len == -1) {
        client_end(&client);
        printf("FAIL\n");
        return -1;
    }

    kv_list_t *kv_list = malloc(sizeof(kv_list_t));
    kv_list->list = calloc(MAX_MSG_SIZE, sizeof(kv_pair_t));
    kv_list->size = parse_nbr_kv_pair(in_msg);

    /* 4 is the size (in bytes) of a 32-bit unsigned integer */
    size_t parsed_kv_pairs = parse_kv_pairs(&in_msg[4], in_msg_len - 4, 0, kv_list);

    if ((int) parsed_kv_pairs == -1) {
        kv_list_free(kv_list);
        client_end(&client);

        printf("FAIL\n");
        return -1;
    }

    /* More packets handling */
    while (parsed_kv_pairs < kv_list->size) {

        size_t startingIndex = parsed_kv_pairs;
        in_msg_len = recv(s, in_msg, MAX_MSG_SIZE, 0);

        size_t more_kv_pairs = parse_kv_pairs(in_msg, in_msg_len, startingIndex, kv_list);

        if (more_kv_pairs == (size_t) -1) {
            printf("FAIL\n");
            return -1;
        }

        parsed_kv_pairs += more_kv_pairs;

        if (in_msg_len == -1 && parsed_kv_pairs != kv_list->size) {
            printf("FAIL\n");
            return -1;
        }


    }

    if (parsed_kv_pairs != kv_list->size) {
        printf("FAIL\n");
        return -1;
    }

    print_kv_pair_list(*kv_list);

    kv_list_free(kv_list);

    client_end(&client);

    return 0;

}

/**
 * @brief Iterate over a list of key_value and prints its content
 * @param kv_pair_list to print
 */
void print_kv_pair_list(kv_list_t kv_pair_list)
{
    for (size_t i = 0; i < kv_pair_list.size; i++) {
        printf("%s = %s\n", kv_pair_list.list[i].key, kv_pair_list.list[i].value);
    }
}

/**
 * @brief Parse an incoming message to a list of key_value pairs
 * @param in_msg message to parse
 * @param length of the message
 * @param starting_index to insert elements in @param kv_list
 * @param kv_list pointer to a kv_list
 * @return the number of key_value pairs parsed, -1 (unsigned) if parsing failed
 */
size_t parse_kv_pairs(const char *in_msg, ssize_t length, size_t starting_index, kv_list_t *kv_list)
{

    char key[MAX_MSG_SIZE];
    int key_index = 0;
    char value[MAX_MSG_SIZE];
    int value_index = 0;

    size_t list_index = starting_index;

    int parsing_key = 1;

    char iterator;

    for (ssize_t i = 0; i < length; i++) {
        iterator = in_msg[i];

        if (parsing_key && iterator != '\0') {
            key[key_index] = iterator;
            key_index++;
        } else if (parsing_key && iterator == '\0') {
            parsing_key = 0;
            key[key_index] = '\0';
            key_index = 0;
        } else if (!parsing_key && iterator != '\0') {
            value[value_index] = iterator;
            value_index++;
        } else if (!parsing_key && iterator == '\0') {

            value[value_index] = '\0';

            kv_list->list[list_index] = create_kv_pair(key, value);
            list_index++;

            if (list_index >= kv_list->size) {
                return (size_t) -1;
            }

            parsing_key = 1;
            value_index = 0;
        }
    }
    value[value_index] = '\0';
    kv_list->list[list_index] = create_kv_pair(key, value);

    return list_index + 1;
}

// standard includes
#include <stdio.h>

// for basic socket communication
#include <sys/socket.h>

#include "system.h"
#include "network.h"
#include "config.h"
#include "error.h"
#include "client.h"
#include "stdlib.h"
#include "network-utils.h"
#include "util.h"
#include <arpa/inet.h>

/**
 * @brief Utilitary executable which pings every server in servers.txt and returns their status
 * As follows : pps-list-nodes
 * @return 0 on normal exit and -1 on abnormal exit
 */
int main(int argc, char* argv[])
{
    size_t BUFFER_LENGTH = 20;

    /* Client initialization and parsing optionnal arguments */
    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.argv = &argv;
    client_i.required = 0;
    client_i.optionnal = 0;
    client_i.size_args = argc;

    error_code error_init = client_init(client_i);

    if(error_init != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    /* Pings every server */
    for(size_t i = 0; i < client.args->N; i++) {
        char* buffer_send = malloc(0);
        send_packet(client.socket, buffer_send, 0, client.server.nodes[i]);

        int error_receive = recv(client.socket, NULL,0,0);
        char* status;
        if(error_receive == -1) {
            status = "FAIL";

        } else {

            status = "OK";

        }
        char buffer[BUFFER_LENGTH];
        printf("%s %hu %s\n", inet_ntop(AF_INET, &client.server.nodes[i].sin_addr, buffer, BUFFER_LENGTH), ntohs(client.server.nodes[i].sin_port), status);
    }



    return 0;
}

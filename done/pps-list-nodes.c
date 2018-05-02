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
#include "util.h"
#include <arpa/inet.h>

int main(void)
{

    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.name = "test";
    client_init(client_i);


    for(int i=0; i<client.server.size; i++) {
        char* buffer_send = malloc(0);
        send_packet(client.socket, buffer_send, 0, client.server.nodes[i]);

        int error_receive = recv(client.socket, NULL,0,0);
        char* status;
        if(error_receive==-1) {
            status = "FAIL";

        } else {

            status = "OK";

        }
        char buffer[20];
        printf("%s %hu %s\n", inet_ntop(AF_INET, &client.server.nodes[i].sin_addr, buffer, 20), ntohs(client.server.nodes[i].sin_port), status);


    }




}

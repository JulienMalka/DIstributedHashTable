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
#include "node.h"
#include <arpa/inet.h>

void print_sha(unsigned char* input)
{
    if (input != NULL) {
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            printf("%02x", input[i]);
        }
    }
}

void create_key_from_sockaddr(struct sockaddr_in address, char* dest){
	memset(dest, '\0', 1);
	char space[2];
	memset(&space, ' ', 1);
	space[1] = '\0';
	strcat(dest, inet_ntoa(address.sin_addr));
	strcat(dest, space);


	char port_str[5];
	sprintf(port_str, "%d", ntohs(address.sin_port));
	strcat(dest, port_str);
}

/**
 * @brief Utilitary executable which pings every server in servers.txt and returns their status
 * As follows : pps-list-nodes
 * @return 0 on normal exit and -1 on abnormal exit
 */
int main(int argc, char* argv[])
{
    socklen_t BUFFER_LENGTH = 20;

    /* Client initialization and parsing optional arguments */
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

    /* Sort the servers by port */
    node_list_sort(&client.server, node_cmp_server_addr);

    /* Pings every server */
    for(size_t i = 0; i < client.server.size; i++) {
        send_packet(client.socket, "", 0, client.server.nodes[i]);
    }

    Htable_t htable_nodes = construct_Htable(10);

    struct sockaddr_in source_adress;
    socklen_t address_size = sizeof(struct sockaddr_in);

    size_t nbr_response = 0;
    while (recvfrom(client.socket, NULL, 0, 0, (struct sockaddr *) &source_adress, &address_size) != -1) {

	//		printf("received confirmation from %s and %d of length %hu \n", inet_ntoa(source_adress.sin_addr), ntohs(source_adress.sin_port), address_size);

			char key[30];
			create_key_from_sockaddr(source_adress, key);

	//		printf("\nmade key successfully = %s \n", key);

			add_Htable_value(htable_nodes, key, "OK");
			nbr_response++;

			/*Received enough responses*/
			if (nbr_response >= client.server.size)
				break;
    }

    for (size_t i = 0; i < client.server.size; i++) {

    	char key[30];
    	create_key_from_sockaddr(client.server.nodes[i].addr, key);
	//	printf("\nmade key successfully = %s \n", key);

        pps_value_t status = get_Htable_value(htable_nodes, key);

        /*Key not found in the hashtable*/
        if (status == NULL)
            status = "FAIL";

        char buffer[BUFFER_LENGTH];
        printf("%s %hu (", inet_ntop(AF_INET, &client.server.nodes[i].addr.sin_addr, buffer, BUFFER_LENGTH), ntohs(client.server.nodes[i].addr.sin_port));
        print_sha(client.server.nodes[i].sha);
        printf(") %s\n", status);

    }

    delete_Htable_and_content(&htable_nodes);

    return 0;
}

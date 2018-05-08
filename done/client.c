#include <stdio.h>
#include "client.h"
#include "config.h"
#include "system.h"
#include "node.h"
#include "network.h"
/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */

void client_end(client_t *client)
{
    node_list_free(&(client->server));
}

error_code client_init(client_init_args_t client_init_args)
{
    client_init_args.client->name = *client_init_args.argv[0];
    //printf("Client name : %s\n", client_init_args.client->name);
    ++*client_init_args.argv;
    //printf("first arg : %s\n", *client_init_args.argv[0]);
    client_init_args.size_args--;
    if(client_init_args.size_args<client_init_args.required){
      return ERR_BAD_PARAMETER;
    }
    int adress_start = *client_init_args.argv;
    args_t* args_opt = parse_opt_args(client_init_args.optionnal, client_init_args.argv);
    //printf("went there\n");
    int nb_parsed;
    if(args_opt==NULL){
    nb_parsed =0;
    }else{
    nb_parsed = ((int)*client_init_args.argv - adress_start)/sizeof(char**);
    client_init_args.client->args = args_opt;
  }
    printf("NB OPT PARSED = %d\n", nb_parsed);
    if(client_init_args.size_args-nb_parsed!=client_init_args.required){
      return ERR_BAD_PARAMETER;
    }
    node_list_t* nodes = node_list_new();
    nodes = get_nodes();
    client_init_args.client->server = *nodes;
    client_init_args.client->socket = get_socket(1);
    //printf("We've done that !\n");
    return 0;
}

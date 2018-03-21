#include "error.h"
#include "hashtable.h"



error_code network_get(client_t client, pps_key_t key, pps_value_t *value){
	// Set up socket with a receive timeout of 1 s.
    int s;
    s = get_socket(1);

    // Load server address.
    struct sockaddr_in srv_addr;
    get_server_addr(PPS_DEFAULT_IP, PPS_DEFAULT_PORT, &srv_addr);

    // Read user input from stdin.
    unsigned int request = 0; // assume we want to send a single integer value
    printf("What int value do you want to send? ");
    scanf("%d", &request);

    // Prepare outgoing message with htonl.
    unsigned int out_msg;
    out_msg = htonl(request);

    // Send message.
    printf("Sending message to %s:%d: %d\n",
           PPS_DEFAULT_IP, PPS_DEFAULT_PORT, request);
    if (sendto(s, &out_msg, sizeof(out_msg), 0,
               (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1)
        die("sendto");

    // Receive response.
    unsigned int in_msg = 0;
    ssize_t in_msg_len = recv(s, &in_msg, sizeof(in_msg), 0);
    if (in_msg_len == sizeof(in_msg)) { // Valid response.
        // Parse response with ntohl
        int response;
        response = ntohl(in_msg);
        printf("Received response: %d\n", response);
    } else if (in_msg_len == -1) { // Timeout.
        puts("Receive timeout.");
    } else { // Wrong message size.
        puts("Received invalid response.");
    }

    // Clean up socket.
    close(s);

    return 0;
}

error_code network_put(client_t client, pps_key_t key, pps_value_t value){
	
	
}

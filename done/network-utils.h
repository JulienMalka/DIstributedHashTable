/**
 * @brief send a packet to a node
 * @param message the packet to send
 * @param socket socket to use
 * @param size size of the packet
 * @param node the node we want to send the packet to
 * @return an error code
 */
error_code send_packet(int socket, const char* message, size_t size, node_t node);


/**
 * @brief format a put request with the caracter \0 correctly placed
 * @param value value to format
 * @param key key to format
 * @return the request correctly formated
 */
char* format_put_request(pps_key_t key, pps_value_t value, size_t skey, size_t svalue);


/**
 * @brief parse a put request
 * @param in_msg the packet to parse
 * @param length length of request
 * @param key pointer to the key
 * @param value pointer to the value
 * @return an error code
 */
error_code parse_put_request(char* in_msg, size_t length, char* key, char* value);

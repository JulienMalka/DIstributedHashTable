#include <stdio.h>
#include <stdlib.h>

//for is digit
#include "ctype.h"

#include "client.h"
#include "network.h"
#include "config.h"
#include "errno.h"

/**
 * @brief checks if string corresponds to a valid number
 * @param str string
 * @return 1 if so, 0 otherwise
 */
int isValidNumber(const char *str) {
	if (str == NULL || *str == '\0' || isspace(*str))
		return 0;
	char *p;
	strtod(str, &p);
	return *p == '\0';
}

error_code string_to_int(const char *string, int *dest) {

	if (!isValidNumber(string))
		return ERR_BAD_PARAMETER;

	errno = 0;
	char *string_end = NULL;

	*dest = (int) strtol(string, &string_end, 10);

	if (errno != 0 || (string && *string_end != 0))
		return ERR_BAD_PARAMETER;

	return ERR_NONE;
}

/**
 * @brief Compute a substring of given position and length of a value associated to a given key and stores it with a given key
 * As follows : pps-client-substr [-n N] [-w W] [-r R] [--] <input-key> <position> <length> <output-key>
 * @return 0 on normal exit, -1 otherwise
 */
int main(int argc, char *argv[]) {

	printf("start exec\n");

	/* Set up client */
	client_t client;
	client_init_args_t client_i;
	client_i.client = &client;
	client_i.argv = &argv;
	client_i.required = 4;
	client_i.optionnal = (TOTAL_SERVERS | PUT_NEEDED | GET_NEEDED);
	client_i.size_args = (size_t) argc;

	error_code error_init = client_init(client_i);

	if (error_init != ERR_NONE) {
		printf("FAIL\n");
		return -1;
	}

	/* Parse mandatory arguments */
	char *key_from = NULL;
	char *key_to = NULL;
	int position;
	int length;

	/* Require 4 mandatory arguments */
	size_t nbr_argv = argv_size(argv);
	printf("args = %lu\n", nbr_argv);
	if (nbr_argv != 4) {
		client_end(&client);
		printf("FAIL\n");
		return -1;
	}

	if (argv[0] != NULL && argv[1] != NULL && argv[2] != NULL && argv[3] != NULL) {

		/* Checks if second and third argument are valid numbers */
		if ((string_to_int(argv[1], &position) == ERR_NONE) && (string_to_int(argv[2], &length) == ERR_NONE)) {
			key_from = argv[0];
			key_to = argv[3];
		} else {
			client_end(&client);
			printf("FAIL\n");
			return -1;
		}
	} else {
		client_end(&client);
		printf("FAIL\n");
		return -1;
	}

	pps_value_t value_from;

	error_code error_get = network_get(client, key_from, &value_from);

	if (error_get != ERR_NONE) {
		client_end(&client);
		printf("FAIL/n");
		return -1;
	}

	/*COMPUTE THE SUBSTRING*/
	if (position < 0) {
		position = (int) (strlen(value_from) + position);
	}

	/*ERROR HANDLING*/
	if (length + position > (int) strlen(value_from)) {
		client_end(&client);
		printf("FAIL\n");
		return -1;
	}

	/* array */
	char substring[strlen(value_from)];

	if (length == 0)
		memset(substring, '\0', strlen(substring));
	else {
		for (int i = position; i < position + length; i++) {
			substring[i - position] = value_from[i];
		}
		substring[length] = '\0';
	}

	error_code error_put = network_put(client, key_to, substring);

	if (error_put != ERR_NONE) {
		client_end(&client);
		printf("FAIL\n");
		return -1;
	}

	printf("OK\n");

	free_const_ptr(value_from);
	client_end(&client);

	return 0;
}

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include "string.h"
#include "client.h"
#include <stdlib.h>
#include <bits/time.h>
#include <time.h>
#include "network.h"

static void print_time(const struct timespec *p_time_start, const struct timespec *p_time_end) {
	long nsec = p_time_end->tv_nsec - p_time_start->tv_nsec;
	double sec = nsec / (double) 100000000;
	printf("%f seconds\n", sec);
}


int main(int argc, char *argv[]) {

	/* Client initialization and parsing optionnal arguments */
	client_t client;
	client_init_args_t client_i;
	client_i.client = &client;
	client_i.argv = &argv;
	client_i.required = 1;
	client_i.optionnal = (TOTAL_SERVERS | GET_NEEDED | PUT_NEEDED);
	client_i.size_args = argc;


	error_code error_init = client_init(client_i);

	if (error_init != ERR_NONE) {
		printf("FAIL\n");
		return -1;
	}

	/* Parse mandatory argument 1 and 2 */
	long nbr_keys = strtol(argv[0], NULL, 10);
	long buffer_length = 100;
//	printf("nbr key = %ld\n", buffer_length);

	/*Set up time monitoring */
	struct timespec time_start, time_end;
	int time_start_ret = clock_gettime(CLOCK_MONOTONIC, &time_start);
	M_EXIT_IF_ERR(time_start_ret, somename);

	/* Putting every key*/
	for (long i = 0; i < nbr_keys; i++) {
		char key_buffer[buffer_length];

		sprintf(key_buffer, "%ld", i);

//		printf("trying to put %s\n", key_buffer);
		error_code error_put = network_put(*client_i.client, key_buffer, key_buffer);
//		printf("success\n");


		if (error_put != ERR_NONE) {
			printf("Failed to put one of the keys : %s\n", key_buffer);
		//	return 1;
		}

	}

	for (long i = 0; i < nbr_keys; i++) {
		char key_buffer[buffer_length];
		pps_value_t value;

		sprintf(key_buffer, "%ld", i);

		error_code error_get = network_get(*client_i.client, key_buffer, &value);

	/*	if (error_get != ERR_NONE) {
			printf("Failed to get one of the keys : %s, got %s\n", key_buffer, value);
			return 1;
		}*/
	}

	int time_end_ret = clock_gettime(CLOCK_MONOTONIC, &time_end);
	M_EXIT_IF_ERR(time_end_ret, somename);

	print_time(&time_start, &time_end);

	client_end(client_i.client);

	return 0;
}


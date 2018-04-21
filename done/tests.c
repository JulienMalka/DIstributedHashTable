/**
 * @file test-hashtable.c
 * @brief test code for hashtables
 *
 * @author Valérian Rousset & Jean-Cédric Chappelier
 * @date 02 Oct 2017
 */

#include <stdio.h> // for puts(). to be removed when no longer needed.

#include <check.h>

#include "tests.h"

#include "hashtable.c"

#include "node_list.h"

#include <arpa/inet.h>

void print_htable(Htable_t* table);

START_TEST(week04)
{
	printf("\nSTARTING TESTS FOR HTABLE\n\n");
    Htable_t table = construct_Htable(HTABLE_SIZE);

    // Defining tests ressource
    const pps_key_t key1 = "antoine";
    const pps_value_t value1 = "crettenand";
    const pps_key_t key2 = "julien";
    const pps_value_t value2 = "malka";

    add_Htable_value(table, key1, value1);
	printf("1. no segfault until now\n");
    add_Htable_value(table, key2, value2);
    
    print_htable(&table);
    
    const pps_value_t value_read1 = get_Htable_value(table, key1);
    printf("2. no segfault until now\n");
    const pps_value_t value_read2 = get_Htable_value(table, key2);
    printf("3. no segfault until now\n");
    
    printf("value_read1 = %s, value_read2 = %s\n", value_read1, value_read2);

    // tests for input value = read value from get method
    ck_assert_str_eq(value1, value_read1);
    ck_assert_str_eq(value2, value_read2);
	/*
    const size_t hashed = hash_function(key1, 0);
    ck_assert_int_eq(0, hashed);

    const size_t maxSize = SIZE_MAX;
    ck_assert_int_eq(0, hash_function(key1, maxSize));*/
    
    delete_Htable_and_content(&table);
}
END_TEST

void print_htable(Htable_t* htable){
	printf("PRINTING CONTENT OF HTABLE\n");
	for (int i = 0; i < htable->size; i++){
		printf("\nindex %d ", i);
		print_bucket(&htable->buckets[i]);		
	}
}

void print_nodes(node_t* nodes, size_t size)
{
    char buffer[20];

    for (int i = 0; i < size; i++) {
        printf("node %d address = %s port = %hu\n", i, inet_ntop(AF_INET, &nodes[i].sin_addr, buffer, 20), ntohs(nodes[i].sin_port));
    }

}

START_TEST(week05)
{
	printf("\nSTARTING TESTS FOR GET_NODES\n\n");
    node_list_t* nodes = node_list_new();

    nodes = get_nodes();

    print_nodes(nodes->nodes, nodes->size);
}
END_TEST



Suite *hashtable_suite()
{

    Suite *s = suite_create("hashtable.h");

    TCase *tc_ht = tcase_create("hashtable");
    suite_add_tcase(s, tc_ht);

    tcase_add_test(tc_ht, week04);
    tcase_add_test(tc_ht, week05);

    return s;
}

TEST_SUITE(hashtable_suite)

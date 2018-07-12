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

#include "config.h"

#include <arpa/inet.h>

#include "args.h"

void print_htable(Htable_t* table);

START_TEST(add_get_hashtable)
{
//	printf("\nSTARTING TESTS FOR HTABLE\n\n");
    Htable_t table = construct_Htable(HTABLE_SIZE);

    // Defining tests ressource
    const pps_key_t key1 = "antoine";
    const pps_value_t value1 = "crettenand";
    const pps_key_t key2 = "julien";
    const pps_value_t value2 = "malka";
    const pps_key_t key3 = "forgot_in_table";

    const pps_value_t value3 = "theboss";

    add_Htable_value(table, key1, value1);
    add_Htable_value(table, key2, value2);

    const pps_value_t value_read1 = get_Htable_value(table, key1);

    ck_assert_str_eq(value1, value_read1);

    const pps_value_t value_read2 = get_Htable_value(table, key2);

    add_Htable_value(table, key1, value3);

    const pps_value_t value_read3 = get_Htable_value(table, key1);
    const pps_value_t value_read4 = get_Htable_value(table, key3);

    // tests for input value = read value from get method
    ck_assert_str_eq(value2, value_read2);
    ck_assert_str_eq(value3, value_read3);
    ck_assert_ptr_null(value_read4);
    ck_assert_ptr_null(get_Htable_value(table, NULL));

    delete_Htable_and_content(&table);
}
END_TEST

/*UTILITARY FUNCTION TO PRINT THE CONTENT OF A BUCKET*/
void print_bucket(struct bucket* bck)
{
    if (bck == NULL) {
        printf("(null), ");
    } else {
        printf("kv(%s, %s), ", bck->key_value.key, bck->key_value.value);
        print_bucket(bck->next);
    }
}

/*UTILITARY FUNCITON TO PRINT THE CONTENT OF AN HTABLE*/
void print_htable(Htable_t* htable)
{
    printf("PRINTING CONTENT OF HTABLE\n");
    for (int i = 0; i < htable->size; i++) {
        printf("\nindex %d ", i);
        print_bucket(&htable->buckets[i]);
    }
}


/*UTILITARY FUNCTION TO PRINT A LIST OF NODES*/
void print_nodes(node_t* nodes, size_t size)
{
    char buffer[20];

    for (int i = 0; i < size; i++) {
        printf("node %d address = %s port = %hu\n", i, inet_ntop(AF_INET, &nodes[i].sin_addr, buffer, 20), ntohs(nodes[i].sin_port));
    }

}

START_TEST(get_nodes_test)
{
    printf("\nSTARTING TESTS FOR GET_NODES\n\n");
    node_list_t* nodes = get_nodes();

    print_nodes(nodes->nodes, nodes->size);
}
END_TEST

START_TEST(get_hashtable_size)
{
//	printf("\nSTARTING TESTS FOR GET_HASHTABLE_CONTENT");

    Htable_t table = construct_Htable(5);

    // Defining tests ressource
    const pps_key_t key1 = "antoine";
    const pps_value_t value1 = "crettenand";
    const pps_key_t key2 = "julien";
    const pps_value_t value2 = "malka";

    add_Htable_value(table, key1, value1);
    add_Htable_value(table, key2, value2);

    /*Get the number of pairs in the htable*/
    size_t table_size = get_Htable_size(table);

    ck_assert_uint_eq(table_size, 2);

    add_Htable_value(table, "1", "a");
    add_Htable_value(table, "2", "b");
    add_Htable_value(table, "3", "c");

    table_size = get_Htable_size(table);

    ck_assert_uint_eq(table_size, 5);

}
END_TEST

/*UTILITARY FUNCTION TO PRINT THE CONTENT OF A KV_LIST*/
void print_kv_list(kv_list_t* list)
{
    printf("\n");
    for (int i = 0; i < list->size; i++) {
        printf("index %d => (%s, %s)\n", i, list->list[i].key, list->list[i].value);
    }
}

START_TEST(get_hashtable_content)
{
    printf("\nSTARTING TESTS FOR GET_HASHTABLE_CONTENT\n");

    Htable_t table = construct_Htable(5);

    // Defining tests ressource
    add_Htable_value(table, "1", "a");
    add_Htable_value(table, "2", "b");
    add_Htable_value(table, "3", "c");
    add_Htable_value(table, "4", "d");
    add_Htable_value(table, "5", "e");
    add_Htable_value(table, "6", "f");

    kv_list_t* list_of_kv = get_Htable_content(table);

    print_kv_list(list_of_kv);

    kv_list_free(list_of_kv);

}
END_TEST

void print_kv_pair_list(kv_pair_t* kv_pair_list, size_t length)
{

    for (int i = 0; i < length; i++) {
        printf("%s %s", kv_pair_list[i].key, kv_pair_list[i].value);
    }

}

kv_pair_t* parse_and_print_response(char* in_msg, size_t length)
{

    printf("STARTING TEST FOR DUMP NODE\n");

    size_t expected_nbr_kv_pair = (in_msg[3]) & (in_msg[2] << 8) & (in_msg[1] << 16) & (in_msg[0] << 24);
    printf("%lu\n", expected_nbr_kv_pair);

    kv_pair_t* kv_pair_list = calloc(expected_nbr_kv_pair, sizeof(kv_pair_t));

    char key[MAX_MSG_SIZE];
    int key_index = 0;
    char value[MAX_MSG_SIZE];
    int value_index = 0;

    size_t list_index = 0;

    int parsing_key = 1;

    char iterator;

    for (int i = 4; i < length; i++) {
        iterator = in_msg[i];

        if (parsing_key && iterator != '\0') {
            key[key_index] = iterator;
            key_index++;
        } else if (parsing_key && iterator == '\0') {
            parsing_key = 0;
            key_index = 0;
        } else if (!parsing_key && iterator != '\0') {
            value[value_index] = iterator;
            value_index++;
        } else if (!parsing_key && iterator == '\0') {
            printf("%s %s\n", key, value);

            kv_pair_list[list_index] = create_kv_pair(key, value);
            list_index++;

            parsing_key = 1;
            value_index = 0;
        }

    }
    kv_pair_list[list_index] = create_kv_pair(key, value);
    list_index++;

    if (list_index != expected_nbr_kv_pair)
        return NULL;

    printf("%s %s\n", key, value);

    return kv_pair_list;
}

START_TEST(parsing_argv)
{

    printf("STARTING TESTS FOR PARSING LINE COMMANDE ARGUMENTS\n");

    char** array_of_str = calloc(7, sizeof(char));
    array_of_str[0] = "-n";
    array_of_str[1] = "2";
    array_of_str[2] = "-w";
    array_of_str[3] = "3";
    array_of_str[4] = "--";
    array_of_str[5] = "antoine";
    array_of_str[6] = "crettenand";

    /*	for (int i = 0; i < 7; i++){
    		printf("%s ", array_of_str[i]);
    	}
    	printf("\n");

    	printf("LAUNCHING METHOD\n");
    */

    args_t* args = parse_opt_args(TOTAL_SERVERS | PUT_NEEDED, &array_of_str);

    printf("mandatory arguments = %s\n", *array_of_str);

    printf("results are N = %lu and W = %lu\n", args->N, args->W);
}
END_TEST

Suite *hashtable_suite()
{

    Suite *s = suite_create("hashtable.h");

    TCase *tc_ht = tcase_create("hashtable");
    suite_add_tcase(s, tc_ht);

    tcase_add_test(tc_ht, get_nodes_test);
    tcase_add_test(tc_ht, add_get_hashtable);
    tcase_add_test(tc_ht, get_hashtable_size);
    tcase_add_test(tc_ht, get_hashtable_content);
    tcase_add_test(tc_ht, parsing_argv);

    return s;
}

TEST_SUITE(hashtable_suite)

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
#include "hashtable.h"

START_TEST(add_value_does_retrieve_same_value)
{
    Htable_t table;
	
	// Defining tests ressource
	const pps_key_t key1 = 'c';
	const pps_value_t value1 = 42;
	const pps_key_t key2 = 'a';
	const pps_value_t value2 = 0;
	
	add_Htable_value(table, key1, value1);
	add_Htable_value(table, key2, value2);
	
	const pps_value_t value_read1 = get_Htable_value(table, key1);
	const pps_value_t value_read2 = get_Htable_value(table, key2);

	// tests for input value = read value from get method
	ck_assert_int_eq(value1, value_read1);
	ck_assert_int_eq(value2, value_read2);
	
	// tests if reference isn't null
	ck_assert_ptr_nonnull(table);
	
	// tests if method outputs correct errors on inputs
	ck_assert_bad_param(add_Htable_value(NULL, key1, value1));
	ck_assert_err_none(add_Htable_value(table, key1, value1));
	
}
END_TEST

Suite *hashtable_suite()
{

    Suite *s = suite_create("hashtable.h");

    TCase *tc_ht = tcase_create("hashtable");
    suite_add_tcase(s, tc_ht);

    tcase_add_test(tc_ht, add_value_does_retrieve_same_value);

    return s;
}

TEST_SUITE(hashtable_suite)

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "error.h"

/*
 * Private implementation of the bucket structure
 * It is a linked list of kv_pair_t
 */
struct bucket {
    kv_pair_t key_value;
    struct bucket* next;
};

/*
 * @brief Create a new Htable of parameter size. An htable is an array of buckets.
 * @param number of buckets in the htable
 * @return new htable
 */
Htable_t construct_Htable(size_t size)
{

    Htable_t htable_new;
    htable_new.buckets = calloc(size, sizeof(struct bucket));
    htable_new.size = size;

    return htable_new;
}
/*
 * @brief Create a new bucket.
 * @param key_value of the bucket
 * @param reference to the next bucket
 * @return reference to the new bucket
 */
struct bucket* create_bucket(kv_pair_t key_value, struct bucket* next)
{
    struct bucket* new = malloc(sizeof(struct bucket));
    new->key_value = key_value;

    new->next = malloc(sizeof(struct bucket));
    new->next = next;
    return new;
}

/*
 * @brief free the reference key and value from a kv_pair
 * @param kv to free
 */
void kv_pair_free(kv_pair_t *kv)
{
    free((char*) kv->key);
    free((char*) kv->value);
}

/*
 * @brief Create a deep copy kv_pair from given key and value
 * @param key
 * @param value
 * @return new kv_pair with copies of key and value
 */
kv_pair_t create_kv_pair(pps_key_t key, pps_value_t value)
{

    char* key_new = calloc(strlen(key), sizeof(char));
    char* value_new = calloc(strlen(value), sizeof(char));

    for (int i = 0; i < strlen(key); i++) {
        key_new[i] = key[i];
    }

    for (int i = 0; i < strlen(value); i++) {
        value_new[i] = value[i];
    }

    kv_pair_t pair_new;
    pair_new.key = key_new;
    pair_new.value = value_new;

    return pair_new;
}

/*
 * @brief free the linked-list recursively bottom-up approach
 * @param bck bucket to delete
 */
void delete_bucket(struct bucket* bck)
{
    if (bck == NULL) {}
    else if (bck->key_value.key == NULL || bck->key_value.value == NULL) {}
    else if (bck->next == NULL) {
        free(bck->next);
        kv_pair_free(&bck->key_value);
    } else {
        delete_bucket(bck->next);
        kv_pair_free(&bck->key_value);
    }
}

/**
 * @brief delete the given hash-table
 *    Note: does NOTHING until week 07.
 * @param table the hash-table to free (passed by reference)
 */
void delete_Htable_and_content(Htable_t* table)
{
    for(int i = 0; i < table->size; i++) {
        delete_bucket(&table->buckets[i]);
    }

    free(table->buckets);
    table->buckets = NULL;
    table->size = 0;

}

/*
 * @brief adds a key value pair to a existent bucket
 * @param bck bucket to add the pair to
 * @param key
 * @param value
 * @return 0
 */
error_code add_value_to_bucket(struct bucket* bck, pps_key_t key, pps_value_t value)
{

    kv_pair_t key_value = bck->key_value;

    if (key_value.key == NULL && key_value.value == NULL) {

        bck->key_value = create_kv_pair(key, value);

    } else if (strcmp(key, key_value.key) == 0) {

        kv_pair_free(&bck->key_value);

        bck->key_value = create_kv_pair(key, value);

    } else if (bck->next == NULL) {
        kv_pair_t pair_new = create_kv_pair(key, value);

        struct bucket* bucket_new = create_bucket(pair_new, NULL);
        bck->next = bucket_new;

    } else add_value_to_bucket(bck->next, key, value);

    return 0;
}

/*
 * @brief adds a key value pair to a existent htable
 * @param htable to add the pair to
 * @param key
 * @param value
 * @return ERR_BAD_PARAMETER if value reference is NULL, 0 otherwise
 */
error_code add_Htable_value(Htable_t htable, pps_key_t key, pps_value_t value)
{
    if (value == NULL)
        return ERR_BAD_PARAMETER;

    size_t index = hash_function(key, htable.size);
    return add_value_to_bucket(&htable.buckets[index], key, value);
}

/*
 * @brief Find the value associated to given key in the bucket
 * @param bck to look in
 * @param key
 * @return value associated or NULL if the key does not exist in the htable
 */
pps_value_t get_value_from_bucket(struct bucket* bck, pps_key_t key)
{
    kv_pair_t key_value = bck->key_value;

    if (key_value.key == NULL || key_value.value == NULL)
        return NULL;
    else if (strcmp(key_value.key, key) == 0) {
        return key_value.value;
    } else if (bck->next == NULL) {
        return NULL;
    } else return get_value_from_bucket(bck->next, key);
}

/*
 * @brief Find the value associated to given key in the htable
 * @param htable to look into
 * @param key
 * @return value associated or NULL if the key does not exist in the htable
 */
pps_value_t get_Htable_value(Htable_t htable, pps_key_t key)
{
    if (key == NULL)
        return NULL;

    size_t index = hash_function(key, htable.size);
    struct bucket* row = &htable.buckets[index];
    return get_value_from_bucket(row, key);
}

/*
 * @brief The hash function used to compute the indexes in the htable
 * @param key to hash
 * @param size of the htable
 * @return index
 */
size_t hash_function(pps_key_t key, size_t size)
{
    M_REQUIRE(size != 0, SIZE_MAX, "size == %d", 0);
    M_REQUIRE_NON_NULL_CUSTOM_ERR(key, SIZE_MAX);

    size_t hash = 0;
    const size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; ++i) {
        hash += (unsigned char) key[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }
    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % size;
}


void kv_list_free(kv_list_t *list)
{
    for (int i = 0; i < list->size; i++) {
        kv_pair_free(&list->list[i]);
    }
    list->size = 0;
    list->list = NULL;
    free(list);
}

size_t get_bucket_size(struct bucket* bck)
{

    size_t total = 0;

    if (bck->key_value.key == NULL || bck->key_value.value == NULL) {
        return total;
    } else if (bck->next == NULL) {
        return 1;
    } else return 1 + get_bucket_size(bck->next);
}

size_t get_Htable_size(Htable_t table)
{

    size_t total = 0;

    for (int i = 0; i < table.size; i++) {
        total += get_bucket_size(&table.buckets[i]);
    }

    return total;
}

size_t get_bucket_content(struct bucket* bck, kv_list_t* list, size_t from)
{

    if (bck->key_value.key == NULL || bck->key_value.value == NULL) {
        return 0;
    } else if (bck->next == NULL) {
//		printf("index %lu => (%s, %s)\n", from, bck->key_value.key, bck->key_value.value);
        list->list[from] = create_kv_pair(bck->key_value.key, bck->key_value.value);
        return 1;
    } else {
        list->list[from] = bck->key_value;
        return 1 + get_bucket_content(bck->next, list, from + 1);
    }
}

kv_list_t *get_Htable_content(Htable_t table)
{

    size_t htable_size = get_Htable_size(table);

//	printf("size of htable = %lu\n", htable_size);

    kv_list_t* list_of_kv = malloc (sizeof(kv_list_t));
    list_of_kv->list = calloc(htable_size, sizeof(kv_list_t));
    list_of_kv->size = htable_size;
    size_t used_until = 0;

    for (int i = 0; i < table.size; i++) {
        used_until += get_bucket_content(&table.buckets[i], list_of_kv, used_until);
    }
    return list_of_kv;
}


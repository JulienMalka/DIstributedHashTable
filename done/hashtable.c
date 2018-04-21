#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "error.h"

struct bucket {
	kv_pair_t key_value;
	struct bucket* next;
 };

Htable_t construct_Htable(size_t size){
	
	Htable_t htable_new;
	htable_new.buckets = calloc(size, sizeof(struct bucket));
	htable_new.size = size;
	
	return htable_new;
}

struct bucket* create_bucket(kv_pair_t key_value, struct bucket* next){
	struct bucket* new = malloc(sizeof(struct bucket));
	new->key_value = key_value;

	new->next = malloc(sizeof(struct bucket));
	new->next = next;	
	return new;
}

void kv_pair_free(kv_pair_t *kv){
	free(&kv->key);
	free(&kv->value);
}

kv_pair_t copy_kv_pair(pps_key_t key, pps_value_t value){
	
	char* key_new = calloc(strlen(key), sizeof(char));
	char* value_new = calloc(strlen(value), sizeof(char));
	
	for (int i = 0; i < strlen(key); i++){
		key_new[i] = key[i];
	}
	
	for (int i = 0; i < strlen(value); i++){
		value_new[i] = value[i];
	}
	
	kv_pair_t pair_new;
	pair_new.key = key_new;
	pair_new.value = value_new;
	
	return pair_new;
}

void delete_bucket(struct bucket* bck){
	if (bck == NULL){
		}
	else if (bck->next == NULL){// && bck->key_value.key != NULL && bck->key_value.value != NULL){
		kv_pair_free(&bck->key_value);	
	} else {
		delete_bucket(bck->next);
		kv_pair_free(&bck->key_value);
		bck = NULL;
	}
}

void delete_Htable_and_content(Htable_t* table){
/*	for(int i = 0; i < table->size; i++){
		delete_bucket(&table->buckets[i]);
	}
*/ 
	free(table->buckets);
	table->size = 0;
	
}

error_code add_value_to_bucket(struct bucket* bck, pps_key_t key, pps_value_t value){
	
	kv_pair_t key_value = bck->key_value;
	
	if (key_value.key == NULL && key_value.value == NULL){
		
		bck->key_value = copy_kv_pair(key, value);
		
	} else if (key == key_value.key){
		
		bck->key_value = copy_kv_pair(key, value);
		
	} else if (bck->next == NULL){
		kv_pair_t pair_new = copy_kv_pair(key, value);

		struct bucket* bucket_new = create_bucket(pair_new, NULL);
//		printf("collision of index\n");
		bck->next = bucket_new;
				
	} else add_value_to_bucket(bck->next, key, value);
//	printf("the seg fault is after\n");
	
	return ERR_NONE;
}

error_code add_Htable_value(Htable_t htable, pps_key_t key, pps_value_t value)
{
    if (value == NULL)
        return ERR_BAD_PARAMETER;
        
    size_t index = hash_function(key, htable.size);
    
//    printf("add_Htable_value => key = %s and value = %s \n", key, value);
    
//    printf("size of htable = %lu and index = %lu\n", htable.size, index);
    
    return add_value_to_bucket(&htable.buckets[index], key, value);
}

pps_value_t get_value_from_bucket(struct bucket* bck, pps_key_t key);

/*TESTS FUNCTION*/
void print_bucket(struct bucket* bck){
	if (bck == NULL){
		printf("(null), ");
	} else {
		printf("kv(%s, %s), ", bck->key_value.key, bck->key_value.value);
		print_bucket(bck->next);
	}	
}

pps_value_t get_Htable_value(Htable_t htable, pps_key_t key)
{
	if (key == NULL)
		return NULL;
		
	size_t index = hash_function(key, htable.size);
//		printf("\nWe got there, looking for %s at index %lu\n", key, index);
	struct bucket* row = &htable.buckets[index];
//	print_bucket(row);
	
	return get_value_from_bucket(row, key);
}

pps_value_t get_value_from_bucket(struct bucket* bck, pps_key_t key){
	kv_pair_t key_value = bck->key_value;
	
//	printf("(key, value) of current = (%s, %s)\n", key_value.key, key_value.value);
	
	if (strcmp(key_value.key, key) == 0){
//		printf("WE FOUND THE KEY\n");
		return key_value.value;
	} else if (bck->next == NULL){
		return NULL;
	} else return get_value_from_bucket(bck->next, key);
}

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

#include <stdio.h>
#include "hashtable.h"
#include "error.h"

error_code add_Htable_value(Htable_t htable, pps_key_t key, pps_value_t value)
{
    if (htable == NULL)
        return ERR_BAD_PARAMETER;

    htable[hash_function(key, HTABLE_SIZE)] = value;
    return ERR_NONE;
}

pps_value_t get_Htable_value(Htable_t htable, pps_key_t key)
{
    return htable[key % HTABLE_SIZE];
}

size_t hash_function(pps_key_t key, size_t size)
{
    if (size > HTABLE_SIZE)
        return 0;
    else if (size == 0) return 0;
    else
        return key % size;

}

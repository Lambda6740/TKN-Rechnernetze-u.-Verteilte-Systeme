#include <stdio.h>
#include "../lib/uthash.h"

typedef struct {
    char *key;
    unsigned int size_of_key;
    char *value;
    unsigned int valueLen;
    UT_hash_handle hh;
} hash_struct;

hash_struct *hash_table = NULL;

hash_struct* get(char *key, uint16_t size_of_key ){
    hash_struct *tmp1;
    hash_struct *tmp2;
    int t;
    if(hash_table){
        t = 0;
        HASH_ITER(hh, hash_table, tmp1, tmp2 ){
            if(size_of_key == (unsigned int)tmp1->size_of_key){
                for(int i = 0; (unsigned int)i < size_of_key; i++){
                    if((char)key[i] != (char)tmp1->key[i])
                    {t++;break;}}
                if( !t ){return tmp1;}}}}
    return NULL;
}

int set(char *key, unsigned int size_of_key, char *value, unsigned int size_of_value){
    hash_struct *temp_struct = get(key, size_of_key );
    if(!temp_struct){
        temp_struct = (hash_struct *)malloc(sizeof(hash_struct));
        if( !temp_struct )
        {perror("memory allocation ERROR\n");EXIT_FAILURE;}
        char * temp_Key = (char *)malloc(size_of_key);
        memcpy((void *)temp_Key, (void *)key, size_of_key);
        temp_struct->key = temp_Key;
        HASH_ADD(hh, hash_table, key, size_of_key, temp_struct );
        temp_struct->size_of_key = size_of_key;
    }
    temp_struct->valueLen = size_of_value;
    char *tempValue = (char *)malloc(size_of_value);
    memcpy((void *)tempValue, (void *)value, size_of_value);
    temp_struct->value = tempValue;
    return 1;
}


int del(char *key, unsigned int size_of_key ){
    int t = 0;
    if(get(key, size_of_key )) {
        hash_struct *to_delete = (hash_struct *) get(key, size_of_key);
        if (hash_table && to_delete) {
            HASH_DEL(hash_table, to_delete);
            free(to_delete->key);
            free(to_delete->value);
            free(to_delete);
            t = 1;
        }
        return t;
    }else{
        return t;
    }
}

/* void main (){
 * }
*/
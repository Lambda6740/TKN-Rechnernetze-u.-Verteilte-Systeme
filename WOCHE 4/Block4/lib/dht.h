#ifndef __DHT_H__
#define __DHT_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>



#define MAX_SERVERS_NO		4
#define MAX_IP_LEN		33
#define MEMCACHED_PUT		0
#define MEMCACHED_GET		1
#define ERR_NO_CONN		-1
#define ERR_NOT_FOUND		-2

struct server {
    int self_ID;
    char* self_IP;
    int self_PORT;
    int pre_ID;
    char * pre_IP;
    int pre_PORT;
    int succ_ID;
    char* succ_IP;
    int succ_PORT;
};

extern struct server servers[MAX_SERVERS_NO];
extern int servers_no, successful_gets;

int set(char *key, char *val);
int get(char *key, char **val, size_t *val_len);
void compute_servers_hashes(int servers_no);

#endif

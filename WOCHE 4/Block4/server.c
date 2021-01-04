#include "lib/dht.h"
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <argp.h>
#include <pcap.h>
#include "lib/uthash.h"



#define DEL (1 << 0)
#define SET (1 << 1)
#define GET (1 << 2)
#define ACK (1 << 3)
#define LOOKUP (1 << 7 | 1 << 0)
#define REPLY (1 << 7 | 1 << 1)

typedef struct sockaddr_in sockaddr_in;

typedef struct {
    char *key;
    unsigned int size_of_key;
    char *value;
    unsigned int size_of_value;
    UT_hash_handle hh;
} hash_struct;


// to test first Byte
void printBits(unsigned char c){
    for (int i = 0; i < 8; ++i, c >>= 1) {
        if (c & 0x1) {
            printf("BIT %d = ",i);
            printf("1\n");

        } else {
            printf("BIT %d = ",i);
            printf("0\n");
        }
    }
}

/* command 1 : GET * command 2 : SET * command 3 : DELETE */
int sig_check(unsigned char buffer){
    int what_bit_i_am_testing = 0;
    int command;
    while (what_bit_i_am_testing < 4) {
        if (buffer && 0x01 && what_bit_i_am_testing == 2) {
            command = 1;
        }
        if (buffer && 0x01 && what_bit_i_am_testing == 1) {
            command = 2;
        }
        if (buffer && 0x01 && what_bit_i_am_testing == 0) {
            command = 3;
        }
        what_bit_i_am_testing++;
        buffer = buffer >> 1;
    }
    return command;
}

//function to set bit
unsigned char setBit(unsigned char data,unsigned int pos)
{
    return (data|(1 << pos));
}

unsigned char make_Ack_sig(unsigned char buffer[]){
    unsigned int i = 3;
    unsigned char c = setBit(buffer[0], i);
    return c;
}
unsigned int hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x % 254;
}
int set_server( struct server in_server) {

    int socket_desc;
    struct sockaddr_in server, client;

    //Create new_client
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create new_client");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(in_server.self_PORT);

    //Bind
    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc, 3);

    //client_sockept and incoming connection
    puts("Waiting for incoming connections...");

    //Receive a message from client
    unsigned char buffer[11];
    bzero(buffer, sizeof(buffer));


    /* ab hier kann man die Befehle an DB senden
    * man kann hier bit-Weise über 'buffer' itereren um Befehle zu
    * lesen und entsprechen reagieren
    */

    /* command 1 : GET
     * command 2 : SET
     * command 3 : DELETE
     */

    //uint16_t size_of_key = buffer[1] << 8 | buffer[2];              // size of KEY
    //uint32_t size_of_value = buffer[3] << 24 | buffer[4] << 16 | buffer[5] << 8 | buffer[6];     // size of VALUE
    int new_client ;
    uint8_t command;
    uint16_t size_of_key;
    uint32_t size_of_value;
    uint16_t hash_Id;
    uint16_t node_Id;
    uint32_t node_Ip;
    uint16_t node_Port;
    char *key;
    char *value = NULL;
    hash_struct *getHash;
    while (1) {
        unsigned int c = sizeof(client);
        new_client = accept(socket_desc, (struct sockaddr*)&client, &c);
        if(new_client < 0 )
        {
            perror("Accept Error");
            close(new_client);
            continue;
        }
        recv(new_client, buffer, 1, 0);
        command = (unsigned char) buffer[0];
        // LOOKUP
        if (command == 130)
            recv(new_client, buffer, 10, 0);
            hash_Id = ntohs(*(uint16_t * )(buffer + 1));
            node_Id = ntohs(*(uint16_t * )(buffer + 3));
            node_Ip= ntohs(*(uint16_t * )(buffer + 7));
            node_Port= ntohs(*(uint16_t * )(buffer + 9));
            if (node_Id == in_ser)
//        size_of_key = ntohs(*(uint16_t * )(buffer + 1));
//        size_of_value = ntohl(*(uint32_t *) (buffer + 3));

        if (size_of_value == 0){
            char buf1[size_of_key];
            buf1[size_of_key] = '\0';
            recv(new_client, buf1, size_of_key, 0);
            key = (char *) (buf1);
        }
        else{
            char buf_2[size_of_key + size_of_value];
            char tmp[size_of_key];
            int recv_counter;
            int tmp_counter=0;
            char buffer_tmp[size_of_value +size_of_key];
            int n=size_of_value +size_of_key;
            int cpy_counter=0;
            while((n>0) && ((recv_counter=recv(new_client, buffer_tmp, n, 0))!=0)){
                if(recv_counter== -1){
                    perror("recv failed!");
                    EXIT_FAILURE;
                }
                memcpy((void *)buf_2 + cpy_counter,(void *)buffer_tmp,recv_counter+1);
                cpy_counter +=recv_counter;
                tmp_counter +=recv_counter;
                n -= recv_counter;
            }

            buf_2[size_of_key + size_of_value] = '\0';
            for (int i = 0; i < size_of_key; ++i) {
                tmp[i] = buf_2[i];
            }
            tmp[size_of_key] = '\0';
            buf_2[size_of_key + size_of_value] = '\0';
            key = (char *) tmp;
            value = (char *) (buf_2 + size_of_key);
        }
        unsigned int forHash[2];
        forHash[0]= (unsigned int) key[0];
        forHash[1]= (unsigned int) key[1];
        unsigned int _hash = forHash[0] + forHash[1];
        int hash_value = hash(_hash) % 251;

    }
}


void main(int argc, char *argv[]){
//    char *ip = argv[2];
//    char *ip_pre = argv[5];
//    char *ip_succ = argv[8];
    struct server server;
    server.self_ID = atoi(argv[1]);
    server.self_IP = argv[2];
    server.self_PORT = atoi(argv[3]);
    server.pre_ID = atoi(argv[4]);
    server.pre_IP = argv[5];
    server.pre_PORT = atoi(argv[6]);
    server.succ_ID = atoi(argv[7]);
    server.succ_IP = argv[8];
    server.succ_PORT = atoi(argv[9]);
    //set_server(server);
    printBits(LOOKUP);
}


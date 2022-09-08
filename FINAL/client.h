#ifndef _HELPER_H_
#define _HELPER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<pthread.h>
#include <arpa/inet.h>
pthread_t *threads;
pthread_mutex_t mutex;
pthread_cond_t cv;
struct sockaddr_in serv_addr;
int fp;
void read_from_file(char* file_name);
void *requests_handle(void* arg);
char** split_requests(char* requests);
int counter1=0;
int counter3=0;
int arrived=0;
typedef struct {
    char requests[100][300];
    int count;
    int id;
}resultrequest;


resultrequest request1[300];


#endif
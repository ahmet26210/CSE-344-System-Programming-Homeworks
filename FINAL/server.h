#ifndef _HELPER_H_
#define _HELPER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<netdb.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<wait.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/time.h>
#include<signal.h>
#include<sys/types.h>
#include <arpa/inet.h>

#define MAX 400
int sockfd , newsockfd;
pthread_t *threads;
void *requests_handle(void* arg);
void terminate_function();
int peek();
void get_current_time();
int isEmpty();
int isFull();
int removeData();
void insert(int data);
int size();
pthread_mutex_t mutex;
pthread_cond_t cv;
int number_of_threads;
typedef struct {
    char requests[100][300];
    int count;
    int id;
}resultrequest;

typedef struct {
    char cities[100][100];
    char ip_num[30];
    int port_num;
    int num_of_dir;
    int num_of_file;
    int num_of_req;
    int process_id;
}servant_data;

servant_data datas[200];
int dir_index=0;

int arrived=0;
void handle_sigint(int sig);
int intArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;
int sig_int_check=0;
char currenttime[200];
int total_request= 0;
#endif
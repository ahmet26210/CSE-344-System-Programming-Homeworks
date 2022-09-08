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
#include <dirent.h>
#include<sys/types.h>
#include<wait.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/time.h>
#include<signal.h>
#include<stdint.h>
#include <arpa/inet.h>

#define SIZE 450
int fp;
void *requests_handle(void* arg);
void read_from_file(char* file_name , int i ,int j);
int index1 = 0;
int index2 = 0;
int check_function(int file_number[3],int start_number[3],int end_number[3]);
typedef struct {
    char transaction_id[50];
    char type[100];
    char name_of_street[50];
    char surface_meters[50];
    char price[50];
}file_members;

typedef struct{
    file_members req[100];
    char file_name[30];
}files;

typedef struct{
    files file[100];
    char dir_name[30];
}Directories;

typedef struct{
    Directories dir[100];
    char ip_num[30];
    int port_num;
    int num_of_dir;
    int num_of_file;
    int num_of_req;
}Dirs_and_port;

struct DataItem {
   Directories data;   
   char key[100];
};

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

struct DataItem *item;
servant_data datas;
pthread_mutex_t mutex;
pthread_cond_t cv;
Dirs_and_port directories ;
void display();
void handle_sigint(int sig);
struct DataItem *search(char* key);
void insert(char* key,Directories data,struct DataItem* item);
int sockfd , newsockfd ;
int thread_number=0;
int sig_int_check=0;
void terminate_function();
int my_get_id();

int total_request= 0;

pthread_t* threads1[500]; 
int createPort();
#endif
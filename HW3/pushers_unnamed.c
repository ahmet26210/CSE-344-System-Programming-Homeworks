#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<wait.h>
#include<semaphore.h>
#include<sys/mman.h>
#include <sys/shm.h>
#include<sys/stat.h>
#include<pthread.h>



void * pusher_sugar();
void * pusher_milk();
void * pusher_flour();
void * pusher_wallnut();
void handle_sigint(int sig);
void handle_sigusr1(int sig);

char* shared_memory;

char* shared_memory1 ="/shared4";

int pusher_id;

char* shared_semephore="/semephore";


sem_t * ptr;

int main(int argc, char* argv[]){

	struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = &handle_sigusr1;
    if(sigaction(SIGUSR1,&sa,NULL) == - 1){
        perror("Sigaction error! \n");
        return 0;
    }

	struct sigaction sa1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;        
    sa1.sa_handler = &handle_sigint;

    if(sigaction(SIGINT,&sa1,NULL) == - 1){
        perror("Sigaction error! \n");
        return 0;
    }
	
    pusher_id = atoi(argv[0]);

	int fd = shm_open(shared_memory1,  O_RDWR , 0666);

    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    shared_memory = (char*) mmap(NULL, sizeof(char) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    close(fd);

    int fd1 = shm_open(shared_semephore,  O_RDWR , 0666);

    if (fd1 == -1) {
        perror("shm_open");
        exit(1);
    }

    ptr = (sem_t *) mmap (NULL, sizeof(sem_t)*12, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    close(fd1);


    if(pusher_id==0){
		
		
		pusher_sugar();
	}
	else if(pusher_id==1){
		
		
		pusher_milk();
	}
	else if(pusher_id==2){
		
		pusher_flour();
	}
	else if(pusher_id==3){
		
		pusher_wallnut();
	}
	else{
		printf("Wrong ID \n");
		return 0;
	}

    return 0;
}

void * pusher_sugar(){
	
    
    for ( ; ; )
	{
		// Wait for this pusher to be needed
		
		sem_wait(&ptr[2]);
		
		sem_wait(&ptr[1]);
		
		
		// Check if the other item we need is on the table
		
		if (shared_memory[1]=='T')
		{
			shared_memory[0]='T';
			
			sem_post(&ptr[11]);
		}
		else if (shared_memory[2]=='T')
		{
			
			shared_memory[0]='T';
			sem_post(&ptr[8]);
		}
		else if (shared_memory[3]=='T')
		{
			shared_memory[0]='T';
			sem_post(&ptr[6]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[0]='T';
		}

		sem_post(&ptr[1]);
	}

	return NULL;
    
 
}

void * pusher_milk(){
    
    for ( ; ; )
	{
		
		sem_wait(&ptr[3]);
		
		sem_wait(&ptr[1]);

		// Check if the other item we need is on the table
		
		if (shared_memory[0]=='T')
		{
			shared_memory[1]='T';
			
			sem_post(&ptr[11]);
		}
		else if (shared_memory[2]=='T')
		{
			shared_memory[1]='T';
			sem_post(&ptr[9]);
		}
		else if (shared_memory[3]=='T')
		{
			shared_memory[1]='T';
			sem_post(&ptr[10]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[1]='T';
		}

		sem_post(&ptr[1]);
	}

	return NULL;
    
   
}

void * pusher_flour(){
    
    for ( ; ; )
	{

		// Wait for this pusher to be needed
		sem_wait(&ptr[4]);
		sem_wait(&ptr[1]);

		// Check if the other item we need is on the table

		if (shared_memory[0]=='T')
		{
			
			shared_memory[2]='T';
			sem_post(&ptr[8]);
		}
		else if (shared_memory[1]=='T')
		{
			shared_memory[2]='T';
			sem_post(&ptr[9]);
		}
		else if (shared_memory[3]=='T')
		{
			shared_memory[2]='T';
			sem_post(&ptr[7]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[2]='T';
		}

		sem_post(&ptr[1]);
	}

	return NULL;
    
  
}

void * pusher_wallnut(){
    
    for ( ; ; )
	{
		
		// Wait for this pusher to be needed
		sem_wait(&ptr[5]);
		sem_wait(&ptr[1]);

		// Check if the other item we need is on the table
		
		if (shared_memory[0]=='T')
		{
			shared_memory[3]='T';
			sem_post(&ptr[6]);
		}
		else if (shared_memory[1]=='T')
		{
			shared_memory[3]='T';
			sem_post(&ptr[10]);
		}
		else if (shared_memory[2]=='T')
		{
			shared_memory[3]='T';
			sem_post(&ptr[7]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[3]='T';
		}

		sem_post(&ptr[1]);
	}

	return NULL;
    
    
}
void handle_sigint(int sig){

    printf("All Resources Are Free , Files Was Closed (Pusher)!!!  \n");
    
 
    exit(EXIT_SUCCESS);
}

void handle_sigusr1(int sig){
	
    exit(EXIT_SUCCESS);
}
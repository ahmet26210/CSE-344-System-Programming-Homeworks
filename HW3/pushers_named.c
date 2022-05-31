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
sem_t *chef_semaphors[6];

char* chef_semaphores_name[6]= {"chef_sem0000","chef_sem1111","chef_sem2222","chef_sem3333","chef_sem4444","chef_sem5555"};
char* shared_memory1 ="/shared4";
sem_t* pusher_lock;

sem_t *pusher_semaphores[4];
char* pusher_semaphores_name[4]= {"sugar_sem000","milk_sem111","flour_sem222","wallnut_sem333"};
int pusher_id;

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

	
    
    pusher_lock=sem_open("pusher7",O_RDWR,0666);

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

    pusher_semaphores[pusher_id]=sem_open(pusher_semaphores_name[pusher_id],O_RDWR,0666);

	
   
    if(pusher_id==0){
		chef_semaphors[0]=sem_open(chef_semaphores_name[0],O_RDWR,0666);
		chef_semaphors[2]=sem_open(chef_semaphores_name[2],O_RDWR,0666);
		chef_semaphors[5]=sem_open(chef_semaphores_name[5],O_RDWR,0666);
		
		pusher_sugar();
	}
	else if(pusher_id==1){
		chef_semaphors[3]=sem_open(chef_semaphores_name[3],O_RDWR,0666);
		chef_semaphors[4]=sem_open(chef_semaphores_name[4],O_RDWR,0666);
		chef_semaphors[5]=sem_open(chef_semaphores_name[5],O_RDWR,0666);
		
		pusher_milk();
	}
	else if(pusher_id==2){
		chef_semaphors[1]=sem_open(chef_semaphores_name[1],O_RDWR,0666);
		chef_semaphors[2]=sem_open(chef_semaphores_name[2],O_RDWR,0666);
		chef_semaphors[3]=sem_open(chef_semaphores_name[3],O_RDWR,0666);
		pusher_flour();
	}
	else if(pusher_id==3){
		chef_semaphors[0]=sem_open(chef_semaphores_name[0],O_RDWR,0666);
		chef_semaphors[1]=sem_open(chef_semaphores_name[1],O_RDWR,0666);
		chef_semaphors[4]=sem_open(chef_semaphores_name[4],O_RDWR,0666);
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
		
		sem_wait(pusher_semaphores[0]);
		
		sem_wait(pusher_lock);
		
		
		// Check if the other item we need is on the table
		
		if (shared_memory[1]=='T')
		{
			shared_memory[0]='T';
			
			sem_post(chef_semaphors[5]);
		}
		else if (shared_memory[2]=='T')
		{
			
			shared_memory[0]='T';
			sem_post(chef_semaphors[2]);
		}
		else if (shared_memory[3]=='T')
		{
			shared_memory[0]='T';
			sem_post(chef_semaphors[0]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[0]='T';
		}

		sem_post(pusher_lock);
	}

	return NULL;
    
 
}

void * pusher_milk(){
    
    for ( ; ; )
	{
		
		sem_wait(pusher_semaphores[1]);
		
		sem_wait(pusher_lock);

		// Check if the other item we need is on the table
		
		if (shared_memory[0]=='T')
		{
			shared_memory[1]='T';
			
			sem_post(chef_semaphors[5]);
		}
		else if (shared_memory[2]=='T')
		{
			shared_memory[1]='T';
			sem_post(chef_semaphors[3]);
		}
		else if (shared_memory[3]=='T')
		{
			shared_memory[1]='T';
			sem_post(chef_semaphors[4]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[1]='T';
		}

		sem_post(pusher_lock);
	}

	return NULL;
    
   
}

void * pusher_flour(){
    
    for ( ; ; )
	{

		// Wait for this pusher to be needed
		sem_wait(pusher_semaphores[2]);
		sem_wait(pusher_lock);

		// Check if the other item we need is on the table

		if (shared_memory[0]=='T')
		{
			
			shared_memory[2]='T';
			sem_post(chef_semaphors[2]);
		}
		else if (shared_memory[1]=='T')
		{
			shared_memory[2]='T';
			sem_post(chef_semaphors[3]);
		}
		else if (shared_memory[3]=='T')
		{
			shared_memory[2]='T';
			sem_post(chef_semaphors[1]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[2]='T';
		}

		sem_post(pusher_lock);
	}

	return NULL;
    
  
}

void * pusher_wallnut(){
    
    for ( ; ; )
	{
		
		// Wait for this pusher to be needed
		sem_wait(pusher_semaphores[3]);
		sem_wait(pusher_lock);

		// Check if the other item we need is on the table
		
		if (shared_memory[0]=='T')
		{
			shared_memory[3]='T';
			sem_post(chef_semaphors[0]);
		}
		else if (shared_memory[1]=='T')
		{
			shared_memory[3]='T';
			sem_post(chef_semaphors[4]);
		}
		else if (shared_memory[2]=='T')
		{
			shared_memory[3]='T';
			sem_post(chef_semaphors[1]);
		}
		else
		{
			// The other item's aren't on the table yet
			shared_memory[3]='T';
		}

		sem_post(pusher_lock);
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
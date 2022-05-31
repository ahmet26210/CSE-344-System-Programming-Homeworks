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
#include<string.h>

sem_t *chef_semaphors[6];


int temp = 0;

const char* agent_sem = "agent_sem99";
char* shared_memory1 ="/shared4";
// An agent semaphore represents items on the table
sem_t *agent_ready;

char* chef_semaphores_name[6]= {"chef_sem0000","chef_sem1111","chef_sem2222","chef_sem3333","chef_sem4444","chef_sem5555"};
int type_id=0;
int number_of_dessert=0;
int id;
void handle_sigusr1(int sig);
void handle_sigint(int sig);

char* shared_memory;
void chef(char* ingr1 , char* ingr2 , char* wholesaler_id);

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

    id=getpid();

    type_id = atoi(argv[0]);
    agent_ready=sem_open(agent_sem,O_RDWR,0666);
    chef_semaphors[type_id]=sem_open(chef_semaphores_name[type_id],O_RDWR,0666);


    int fd = shm_open(argv[1],  O_RDWR , 0666);

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

    chef(argv[2],argv[3],argv[4]);



    return number_of_dessert;
}

void chef(char* ingr1 , char* ingr2 , char* wholesaler_id)
{
    
	int id =getpid();
	for (; ;)
	{
         
		printf("Chef %d (pid %d) is waiting for %s and %s  \n",type_id,id,ingr1,ingr2);
        fflush(stdout);

		sem_wait(chef_semaphors[type_id]);
        if(temp==34){
            break;
        }
        number_of_dessert++;
        
        char ingredient3 ='-';
        char ingredient4 ='-';
        for (int i = 0; i < 4; i++)
        {
            if(shared_memory[i]=='T'){
                
                if(ingredient3=='-'){
                  
                    if(i==0){
                        ingredient3='S';

                       
                    }
                    else if(i==1){
                        ingredient3='M';
                    }
                    else if(i==2){
                        ingredient3='F';
                    }
                    else if(i==3){
                        ingredient3='W';
                    }
                   
                }   

                else{
                   
                    if(i==0){
                        ingredient4='S';
                    }
                    else if(i==1){
                        ingredient4='M';
                    }
                    else if(i==2){
                      
                        ingredient4='F';

                    }
                    else if(i==3){
                        ingredient4='W';
                    }
                   
                    break;
                }
            }
        }
        if(ingredient3=='S'){
            printf("Chef %d (pid %d) has taken the sugar \n",type_id,id);
            fflush(stdout);
        }
        else if(ingredient3=='M'){
            printf("Chef %d (pid %d) has taken the milk \n",type_id,id);
            fflush(stdout);
        }
        else if(ingredient3=='F'){
            printf("Chef %d (pid %d) has taken the flour \n",type_id,id);
            fflush(stdout);
        }
        else if(ingredient3=='W'){
            printf("Chef %d (pid %d) has taken the wallnut \n",type_id,id);
            fflush(stdout);
        }
        
        if(ingredient4=='S'){
            printf("Chef %d (pid %d) has taken the sugar \n",type_id,id);
            fflush(stdout);
        }
        else if(ingredient4=='M'){
            printf("Chef %d (pid %d) has taken the milk \n",type_id,id);
            fflush(stdout);
        }
        else if(ingredient4=='F'){
            printf("Chef %d (pid %d) has taken the flour \n",type_id,id);
            fflush(stdout);
        }
        else if(ingredient4=='W'){
            printf("Chef %d (pid %d) has taken the wallnut \n",type_id,id);
            fflush(stdout);
        }
        

		printf("Chef %d (pid %d) is preparing the dessert \n",type_id,id);
        fflush(stdout);

        printf("Chef %d (pid %d) has delivered the dessert \n",type_id,id);
        fflush(stdout);

        printf("Shared memory items:");

        for (size_t i = 0; i < 4; i++)
        {
            if(shared_memory[i]=='T'){
                
                if(i==0){
                    printf(" Sugar");
                }
                if(i==1){
                    printf(" Milk");
                }
                if(i==2){
                    printf(" Flour");
                }
                if(i==3){
                    printf(" Wallnut");
                }   
            }
        }

        printf("\n");

        for (size_t i = 0; i < 4; i++)
        {
            shared_memory[i] = 'F';
        }
        

		usleep(rand() % 50000);
		sem_post(agent_ready);

		printf("The wholesaler (pid %s) has obtained the dessert and left \n",wholesaler_id);
		printf("\n");
        fflush(stdout);

		usleep(rand() % 50000);

	}

	return;
}

void handle_sigusr1(int sig){
    printf("chef%d has %d desert\n",type_id,number_of_dessert);
    fflush(stdout);

    printf("chef%d (pid %d) is exiting...\n",type_id,id);

    temp=34;
    
    sem_post(chef_semaphors[type_id]);
}
void handle_sigint(int sig){

    printf("chef%d has %d desert\n",type_id,number_of_dessert);
    fflush(stdout);

    printf("chef%d (pid %d) is exiting...\n",type_id,id);

    temp=34;
    
    sem_post(chef_semaphors[type_id]);

    printf("All Resources Are Free , Files Was Closed (Chef)!!!  \n");

  
}
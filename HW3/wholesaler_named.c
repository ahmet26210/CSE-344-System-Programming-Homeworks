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

extern int errno;
int fp;

char* array;
char* shared_memory1 ="/shared4";
int total_number_of_dessert=0;
const char* agent_sem = "agent_sem99";

// An agent semaphore represents items on the table
sem_t *agent_ready;

int chefs_id[6];
int index1=0;

int pusher_id[4];
int index2=0;
// Each chef semaphore represents when a chef has the items they need
sem_t *chef_semaphors[6];
char* chef_types[6] = { "0","1","2","3","4","5"};
char* chef_semaphores_name[6]= {"chef_sem0000","chef_sem1111","chef_sem2222","chef_sem3333","chef_sem4444","chef_sem5555"};
// This list represents item types that are on the table. This should corrispond
// with the chef_types, such that each item is the one the chef has. So the
// first item would be paper, then tobacco, then matches.


sem_t* pusher_lock;

// Each pusher pushes a certian type item, manage these with this semaphore
sem_t *pusher_semaphores[4];
char* pusher_semaphores_name[4]= {"sugar_sem000","milk_sem111","flour_sem222","wallnut_sem333"};
char* pusher_types[4] = {"0","1","2","3"};


int id;


void handle_sigint(int sig);

int main( int argc, char * argv [] ){
    
    if(argc!=5){
        perror("Invalid Input Please Enter Correctly ex: \n (./hw3named -i inputFilePath -n name \n");
        return 0;
    }

    id = getpid();
    char wholesaler_id[20];
    sprintf(wholesaler_id,"%d",id);

    char* chefs[6][2]={{"milk","flour"},{"milk","sugar"},{"milk","walnuts"},{"sugar","walnuts"},{"sugar","flour"},{"flour","walnuts"}};

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;        
    sa.sa_handler = &handle_sigint;

    if(sigaction(SIGINT,&sa,NULL) == - 1){
        perror("Sigaction error! \n");
        return 0;
    }

    sem_unlink(agent_sem);
    sem_unlink("pusher7");

    sem_unlink(pusher_semaphores_name[0]);
    sem_unlink(pusher_semaphores_name[1]);
    sem_unlink(pusher_semaphores_name[2]);
    sem_unlink(pusher_semaphores_name[3]);

    sem_unlink(chef_semaphores_name[0]);
    sem_unlink(chef_semaphores_name[1]);
    sem_unlink(chef_semaphores_name[2]);
    sem_unlink(chef_semaphores_name[3]);
    sem_unlink(chef_semaphores_name[4]);
    sem_unlink(chef_semaphores_name[5]);


    
    agent_ready=sem_open(agent_sem,O_CREAT | O_RDWR ,0666,1);

    pusher_lock=sem_open("pusher7",O_CREAT | O_RDWR ,0666,1);

    pusher_semaphores[0]=sem_open(pusher_semaphores_name[0],O_CREAT | O_RDWR  ,0666,0);
    pusher_semaphores[1]=sem_open(pusher_semaphores_name[1],O_CREAT | O_RDWR  ,0666,0);
    pusher_semaphores[2]=sem_open(pusher_semaphores_name[2],O_CREAT | O_RDWR  ,0666,0);
    pusher_semaphores[3]=sem_open(pusher_semaphores_name[3],O_CREAT | O_RDWR  ,0666,0);
    
    chef_semaphors[0]=sem_open(chef_semaphores_name[0],O_CREAT | O_RDWR ,0666,0);
    chef_semaphors[1]=sem_open(chef_semaphores_name[1],O_CREAT | O_RDWR ,0666,0);
    chef_semaphors[2]=sem_open(chef_semaphores_name[2],O_CREAT | O_RDWR ,0666,0);
    chef_semaphors[3]=sem_open(chef_semaphores_name[3],O_CREAT | O_RDWR ,0666,0);
    chef_semaphors[4]=sem_open(chef_semaphores_name[4],O_CREAT | O_RDWR ,0666,0);
    chef_semaphors[5]=sem_open(chef_semaphores_name[5],O_CREAT | O_RDWR ,0666,0);

    for (size_t i = 0; i < 4; i++)
    {
        if(pusher_semaphores[i]==SEM_FAILED){
            perror("semephore");
            return 0;
        }
    }

    for (size_t i = 0; i < 6; i++)
    {
        if(chef_semaphors[i]==SEM_FAILED){
            perror("semephore");
            return 0;
        }
    }
    

    unlink(shared_memory1);

    int fd = shm_open(shared_memory1,  O_RDWR | O_CREAT | O_TRUNC, 0666);

    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(fd, sizeof(char) * 4)) {
        perror("ftruncate");
        exit(1);
    }

    char* shared_memory = (char*) mmap(NULL, sizeof(char) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    close(fd);

    shared_memory[0]='F'; // sugar
    shared_memory[1]='F'; // milk 
    shared_memory[2]='F'; // flour
    shared_memory[3]='F'; // wallnut
   

    mode_t mode = S_IRUSR | S_IWUSR;

    pid_t pid;
    
    for(int i=0;i<6;i++){

        pid =fork();

        if(pid==0){
            char* argv_l[] = {
                chef_types[i],shared_memory1,chefs[i][0],chefs[i][1],wholesaler_id,NULL
            };

            execv("./ch", argv_l);
            perror("Exevce Error ! \n");
            exit(EXIT_SUCCESS);
            
        }
        else{
            chefs_id[index1++]=pid;
        }

    }

    for(int i=0;i<4;i++){

        pid =fork();

        if(pid==0){
            char* argv_l[] = {
                pusher_types[i],shared_memory1, NULL
            };

                    
            execv("./push", argv_l);
            perror("Exevce Error ! \n");
            exit(EXIT_SUCCESS);
            
        }
        else{
            pusher_id[index2++]=pid;
        }

    } 

    int read_bytes = 0;

    int index = 0 ;
    
    char buf[1];

    
    array = (char *)malloc(2048 * sizeof(char));

    if(array == NULL){
        perror("error");
        return 0;
    }

    if((fp = open(argv[2],O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");

        return 0;

    }
    
    for( ; ; ) {
       
        while (((read_bytes = read(fp, buf,1)) == -1) 
            && (errno == EINTR)){
            continue;
        }

        if (read_bytes <= 0){

            sem_wait(agent_ready);
            

            if((array[index-2]=='S' || array[index-2]=='F') && (array[index-1]=='S' || array[index-1]=='F')){

                printf("The wholesaler (pid %d) delivers Sugar and Flour.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[2]);
                sem_post(pusher_semaphores[0]);
                
                
            }
            else if((array[index-2]=='S' || array[index-2]=='M') && (array[index-1]=='S' || array[index-1]=='M')){

                printf("The wholesaler (pid %d) delivers Sugar and Milk.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[0]);
                sem_post(pusher_semaphores[1]);

            }
            else if((array[index-2]=='S' || array[index-2]=='W') && (array[index-1]=='S' || array[index-1]=='W')){

                printf("The wholesaler (pid %d) delivers Sugar and Wallnut.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[0]);
                sem_post(pusher_semaphores[3]);
            }
            else if((array[index-2]=='F' || array[index-2]=='M') && (array[index-1]=='F' || array[index-1]=='M')){

                printf("The wholesaler (pid %d) delivers Flour and Milk.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[1]);
                sem_post(pusher_semaphores[2]);
            }
            else if((array[index-2]=='F' || array[index-2]=='W') && (array[index-1]=='F' || array[index-1]=='W')){

                printf("The wholesaler (pid %d) delivers Flour and Wallnut.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[2]);
                sem_post(pusher_semaphores[3]);
            }
            else if((array[index-2]=='M' || array[index-2]=='W') && (array[index-1]=='M' || array[index-1]=='W')){

                printf("The wholesaler (pid %d) delivers Milk and Wallnut.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[1]);
                sem_post(pusher_semaphores[3]);
            }
            printf("the wholesaler (pid %d) is waiting for the dessert\n",id);

            break;
        } 

        if(*buf=='\n'){

            sem_wait(agent_ready);
            

            if((array[index-2]=='S' || array[index-2]=='F') && (array[index-1]=='S' || array[index-1]=='F')){

                printf("The wholesaler (pid %d) delivers Sugar and Flour.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[2]);
                sem_post(pusher_semaphores[0]);
                
                
            }
            else if((array[index-2]=='S' || array[index-2]=='M') && (array[index-1]=='S' || array[index-1]=='M')){

                printf("The wholesaler (pid %d) delivers Sugar and Milk.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[0]);
                sem_post(pusher_semaphores[1]);

            }
            else if((array[index-2]=='S' || array[index-2]=='W') && (array[index-1]=='S' || array[index-1]=='W')){

                printf("The wholesaler (pid %d) delivers Sugar and Wallnut.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[0]);
                sem_post(pusher_semaphores[3]);
            }
            else if((array[index-2]=='F' || array[index-2]=='M') && (array[index-1]=='F' || array[index-1]=='M')){

                printf("The wholesaler (pid %d) delivers Flour and Milk.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[1]);
                sem_post(pusher_semaphores[2]);
            }
            else if((array[index-2]=='F' || array[index-2]=='W') && (array[index-1]=='F' || array[index-1]=='W')){

                printf("The wholesaler (pid %d) delivers Flour and Wallnut.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[2]);
                sem_post(pusher_semaphores[3]);
            }
            else if((array[index-2]=='M' || array[index-2]=='W') && (array[index-1]=='M' || array[index-1]=='W')){

                printf("The wholesaler (pid %d) delivers Milk and Wallnut.\n",id);
                fflush(stdout);

                sem_post(pusher_semaphores[1]);
                sem_post(pusher_semaphores[3]);
            }
            printf("the wholesaler (pid %d) is waiting for the dessert\n",id);
            continue;
        }
        
        array[index++]=*buf;
       
    }

    sleep(1);
    
    for (size_t i = 0; i < 6; i++)
    {   
        
        kill(chefs_id[i],SIGUSR1);
    }

    for (size_t i = 0; i < 4; i++)
    {   
        
        kill(pusher_id[i],SIGUSR1);
    }


    for (size_t i = 0; i < 6; i++)
    {
        int wstatus;
        waitpid(chefs_id[i], &wstatus, 0); // Store proc info into wstatus
        int return_value = WEXITSTATUS(wstatus); // Extract return value from wstatus
        total_number_of_dessert+=return_value;
    }
    
    
    printf("\nThe wholesaler (pid %d) is done (total desserts: %d)\n",id,total_number_of_dessert);

    
    sleep(1);

    close(fp);

    for (size_t i = 0; i < 6; i++)
    {
        sem_close(chef_semaphors[i]);
    }
    for (size_t i = 0; i < 4; i++)
    {
        sem_close(pusher_semaphores[i]);
    }

    sem_close(agent_ready);
    sem_close(pusher_lock);

  
    sem_unlink(agent_sem);
    sem_unlink("pusher7");

    sem_unlink(pusher_semaphores_name[0]);
    sem_unlink(pusher_semaphores_name[1]);
    sem_unlink(pusher_semaphores_name[2]);
    sem_unlink(pusher_semaphores_name[3]);

    sem_unlink(chef_semaphores_name[0]);
    sem_unlink(chef_semaphores_name[1]);
    sem_unlink(chef_semaphores_name[2]);
    sem_unlink(chef_semaphores_name[3]);
    sem_unlink(chef_semaphores_name[4]);
    sem_unlink(chef_semaphores_name[5]);
    
    free(array);
    unlink("/shared4");
    unlink("/semephore");

    
    return 0;
}
void handle_sigint(int sig){

    for (size_t i = 0; i < 6; i++)
    {   
        
        kill(chefs_id[i],SIGUSR1);
    }

    for (size_t i = 0; i < 4; i++)
    {   
        
        kill(pusher_id[i],SIGUSR1);
    }
    

    for (size_t i = 0; i < 6; i++)
    {
        int wstatus;
        waitpid(chefs_id[i], &wstatus, 0); // Store proc info into wstatus
        int return_value = WEXITSTATUS(wstatus); // Extract return value from wstatus
        total_number_of_dessert+=return_value;
    }
    
    printf("\nThe wholesaler (pid %d) is done (total desserts: %d)\n",id,total_number_of_dessert);
    fflush(stdout);
    
    printf("All Resources Are Free , Files Was Closed (Wholesaler) !!!  \n");
    close(fp);
    free(array);
    unlink("/shared4");
    unlink("/semephore");

    for (size_t i = 0; i < 6; i++)
    {
        sem_close(chef_semaphors[i]);
    }
    for (size_t i = 0; i < 4; i++)
    {
        sem_close(pusher_semaphores[i]);
    }

    
    sem_close(agent_ready);
    sem_close(pusher_lock);

    
    sem_unlink(agent_sem);
    sem_unlink("pusher7");

    sem_unlink(pusher_semaphores_name[0]);
    sem_unlink(pusher_semaphores_name[1]);
    sem_unlink(pusher_semaphores_name[2]);
    sem_unlink(pusher_semaphores_name[3]);

    sem_unlink(chef_semaphores_name[0]);
    sem_unlink(chef_semaphores_name[1]);
    sem_unlink(chef_semaphores_name[2]);
    sem_unlink(chef_semaphores_name[3]);
    sem_unlink(chef_semaphores_name[4]);
    sem_unlink(chef_semaphores_name[5]);
    
    exit(EXIT_SUCCESS);
    
}
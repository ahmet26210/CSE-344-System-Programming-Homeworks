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
#include<sys/shm.h>
#include<sys/stat.h>
#include<pthread.h>
#include<string.h>
#include<sys/sem.h>
#include <sys/time.h>

union semun { /* Used in calls to semctl() */
 int val;
 struct semid_ds * buf;
 unsigned short * array;
 struct seminfo * __buf;
};

void* supplier_function(void* args);
void* consumer_function(void* args);
void handle_sigint(int sig);

int fp;
union semun arg;
struct flock lock;
char* file_name;
int semid;
int number_of_consumer,number_of_repeated;
time_t tm;
int* a;
pthread_attr_t detachedThread;
pthread_t* th1;
int main( int argc, char * argv [] ){
    
    if(argc!=7){
        perror("Invalid Input Please Enter Correctly ex: \n (./hw4 -C 10 -N 5 -F inputfilePath \n");
        return 0;
    }

    number_of_consumer = atoi(argv[2]);

    number_of_repeated = atoi(argv[4]);

    file_name=argv[6];

    
    unsigned short values[2] = {0,0};
    

    arg.array = values;
    
    int nsems = 2;
    semid = semget(IPC_PRIVATE, nsems, S_IRUSR | S_IWUSR); // IPC_CREAT | IPC_EXCL

    if(semid == -1){
        perror("semget");
        exit(1);    
    }

    if (semctl (semid,0, SETALL, arg) == -1)
    {
        perror("semctl");
        exit(1);
    }

    if(number_of_consumer < 5 || number_of_repeated < 2){
        fprintf(stdout,"Invalid C or N size. Try Again ! \n");
        return 0;
    }

    struct sigaction sa2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;        
    sa2.sa_handler = &handle_sigint;

    if(sigaction(SIGINT,&sa2,NULL) == - 1){
        
        perror("error");
        return 0;
    }

    a = malloc(sizeof(int)*number_of_consumer);

    for (int i = 0; i < number_of_consumer; i++)
    {
        a[i] = i;
    }
    

    

    th1 = malloc(sizeof(pthread_t)*number_of_consumer);

    int i;
    
    for (i = 0; i < number_of_consumer; i++) {
       
        if (pthread_create(&th1[i], NULL, &consumer_function,(void*)&a[i]) != 0) {
            perror("Failed to create thread");
        }
    }

    pthread_t th;
    
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED);
    
    if (pthread_create(&th, &detachedThread, &supplier_function, NULL) != 0) {
        perror("Failed to create thread");
    }
    
    pthread_attr_destroy(&detachedThread);
    
    for (i = 0; i < number_of_consumer; i++) {
    
        if (pthread_join(th1[i],NULL) != 0) {
            perror("Failed to join thread");
        }
        
    }

    if (semctl(semid, 0, IPC_RMID) == -1) // remove the set
    {    
        perror("semctl-close");
        exit(1);
    }

    free(a);

    free(th1);
    
    return 0;


}
void* supplier_function(void* args) {
    
    struct sembuf sops;
    sops.sem_op = 1;

    int read_bytes = 0;

    char buf[1];

    mode_t mode = S_IRUSR | S_IWUSR;

    if((fp = open(file_name,O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");

        exit(1);

    }

    for( ; ; ) {

        if (semctl(semid, 0, GETALL, arg) == -1)
        {    
            perror("semctl-GETALL");
            exit(1);
        }

        while (((read_bytes = read(fp, buf,1)) == -1) 
            && (errno == EINTR)){
            continue;
        }
        
        if(*buf!='\n' && read_bytes > 0){
            memset(&lock , 0 ,sizeof(lock));

            lock.l_type = F_WRLCK;

            fcntl(fp,F_SETLKW,&lock);

            time(&tm);
            printf("%s",ctime(&tm));
            fprintf(stdout,"Supplier: read from input a ‘%c’. Current amounts: %d x ‘1’, %d x ‘2’.\n",*buf,arg.array[0]/2,arg.array[1]/2);
            setbuf(stdout,NULL);

            lock.l_type =F_UNLCK;

            fcntl(fp,F_SETLKW,&lock);
        }
        

        if (read_bytes <= 0){

            if(*buf == '1'){
            
                sops.sem_num = 0;
                
                if(semop(semid,&sops,1) == -1){
                    perror("semop");
                    exit(1);
                }
            }

            if(*buf == '2'){
            
                sops.sem_num = 1;
                if(semop(semid,&sops,1) == -1){
                    perror("semop");
                    exit(1);
                }
            }

            break;
        } 

        if(*buf=='\n'){

            continue;
        }
        
        if(*buf == '1'){
            
            sops.sem_num = 0;
            
            if(semop(semid,&sops,1) == -1){
                perror("semop");
                exit(1);
            }
        }

        if(*buf == '2'){
           
            sops.sem_num = 1;
            if(semop(semid,&sops,1) == -1){
                perror("semop");
                exit(1);
            }
        }
        
        if (semctl(semid, 0, GETALL, arg) == -1)
        {    
            perror("semctl-GETALL");
            exit(1);
        }

        memset(&lock , 0 ,sizeof(lock));

        lock.l_type = F_WRLCK;

        fcntl(fp,F_SETLKW,&lock);

        time(&tm);
        printf("%s",ctime(&tm));
        fprintf(stdout,"Supplier: delivered a ‘%c’. Post-delivery amounts: %d x ‘1’, %d x ‘2’.\n\n",*buf,(arg.array[0]+1)/2,(arg.array[1]+1)/2);
        setbuf(stdout,NULL);

        lock.l_type =F_UNLCK;

        fcntl(fp,F_SETLKW,&lock);

        if(*buf == '1'){
            
            sops.sem_num = 0;
            
            if(semop(semid,&sops,1) == -1){
                perror("semop");
                exit(1);
            }
        }

        if(*buf == '2'){
        
            sops.sem_num = 1;
            if(semop(semid,&sops,1) == -1){
                perror("semop");
                exit(1);
            }
        }

    }

    close(fp);

    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_WRLCK;

    fcntl(fp,F_SETLKW,&lock);

    time(&tm);
    printf("%s",ctime(&tm));
    fprintf(stdout,"The Supplier has left \n");
    setbuf(stdout,NULL);

    lock.l_type =F_UNLCK;

    fcntl(fp,F_SETLKW,&lock);

    pthread_exit(0);

}

void* consumer_function(void* args) {

    int index = *(int*)args;

    struct sembuf sops[2];
    sops[0].sem_num = 0;
    sops[0].sem_op = -2;

    sops[1].sem_num = 1;
    sops[1].sem_op = -2;


    for (int i = 0; i < number_of_repeated; i++)
    {
        if (semctl(semid, 0, GETALL, arg) == -1)
        {    
            perror("semctl-GETALL");
            exit(1);
        }

        memset(&lock , 0 ,sizeof(lock));
       
        lock.l_type = F_WRLCK;

        fcntl(fp,F_SETLKW,&lock);

        time(&tm);
        printf("%s",ctime(&tm));
        fprintf(stdout,"Consumer-%d at iteration %d (waiting). Current amounts: %d x ‘1’, %d x ‘2’.\n",index,i,arg.array[0]/2,arg.array[1]/2);
        setbuf(stdout,NULL);

        lock.l_type =F_UNLCK;

        fcntl(fp,F_SETLKW,&lock);

        if(semop(semid,sops,2) == -1){
            perror("semop");
            exit(1);
        }

        if (semctl(semid, 0, GETALL, arg) == -1)
        {    
            perror("semctl-GETALL");
            exit(1);
        }

        memset(&lock , 0 ,sizeof(lock));

        lock.l_type = F_WRLCK;

        fcntl(fp,F_SETLKW,&lock);

        time(&tm);
        printf("%s",ctime(&tm));
        fprintf(stdout,"Consumer-%d at iteration %d (consumed). Post-consumption amounts: %d x ‘1’, %d x ‘2’.\n",index,i,arg.array[0]/2,arg.array[1]/2);
        setbuf(stdout,NULL);

        lock.l_type =F_UNLCK;

        fcntl(fp,F_SETLKW,&lock);

    }

    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_WRLCK;

    fcntl(fp,F_SETLKW,&lock);

    time(&tm);
    printf("%s",ctime(&tm));
    fprintf(stdout,"Consumer-%d has left.\n",index);
    setbuf(stdout,NULL);

    lock.l_type =F_UNLCK;

    fcntl(fp,F_SETLKW,&lock);

    pthread_exit(0);
}

void handle_sigint(int sig){

    printf("EXITING GRACEFULLY \n");

    for (int i = 0; i < number_of_consumer; i++) {
    
        if (pthread_join(th1[i],NULL) != 0) {
            perror("Failed to join thread");
        }
        
    }

    if (semctl(semid, 0, IPC_RMID) == -1) // remove the set
    {    
        perror("Already Closed\n");

    }
    
    free(a);

    free(th1);
    
    close(fp);

    pthread_attr_destroy(&detachedThread);
    
    pthread_exit(0);

    exit(1);

}
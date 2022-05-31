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

#define BD_NO_CHDIR  01
#define BD_NO_CLOSE_FILES  02
#define BD_NO_REOPEN_STD_FDS  04
#define BD_NO_UMASK0 010
#define BD_MAX_CLOSE 8192

typedef struct Client{
	int matrix_size;
	int clientID;
	int matrix[1024];
}Client;

char fifo[50];

char str1[80];
char str[80];
int number_of_clients=0;
sem_t* number_of_clients1=0;
sem_t* number_client=0;
int fp;
int fd6;
int fd5;
int fd2[2];
extern int errno;
sem_t *sem;
sem_t *sem1;

sem_t* Total_request=0;
sem_t* invertible_request=0;
sem_t* not_invertible_request=0;
sem_t* forwarded_request=0;
sem_t* invertible_request_serverZ=0;
sem_t* not_invertible_request_serverZ=0;

int isInvertible(int mat[1024], int n); 

int getDeterminant(int** mat, int n);
void findCoFactor(int** mat, int** mat2, int p, int q, int n);
void handle_sigusr1(int sig);
void handle_sigusr2(int sig);
void handle_sigusr3(int sig);
void handle_sigusr4(int sig);
void serverZ();
int becomeDaemon(int flags);
void handle_sigint_server_Y(int sig);
void handle_sigint_server_Z(int sig);

int main( int argc, char * argv [] ){

    
    int poolSize;
    int poolSize2;
    int duration;

    if(argc!=11){
        perror("Invalid Input Please Enter Correctly ex: \n (./serverY -s pathToServerFifo -o pathToLogFile -p poolSize -r poolSize2 -t 2) \n");
        return 0;
    }
    int byteswritten=0;
    mode_t mode = S_IRUSR | S_IWUSR;

    if((fp = open(argv[4],O_CREAT | O_WRONLY | O_TRUNC ,mode)) == -1){

        perror("Failed To Open Input File !\n");

        return 0;

    }

    char a[200];

    strcpy(fifo,argv[2]);
   

    poolSize=atoi(argv[6]);
    poolSize2=atoi(argv[8]);
    duration=atoi(argv[10]);

    strcpy(a,"Invalid Size\n");

    if(poolSize<2 || poolSize2<2){
        while(((byteswritten = write(fp,"invalid size\n",14)) == -1 ) && (errno ==EINTR));
        close(fp);
        return 0;
    }   
    number_client = sem_open("num_client",O_CREAT,0666,0);
    Total_request = sem_open("total",O_CREAT,0666,0);
    invertible_request = sem_open("invertible",O_CREAT,0666,0);
    not_invertible_request = sem_open("non_invertible",O_CREAT,0666,0);
    forwarded_request = sem_open("forwarded",O_CREAT,0666,0);
    not_invertible_request_serverZ = sem_open("invertible_serverz",O_CREAT,0666,0);
    invertible_request_serverZ = sem_open("non_invertible_serverz",O_CREAT,0666,0);
    
    becomeDaemon(BD_NO_CHDIR | BD_NO_UMASK0 | BD_NO_CLOSE_FILES);

    sigset_t prevMask , intMask;
    sigemptyset(&intMask);
    sigaddset(&intMask,SIGINT);

    time_t tm;
    
    struct flock lock;
    
    
    
    strcpy(a,"Server Y started Instantiated server Z\n");
    time(&tm);
    byteswritten=0;

    while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
    if(byteswritten==0){
        strcpy(a,"error\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
        return 0;
    }
    byteswritten=0;

    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
    if(byteswritten==0){
        strcpy(a,"error\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
        return 0;
    }
    byteswritten=0;

     if(mkfifo(argv[2],0666) == -1){
        if(errno !=EEXIST){
            time(&tm);
            while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }
            byteswritten =0;

            strcpy(a,"Could not create fifo file \n");
            while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }
            byteswritten=0;

            return -1;
        }
    }

    

    int arr[110][110];
    int n;
    int client_id;

    int fd1[2];

    if(pipe(fd1)==-1){
        strcpy(a,"error\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
        return 2;
    }

    if(pipe(fd2)==-1){
        strcpy(a,"error\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
        return 2;
    }
    

    pid_t pid;
    pid_t pid1=-1;


    
    for(int i=0;i<poolSize;i++){
        pid =fork();

        if(pid==0){
            break;
        }

    }

    for( ; ; ){
        
        if(pid == 0){

            char write_str[200];
                   
            close(fd1[1]);

            Client childclients;

            if(read(fd1[0],&childclients,sizeof(Client))==-1){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return -1;
            }
            byteswritten=0;

            int worker_pid=getpid();

            
            

            if(childclients.matrix_size!=0){
                
                kill(getppid(),SIGUSR1);
            }
            int numberr;
            sem_getvalue(number_client,&numberr);
             snprintf(write_str, 200, "Worker PID %d is handling client PID# %d matrix size %dx%d pool busy %d / %d \n"
            , worker_pid,childclients.clientID,childclients.matrix_size,childclients.matrix_size,numberr,poolSize);
            
            memset(&lock , 0 ,sizeof(lock));

            lock.l_type = F_WRLCK;

            fcntl(fp,F_SETLKW,&lock);

            time(&tm);
            while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }
            byteswritten =0;
            
            while(((byteswritten = write(fp,&write_str,strlen(write_str))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }

            lock.l_type =F_UNLCK;

            fcntl(fp,F_SETLKW,&lock);

            
            
            

    
            snprintf(str, 80, "%d", childclients.clientID);

            if(mkfifo(str,0666) == -1){
                if(errno !=EEXIST){
                    strcpy(a,"Could not create fifo file \n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                    
                    return -1;
                }
            }

            

            if((fd6 = open(str,O_WRONLY,mode)) == -1){
                strcpy(a,"Failed To Open Fifo  \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
               

                return 0;

            }

            if(sigprocmask(SIG_BLOCK, &intMask , &prevMask) == -1){
                strcpy(a,"Critical Section Started No Interrupted ! \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }

            int result = isInvertible(childclients.matrix,childclients.matrix_size);

            if(write(fd6,&result,sizeof(int))==-1){
                return -1;
            }

            close(fd6);

            if(sigprocmask(SIG_SETMASK, &prevMask ,NULL) == -1){
                strcpy(a,"Critical Section End! \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                
            }

            if(result == 1){
                sem_post(invertible_request);
                snprintf(write_str, 200, "Worker PID %d responding to client PID# %d the matrix is invertible \n"
            , worker_pid,childclients.clientID);
            }
            else{
                sem_post(not_invertible_request);
                snprintf(write_str, 200, "Worker PID %d responding to client PID# %d the matrix is not invertible \n"
            , worker_pid,childclients.clientID);
            }

            memset(&lock , 0 ,sizeof(lock));

            lock.l_type = F_WRLCK;

            fcntl(fp,F_SETLKW,&lock);

            time(&tm);
            while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }
            byteswritten =0;
            
            while(((byteswritten = write(fp,&write_str,strlen(write_str))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return 0;
            }

            lock.l_type =F_UNLCK;

            fcntl(fp,F_SETLKW,&lock);
            

            sleep(duration);
            
            

            if(childclients.matrix_size!=0){
                
                kill(getppid(),SIGUSR2);
            }
            
            unlink(str);
        }
        else{

            struct sigaction sa2;
            sigemptyset(&sa2.sa_mask);
            sa2.sa_flags = 0;        
            sa2.sa_handler = &handle_sigint_server_Y;

            if(sigaction(SIGINT,&sa2,NULL) == - 1){
                
                if(byteswritten==0){
                    strcpy(a,"error\n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                    return 0;
                }
                byteswritten=0;
                return 0;
            }

            struct sigaction sa;
            sa.sa_flags = SA_RESTART;
            sa.sa_handler = &handle_sigusr1;

            sigaction(SIGUSR1,&sa,NULL);

            struct sigaction sa1;
            sa1.sa_flags = SA_RESTART;
            sa1.sa_handler = &handle_sigusr2;

            sigaction(SIGUSR2,&sa1,NULL);

            
            Client clients;
            
            
            int fd =open(argv[2],O_RDONLY);

            sem_post(Total_request);
            

            if(fd == -1){
                strcpy(a,"error \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return -1;
            }
            
            if(read(fd,&client_id,sizeof(int)) == -1){
                strcpy(a,"error \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return -1;
            }
        
            if(read(fd,&n,sizeof(int))==-1){
                strcpy(a,"error \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return -1;
            }
            
            

            int i,j;

            for(i = 0; i< n ; i++) {
                for(j = 0;j<n; j++){
                    if(read(fd,&arr[i][j],sizeof(int))==-1){
                        return -1;
                    }
                }
            }

            close(fd);

            clients.matrix_size=n;
            clients.clientID=client_id;

            
            
            int k=0;
            for(int i=0;i<n;i++){
                for(int j = 0; j<n ; j++){
                    clients.matrix[k]=arr[i][j];
                    k++;
                }
            }
            
            
           

            if(number_of_clients==poolSize){
                sem_post(forwarded_request);
                char write_str[200];

                int num_client1;
                sem_getvalue(number_client,&num_client1);
                snprintf(write_str, 200, "Forwarding request of client PID# %d to serverZ,matrix size %dx%d pool busy %d / %d \n"
            ,clients.clientID,clients.matrix_size,clients.matrix_size,num_client1,poolSize);

                
                memset(&lock , 0 ,sizeof(lock));

                lock.l_type = F_WRLCK;

                fcntl(fp,F_SETLKW,&lock);

                time(&tm);
                while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
                if(byteswritten==0){
                    strcpy(a,"error\n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                    return 0;
                }
                byteswritten =0;
                
                while(((byteswritten = write(fp,&write_str,strlen(write_str))) == -1 ) && (errno ==EINTR));
                if(byteswritten==0){
                    strcpy(a,"error\n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                    return 0;
                }

                lock.l_type =F_UNLCK;

                fcntl(fp,F_SETLKW,&lock);

               
                if(pid1==-1){
                    pid1 = fork();
                    if(pid1==0){

                                           
                        if(write(fd2[1],&clients,sizeof(Client)) <0){

                            strcpy(a,"error \n");
                            while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                            
                            return 0;
                        }

            

                        serverZ(fd2,poolSize2,duration);
                    }
                }
                else{
                    
                    if(write(fd2[1],&clients,sizeof(Client)) <0){

                        strcpy(a,"error \n");
                        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                        
                        return 0;
                    }

                
                    
                }
                
            }
            else{
                close(fd1[0]);

                if(write(fd1[1],&clients,sizeof(Client)) <0){

                    strcpy(a,"error \n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                
                    return 0;
                }

        
            }

        }
        
    }
    
    
    return 0;
}



void handle_sigusr1(int sig){
    sem_post(number_client);
    number_of_clients++;
}

void handle_sigusr2(int sig){
    sem_wait(number_client);
    number_of_clients--;
}
void handle_sigusr3(int sig){
    
   sem_post(number_of_clients1);
}

void handle_sigusr4(int sig){
    
    sem_wait(number_of_clients1);
}
void handle_sigint_server_Y(int sig){
    
    int byteswritten;
    char a[200];
    int total,inv,notinv,forw;
    sem_getvalue(Total_request,&total);sem_getvalue(invertible_request,&inv);sem_getvalue(not_invertible_request,&notinv);sem_getvalue(forwarded_request,&forw);
    snprintf(a, 200, "SIGINT received, terminating Z and exiting server Y. Total requests handled:%d , %d invertible , %d not. %d request were forwarded.\n"
            ,total,inv,notinv,forw);
    struct flock lock;
    time_t tm;
    time(&tm);
    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_WRLCK;

    fcntl(fp,F_SETLKW,&lock);
    
    while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
   
    
    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
    
    lock.l_type =F_UNLCK;

    fcntl(fp,F_SETLKW,&lock);
    sem_close(number_client);
    sem_close(Total_request);
    sem_close(invertible_request);
    sem_close(invertible_request_serverZ);
    sem_close(not_invertible_request);
    sem_close(not_invertible_request_serverZ);
    sem_close(forwarded_request);
    sem_unlink("num_client");
    sem_unlink("total");
    sem_unlink("invertible");
    sem_unlink("non_invertible");
    sem_unlink("forwarded");
    sem_unlink("invertible_serverz");
    sem_unlink("non_invertible_serverz");

    unlink(str);
    unlink(fifo);
    close(fd2[0]);
    close(fd2[1]);
    close(fp);
    close(fd6);
    int child_pid;
    for(;;){

        child_pid =wait(NULL);
        if(child_pid == -1){
            if(errno == ECHILD){
                strcpy(a,"No more children bye \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                break;
            }
            else{
                strcpy(a,"Wait to kill children \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
            }
        }

    }

    exit(EXIT_SUCCESS);
}
void handle_sigint_server_Z(int sig){
    int byteswritten;
    char a[200];
    int forw,inv,notinv;
    sem_getvalue(forwarded_request,&forw);sem_getvalue(invertible_request_serverZ,&inv);sem_getvalue(not_invertible_request_serverZ,&notinv);
    snprintf(a, 200, "SIGINT received, exiting server Z. Total requests handled :%d , %d invertible , %d not..\n"
            ,forw,inv,notinv);
    struct flock lock;
    time_t tm;
    time(&tm);
    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_WRLCK;

    fcntl(fp,F_SETLKW,&lock);
    
    while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
   
    
    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
    
    lock.l_type =F_UNLCK;

    fcntl(fp,F_SETLKW,&lock);

    sem_close(sem);
    sem_close(sem1);
    sem_close(number_of_clients1);
    sem_unlink("num_of_client");
    sem_unlink("mutex");
    sem_unlink("mutex1");
    shm_unlink("/shared");
    unlink(str1);
    int child_pid;
    close(fd5);
    for(;;){

        child_pid =wait(NULL);
        if(child_pid == -1){
            if(errno == ECHILD){
                
                strcpy(a,"No more children bye \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                break;
            }
            else{
                
                strcpy(a,"Wait to kill children \n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
            }
        }

    }

    exit(EXIT_SUCCESS);
}

void serverZ(int fd1[2],int poolSize2,int duration){
    int byteswritten =0;
    char a[200];
    time_t tm;

    time(&tm);
    while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
    strcpy(a,"ServerZ is started \n");
    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));

    mode_t mode = S_IRUSR | S_IWUSR;
    
    int fd;

    struct sigaction sa2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;        
    sa2.sa_handler = &handle_sigint_server_Z;

    if(sigaction(SIGINT,&sa2,NULL) == - 1){
        
        if(byteswritten==0){
            strcpy(a,"error\n");
            while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
            return;
        }
        byteswritten=0;
        return;
    }
    
    fd = shm_open("/shared", O_CREAT | O_RDWR , 0666);

    if(fd == -1){
        strcpy(a,"Error while creating shared memory\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
        
        return ;
    }

    if(ftruncate(fd,sizeof(Client)*100) == -1){
        strcpy(a,"Error\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
    }
    struct flock lock;
    sigset_t prevMask , intMask;
    sigemptyset(&intMask);
    sigaddset(&intMask,SIGINT);

    sem = sem_open("mutex",O_CREAT,0666,0);
    sem1 = sem_open("mutex1",O_CREAT,0666,0);
    number_of_clients1 = sem_open("num_of_client",O_CREAT,0666,0);

    struct Client * shm = (struct Client *)mmap(NULL,sizeof(Client)*100,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
            
    if(shm == MAP_FAILED){
        strcpy(a,"Error in shared memory mapping.\n");
        while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
        return ;
    }

    pid_t pid;
    
    for(int i=0;i<poolSize2;i++){

        pid =fork();

        if(pid==0){
            break;
        }

    }

    for( ; ; ){
      
        if(pid == 0){
            char write_str[200];
            Client temp;

            

            if (sem_wait(sem) == -1){
                strcpy(a,"Could not create fifo file !\n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                    return;
            }
                

           

            temp.clientID=shm->clientID;
            temp.matrix_size=shm->matrix_size;
            for(int i =0; i<temp.matrix_size*temp.matrix_size;i++){
                temp.matrix[i]=shm->matrix[i];
            }


            if (sem_post(sem1) == -1){
                strcpy(a,"error !\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
            }
            
            if(temp.matrix_size!=0){
                
                kill(getppid(),SIGUSR1);
            }

            int worker_pid=getpid();
              int num_client1;
              sem_getvalue(number_of_clients1,&num_client1);
             snprintf(write_str, 200, "Worker PID %d is handling client PID# %d matrix size %dx%d pool busy %d / %d \n"
            , worker_pid,temp.clientID,temp.matrix_size,temp.matrix_size,num_client1,poolSize2);
            
            memset(&lock , 0 ,sizeof(lock));

            lock.l_type = F_WRLCK;

            fcntl(fp,F_SETLKW,&lock);

            time(&tm);
            while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }
            byteswritten =0;
            
            while(((byteswritten = write(fp,&write_str,strlen(write_str))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }

            lock.l_type =F_UNLCK;

            fcntl(fp,F_SETLKW,&lock);

            

            snprintf(str1, 80, "%d", temp.clientID);

            if(mkfifo(str1,0777) == -1){
                if(errno !=EEXIST){
                    strcpy(a,"Could not create fifo file !\n");
                    while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                    return ;
                }
            }

            if((fd5 = open(str1,O_WRONLY,mode)) == -1){
                strcpy(a,"Failed To Open Fifo !\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                

                return ;

            }

            if(sigprocmask(SIG_BLOCK, &intMask , &prevMask) == -1){
                strcpy(a,"critic section start!\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }

            int result = isInvertible(temp.matrix,temp.matrix_size);

            if(write(fd5,&result,sizeof(int))==-1){
                return ;
            }

            close(fd5);

            if(sigprocmask(SIG_SETMASK, &prevMask ,NULL) == -1){
                strcpy(a,"critic section end!\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }

            if(result == 1){
                sem_post(invertible_request);
                sem_post(invertible_request_serverZ);
                snprintf(write_str, 200, "Worker PID %d responding to client PID# %d the matrix is invertible \n"
            , worker_pid,temp.clientID);
            }
            else{
                sem_post(not_invertible_request);
                sem_post(not_invertible_request_serverZ);
                snprintf(write_str, 200, "Worker PID %d responding to client PID# %d the matrix is not invertible \n"
            , worker_pid,temp.clientID);
            }

            memset(&lock , 0 ,sizeof(lock));

            lock.l_type = F_WRLCK;

            fcntl(fp,F_SETLKW,&lock);

            time(&tm);
            while(((byteswritten = write(fp,ctime(&tm),strlen(ctime(&tm)))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return ;
            }
            byteswritten =0;
            
            while(((byteswritten = write(fp,&write_str,strlen(write_str))) == -1 ) && (errno ==EINTR));
            if(byteswritten==0){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return ;
            }

            lock.l_type =F_UNLCK;

            fcntl(fp,F_SETLKW,&lock);

        
            sleep(duration);

            if(temp.matrix_size!=0){
                
                kill(getppid(),SIGUSR2);
            }

            unlink(str1);
   
        }
        else{
            struct sigaction sa;
            sa.sa_flags = SA_RESTART;
            sa.sa_handler = &handle_sigusr3;

            if(sigaction(SIGUSR1,&sa,NULL)==-1){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }

            struct sigaction sa1;
            sa1.sa_flags = SA_RESTART;
            sa1.sa_handler = &handle_sigusr4;

            if(sigaction(SIGUSR2,&sa1,NULL) == -1){
                strcpy(a,"error\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }

           
                     
            Client clients;

            close(fd2[1]);

            if(read(fd2[0],&clients,sizeof(Client)) < 0){
                strcpy(a,"error !\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return ;    
            }
           

            memcpy(shm,&clients,sizeof(Client));

           

            if (sem_post(sem) == -1){
                strcpy(a,"error semephore !\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }
                
            if (sem_wait(sem1) == -1){
                strcpy(a,"error semephore !\n");
                while(((byteswritten = write(fp,&a,strlen(a))) == -1 ) && (errno ==EINTR));
                return;
            }
                  

        }
        
    }

}

int becomeDaemon(int flags){
    
    int maxfd, fd;
    switch (fork()) {
        case -1: return -1;
        case  0: break;
        default: _exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
        return -1; 

    switch (fork()) {
        case -1: return -1;
        case  0: break;
        default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0);

    if (!(flags & BD_NO_CHDIR))
        chdir("/");    

    if (!(flags & BD_NO_CLOSE_FILES)) {
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)
            maxfd = BD_MAX_CLOSE;
    }
    for (fd = 0; fd < maxfd; fd++)
        close(fd);
    
    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)
            return -1;

        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }
    return 0;
}

void findCoFactor(int** mat, int** mat2, int p, int q, int n) {
   int i = 0, j = 0;
   for (int row = 0; row < n; row++) {
      for (int col = 0; col < n; col++) {
         if (row != p && col != q) {
            mat2[i][j++] = mat[row][col];
            if (j == n - 1) {
               j = 0;
               i++;
            }
         }
      }
   }
}

int getDeterminant(int** mat, int n) {
   int determinant = 0;
   if (n == 1)
      return mat[0][0];
   int** temp;
   temp = (int **) malloc(n * sizeof(int*));

    
    for(int i =0; i< n ;i++){
        temp[i] = (int *)malloc(n *sizeof(int));
    } 
   int sign = 1;
   for (int f = 0; f < n; f++) {
      findCoFactor(mat, temp, 0, f, n);
      determinant += sign * mat[0][f] * getDeterminant(temp, n - 1);
      sign = -sign;
   }

   for(int k = 0;k < n ; k++){
        free(temp[k]);
    }
    free(temp);
   return determinant;
}

int isInvertible(int mat[1024], int n) {
    int** array;
    array = (int **) malloc(n * sizeof(int*));

    
    for(int i =0; i< n ;i++){
        array[i] = (int *)malloc(n *sizeof(int));
    }   
    

    int k=0;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            array[i][j]=mat[k];
            k++;
        }
    }

    if (getDeterminant(array, n) != 0){
        for(int k = 0;k < n ; k++){
            free(array[k]);
        }
        free(array);
        return 1;
    }
        
    else{
        for(int k = 0;k < n ; k++){
            free(array[k]);
        }
        free(array);
         return 0;
    }
       
}
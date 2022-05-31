#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<fcntl.h>
#include<time.h>
#include<signal.h>
#include<string.h>
int fd2;
int** matrices;
char* buff;
int counter1=0;
int fd1;
int fd;
void handle_client(int sig);
int main( int argc, char * argv [] ){

    if(argc!=5){
        perror("Invalid Input Please Enter Fifo and Data File Name: \n (./client -s pathToServerFifo -o pathToDataFile) \n");
        return 0;
    }

    clock_t begin = clock();
    
    mode_t mode = S_IRUSR | S_IWUSR;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;        
    sa.sa_handler = &handle_client;

    if(sigaction(SIGINT,&sa,NULL) == - 1){
        
        perror("error");
        return 0;
    }
    
    int read_bytes = 0;

    int index = 0 ,counter=0;
    
    char buf[1];

    buff = (char*)calloc(10,sizeof(char));

    if(buff == NULL){
        perror("Memory allocation error ! \n");
        return 0;
    }

    matrices = (int **) malloc((counter1 + 1) * sizeof(int));

    if(matrices == NULL){
        
        free(buff);
        perror("Memory allocation error ! \n");
        return 0;
    }

    matrices[counter1] = (int *)malloc((counter1 + 1) *sizeof(int));

    if(matrices[counter1] == NULL){
        
        free(buff);
        perror("Memory allocation error ! \n");
        free(matrices);
        return 0;
    }


    
    

    if((fd1 = open(argv[4],O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");

        return 0;

    } 

    for( ; ; ) {

        while (((read_bytes = read(fd1, buf,1)) == -1) 
            && (errno == EINTR)){
            continue;
        }

        if (read_bytes <= 0){
            
            int temp;
            sscanf(buff, "%d", &temp);
            matrices[counter1][counter]=temp;

            for(int i=0;i<10;i++){
                buff[i]=0;
            }

            counter1++;
            matrices = realloc(matrices, (counter1 + 1) * sizeof(int*));
            if (matrices == NULL){
                return 0;
            }
            matrices[counter1] = (int *)malloc(1*sizeof(int));

            if (matrices[counter1] == NULL){
                free(matrices);
                return 0;
            }
                
            counter = 0;
            index=0;

            break;
        } 

        
 
        if(*buf==','){
            int temp;
            sscanf(buff, "%d", &temp);
            matrices[counter1][counter]=temp;
            
            counter++;

            matrices[counter1] = realloc(matrices[counter1],(counter + 1 )*sizeof(int));
            index = 0;

            for(int i=0;i<10;i++){
                buff[i]=0;
            }

            continue;
        }

        if(*buf=='\n'){
            int temp;
            sscanf(buff, "%d", &temp);
            matrices[counter1][counter]=temp;

            for(int i=0;i<10;i++){
                buff[i]=0;
            }

            counter1++;
            matrices = realloc(matrices, (counter1 + 1) * sizeof(int*));
            if (matrices == NULL){
                return 0;
            }
            matrices[counter1] = (int *)malloc(1*sizeof(int));

            if (matrices[counter1] == NULL){
                free(matrices);
                return 0;
            }
                
            counter = 0;
            index=0;
           
            continue;
        }
        
        buff[index]=*buf;
        index++;
    }

    close(fd1);

    if(counter1<2){
        return 0;
    }

    
    
    if((fd = open(argv[2],O_WRONLY,mode)) == -1){

        perror("Failed To Open Fifo !\n");

        return 0;

    }
    
    pid_t client_pid = getpid();
    
    if(write(fd,&client_pid,sizeof(int))==-1){
        return -1;
    } 
    
    if(write(fd,&counter1,sizeof(int))==-1){
        return -1;
    } 
    

    for(int i=0;i<counter1;i++){
        for(int j = 0; j<counter1 ; j++){
            if(write(fd,&matrices[i][j],sizeof(int))==-1){
                return -1;
            }
        }
        
    }

    close(fd);

    sleep(4);

    char str[80];
    
    snprintf(str, 80, "%d", client_pid);

    int result;
    fd2 =open(str,O_RDONLY | O_TRUNC | O_EXCL);
        
    if(fd2 == -1){
        return -1;
    }
    
    if(read(fd2,&result,sizeof(int)) == -1){
        return -1;
    }
    int byteswritten=0;
    clock_t end = clock();
    double time_spent = 0.0;
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    char string[100];
    if(result == 1){
        snprintf(string,100,"Client PID#%d (%s) is submitting a %dx%d matrix. \n",client_pid,argv[4],counter1,counter1);
        while(((byteswritten = write(1,&string,strlen(string))) == -1 ) && (errno ==EINTR));
        if(byteswritten==0){
            perror("error");
            return 0;
        }
        byteswritten =0;
        snprintf(string,100,"Client PID#%d: Matrix is invertible.total time %lf seconds goodbye. \n",client_pid,time_spent);
        while(((byteswritten = write(1,&string,strlen(string))) == -1 ) && (errno ==EINTR));
        if(byteswritten==0){
            perror("error");
            return 0;
        }
        
    }
    else{
        snprintf(string,100,"Client PID#%d (%s) is submitting a %dx%d matrix. \n",client_pid,argv[4],counter1,counter1);
        while(((byteswritten = write(1,&string,strlen(string))) == -1 ) && (errno ==EINTR));
        if(byteswritten==0){
            perror("error");
            return 0;
        }
        byteswritten =0;
        snprintf(string,100,"Client PID#%d: Matrix is not invertible.total time %lf seconds goodbye. \n",client_pid,time_spent);
        while(((byteswritten = write(1,&string,strlen(string))) == -1 ) && (errno ==EINTR));
        if(byteswritten==0){
            perror("error");
            return 0;
        }
        
    }

    close(fd2);  

    for(int k = 0;k < counter1+1 ; k++){
        free(matrices[k]);
    }
    free(matrices);

    free(buff);
    

    return 0;
}

void handle_client(int sig){
    close(fd2);
    close(fd);
    close(fd1);

    for(int k = 0;k < counter1+1 ; k++){
        free(matrices[k]);
    }
    free(matrices);

    free(buff);

    printf("Exiting Gracefully ...\n");
    exit(EXIT_SUCCESS);
}
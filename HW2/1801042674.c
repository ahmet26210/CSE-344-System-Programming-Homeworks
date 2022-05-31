#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include<dirent.h>
#include<fcntl.h>
#include<math.h>
#include<signal.h>

#define TRUE 1
#define FALSE 0
#define INT_MAX 999999

#define READ_FLAGS (O_RDONLY)
#define BUF_SIZE 3

void check();
void handle_sigint(int sig);
double frobeniusNorm(double* metrice);

char* file_name;
int* child_process_id;
int child_process_number=0;
int fd,fd4;
char* buff;
double **matrices;
double* resultMatrice;
int counter1=0; 
sig_atomic_t chech_sigint=0;

int main( int argc, char * argv [] ){
    
    
    if(argc!=5){
        perror("Invalid Input Please Enter Input Output File Name: \n (./processP -i inputFilePath -o outputFilePath) \n");
        return 0;
    }

    check();

    printf("Calculating, Wait Please... \n");
    file_name = argv[4];

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;        
    sa.sa_handler = &handle_sigint;

    if(sigaction(SIGINT,&sa,NULL) == - 1){
        perror("Sigaction error! \n");
        return 0;
    }

    sigset_t prevMask , intMask;
    sigemptyset(&intMask);
    sigaddset(&intMask,SIGINT);
    
    char temp[30];
    int tempcount=0;
    char buffer[3];

    mode_t mode = S_IRUSR | S_IWUSR;

    int count = 0, read_bytes = 0, total_bytes = 0;

    if((fd = open(argv[2],READ_FLAGS,mode)) == -1){

        perror("Failed To Open Input File !\n");

        return 0;

    }   
   
    for(int j =0;j<30 ; j++){

        temp[j] = '\0';

    }

    check();

    if(sigprocmask(SIG_BLOCK, &intMask , &prevMask) == -1){
        perror("Critical Section Started No Interrupted ! \n");
    }

    child_process_id = (int *) malloc((child_process_number + 1) * sizeof(int));

    if(child_process_id == NULL){
        perror("Memory allocation error ! \n");
        return 0;
    }
    printf("Process P reading %s \n",argv[2]);
    for( ; ; ) {

        while (((read_bytes = read(fd, buffer, BUF_SIZE)) == -1) 
            && (errno == EINTR)){
            continue;
        }
       
        if (read_bytes <= 0) break;

        total_bytes += read_bytes;

        if(total_bytes == 3){
            temp[tempcount]=buffer[0];
            temp[tempcount+1]=buffer[1];
            temp[tempcount+2]=buffer[2];
            total_bytes=0;
            count++;
            tempcount+=3;
        }

        if (total_bytes > 3){
            
            lseek(fd, -read_bytes , SEEK_CUR);
            total_bytes=0;

        }
        
        if(count == 10){
            printf("Created R_%d with ",child_process_number+1);
            for(int i=0 ; i< 10;i=i+3){
                printf("(%d , %d , %d ),",temp[i],temp[i+1],temp[i+2]);
            }
            printf("\n");
            count=0;

            tempcount=0;

            char* argv_l[] = {
                argv[4], NULL
            };

            char *envp[] =
            {
                temp,
                0
            };
            
            __pid_t pid = fork();
            

            if(pid != 0){
                child_process_id[child_process_number]=pid;
                child_process_number++;
                child_process_id = (int*) realloc(child_process_id, (child_process_number + 1) * sizeof(int));

                if(child_process_id == NULL){
                    perror("Memory allocation error ! \n");
                    return 0;
                }
            }

            if(pid==0){
                
                execve("./ch", argv_l, envp);
                perror("Exevce Error ! \n");
                exit(EXIT_SUCCESS);
            }
            
        }
        
        read_bytes=0;
        
    }
    
    int fd3 = close(fd);

    if(fd3 == -1){

        perror("Failed to Close Output File !! \n");

        return 0;
    }
    if(child_process_number == 0) return 0;

    if(sigprocmask(SIG_SETMASK, &prevMask ,NULL) == -1){
        perror("Critical Section End! \n");
    }

    check();

    int child_pid;
    for(;;){
        child_pid =wait(NULL);
        if(child_pid == -1){
            if(errno == ECHILD){
                printf("No more children bye.\n");
                break;
            }
            else{
                perror("Wait to kill children ! \n");
            }
        }

    }

    


    if((fd4 = open(argv[4],READ_FLAGS,mode)) == -1){

        perror("Failed To Open Input File !\n");

        return 0;

    } 
    
    int index = 0 ,counter=0;
    double a;
    char buf[1];
    buff = (char*)calloc(20,sizeof(char));

    if(buff == NULL){
        free(child_process_id);
        perror("Memory allocation error ! \n");
        return 0;
    }

    check();
    
    matrices = (double **) malloc((counter1 + 1) * sizeof(double*));

    if(matrices == NULL){
        free(child_process_id);
        free(buff);
        perror("Memory allocation error ! \n");
        return 0;
    }

    if(sigprocmask(SIG_BLOCK, &intMask , &prevMask) == -1){
        perror("Critical Section Started No Interrupted ! \n");
    }

    matrices[counter1] = (double *)malloc(9*sizeof(double));

    if(matrices[counter1] == NULL){
        free(child_process_id);
        free(buff);
        perror("Memory allocation error ! \n");
        free(matrices);
        return 0;
    }

    for( ; ; ) {

        while (((read_bytes = read(fd4, buf,1)) == -1) 
            && (errno == EINTR)){
            continue;
        }

        if (read_bytes <= 0) break;
        if(buf=='\0') break;
 
        if(*buf=='*'){
            sscanf(buff,"%lf",&a);
            matrices[counter1][counter]=a;
            counter++;
            index = 0;
            continue;
        }

        if(*buf=='\n'){
            counter1++;
            matrices = realloc(matrices, (counter1 + 1) * sizeof(double*));
            if (matrices == NULL){
                return 0;
            }
            matrices[counter1] = (double *)malloc(9*sizeof(double));

            if (matrices[counter1] == NULL){
                free(matrices);
                return 0;
            }
                
            counter = 0;

           
            continue;
        }
        
        buff[index]=*buf;
        index++;
    }
   
    if(sigprocmask(SIG_SETMASK, &prevMask ,NULL) == -1){
        perror("Critical Section End! \n");
    }

    int fd6 = close(fd4);

    if(fd6 == -1){

        perror("Failed to Close Output File !! \n");

        return 0;
    }

    printf("Reached EOF, collecting outputs from %s \n",file_name);

    check();

    resultMatrice = (double *)malloc(counter1*sizeof(double));
     
     if(resultMatrice == NULL){
        free(child_process_id);
        free(buff);
        free(matrices);
        perror("Memory allocation error ! \n");
        return 0;
    }

    for(int k = 0; k<counter1; k++){
        resultMatrice[k] =frobeniusNorm(matrices[k]);
    }

    double diff = INT_MAX;
    int min_matrix_1 , min_matrix_2;

    for (int i=0; i<counter1-1; i++){
        for (int j=i+1; j<counter1; j++){
            if (fabs(resultMatrice[i] - resultMatrice[j]) < diff){
                diff = fabs(resultMatrice[i] - resultMatrice[j]);
                min_matrix_1 = i;
                min_matrix_2 = j;
            }
        }
    }

    check();
    
    printf("\n");
    printf("The Closest Matrices Are These\n \n");
    printf("================================\n");

    for(int m = 0; m < 9 ; m++){
        
        if(m%3 == 0 && m!=0){
            printf("\n");
        }
        printf("%lf ",matrices[min_matrix_1][m]);
        
    }
    printf("\n\n\n");
    printf("================================\n");

    for(int m = 0; m < 9 ; m++){
        if(m%3 == 0 && m!=0){
            printf("\n");
        }
        printf("%lf ",matrices[min_matrix_2][m]);
        
    }

    printf("\n");
    printf("================================\n");

    printf("\nTheir Distance Is : %-3.5lf\n",diff);


    for(int k = 0;k < counter1+1 ; k++){
        free(matrices[k]);
    }

    check();

    free(child_process_id);
    free(matrices);
    free(buff);
    free(resultMatrice);
    
    return 0 ;
}


double frobeniusNorm(double* metrice)
{
 
    double sumSq = 0;
    for (int i = 0; i < 9; i++) {
        
        sumSq += pow(metrice[i], 2);
        
    }
 
    double res = sqrt(sumSq);
    return res;
}


void handle_sigint(int sig){

    printf("All Resources Are Free , Files Was Closed , Output.txt Is Removed !!!  \n");
    chech_sigint = 1;
    
}

void check(){

    if(chech_sigint== 1){

        close(fd);
        close(fd4);

        for(int i =0; i<child_process_number;i++){
            
            if(kill(child_process_id[i], SIGINT) == -1 && errno != ESRCH){
                
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid =fork();

        if(pid==0){

            char *argv[] = { "rm", file_name, NULL};
            
            execv("/bin/rm",argv);

        }

        int child_pid;

        for(;;){

            child_pid =wait(NULL);
            if(child_pid == -1){
                if(errno == ECHILD){
                    perror("No more children bye.\n");
                    break;
                }
                else{
                    perror("Wait to kill children ! \n");
                }
            }

        }

        for(int k = 0;k < counter1 ; k++){
            if(matrices[k]!=NULL)
                free(matrices[k]);
        }
        if(child_process_id!=NULL){
             free(child_process_id);
        }
        if(matrices!=NULL){
            free(matrices);
        }
        if(buff!=NULL)
            free(buff);
        if(resultMatrice!=NULL)
            free(resultMatrice);
        fflush(stdout);
        exit(1);
    }
    
}
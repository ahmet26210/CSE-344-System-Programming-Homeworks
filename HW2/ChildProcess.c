#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include<dirent.h>
#include<fcntl.h>
#include<signal.h>

int mean(int* arr);
char* int_to_char(double val);
void handle_sigint1(int sig);
char** Coveriance_Matrix(int values[10][3]);
void check_SIGINT();
sig_atomic_t check_sigint=0;
char** result;
int fp;
int main(int argc, char* argv[] ,char* envp[]){

    struct sigaction sa1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;        
    sa1.sa_handler = &handle_sigint1;
    sigaction(SIGINT,&sa1,NULL);

    mode_t mode = S_IRUSR | S_IWUSR;

    int byteswritten = 0;
    struct flock lock;
    int values[10][3];

    int j=0;
    for(int i=0;i<10;i++){
        values[i][0]=envp[0][j]-'0';
        values[i][1]=envp[0][j+1]-'0';
        values[i][2]=envp[0][j+2]-'0';
        j=j+3;
    }

    check_SIGINT();
    
    if((fp= open(argv[0],O_WRONLY | O_APPEND | O_CREAT , mode)) == -1){

        perror("Failed To Open Input File !\n");

        return 0;

    }
   
    
    result = Coveriance_Matrix(values);

    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_WRLCK;

    fcntl(fp,F_SETLKW,&lock);
    check_SIGINT();
    
    while(*result !=NULL){

        while(((byteswritten = write(fp,*result,strlen(*result))) == -1 ) && (errno ==EINTR));
        byteswritten = write(fp,"*",sizeof(char));
        result++;

        if(byteswritten < 0){
            for(int k = 0;k<10;k++){
                free(result[k]);
            }

            free(result);
            perror("Error while writing the file !! \n ");

            return 0;
        }
    }
    check_SIGINT();
    byteswritten = write(fp,"\n",sizeof(char));  
    if(byteswritten < 0 ){ perror("Error while writing the file !! \n "); return 0;}

    lock.l_type =F_UNLCK;

    fcntl(fp,F_SETLKW,&lock);

    int fd1 = close(fp);

    if(fd1==-1){

        perror("Failed to Close Input File !! \n");

        return 0;
    }
    
    check_SIGINT();
    
    for(int k = 0;k<10;k++){
        free(result[k]);
    }

    free(result);
    return 0;
}


char** Coveriance_Matrix(int values[10][3]){

    char** MATRIX;

    MATRIX = (char**)calloc(10,sizeof(char*));

    if(MATRIX == NULL){
        perror("Memory allocation error ! \n");
        return 0;
    }

    double mean_x = 0 , mean_y = 0, mean_z = 0;
    double sum_x = 0 , sum_y =0 , sum_z = 0;
    double var_x = 0 , var_y =0 , var_z = 0;
    double cov_xy = 0 , cov_xz =0 , cov_yz = 0;

    for(int i=0;i< 10 ;i++){    
        sum_x += values[i][0];
        sum_y += values[i][1];
        sum_z += values[i][2];
    }
       
    mean_x = sum_x / 10 ;
    mean_y = sum_y / 10 ;
    mean_z = sum_z / 10 ;

    for(int i=0;i< 10 ;i++){    
        var_x += (values[i][0] - mean_x)*(values[i][0] - mean_x);
        var_y += (values[i][1] - mean_y)*(values[i][1] - mean_y);
        var_z += (values[i][2] - mean_z)*(values[i][2] - mean_z);
    }

    var_x = var_x / 10;
    var_y = var_y / 10;
    var_z = var_z / 10;

    for(int i=0;i< 10 ;i++){

        cov_xy += (values[i][0]-mean_x)*(values[i][1] - mean_y);
        cov_xz += (values[i][0]-mean_x)*(values[i][2] - mean_z);
        cov_yz += (values[i][1]-mean_y)*(values[i][2] - mean_z);

    }

    cov_xy = cov_xy / 10;
    cov_xz = cov_xz / 10;
    cov_yz = cov_yz / 10;

    MATRIX[0] = int_to_char(var_x);
    MATRIX[1] = int_to_char(cov_xy);
    MATRIX[2] = int_to_char(cov_xz);

    MATRIX[3] = int_to_char(cov_xy);
    MATRIX[4] = int_to_char(var_y);
    MATRIX[5] = int_to_char(cov_yz);

    MATRIX[6] = int_to_char(cov_xz);
    MATRIX[7] = int_to_char(cov_yz);
    MATRIX[8] = int_to_char(var_z);
    MATRIX[9] = NULL;
    
    return MATRIX;
    
}

char* int_to_char(double val){

    char* str = (char*) calloc(sizeof(char),15);
    if(str == NULL){
        perror("Memory allocation error ! \n");
        return NULL;
    }
    sprintf(str, "%lf", val);
    return str;
}

void handle_sigint1(int sig){
    
    check_sigint =1;
    
  
}
void check_SIGINT(){
    if(check_sigint == 1){
        for(int k = 0;k<10;k++){
            if(result[k]!=NULL)
                free(result[k]);
        }
        if(result!=NULL)
            free(result);
        close(fp);
        exit(1);
    }
}
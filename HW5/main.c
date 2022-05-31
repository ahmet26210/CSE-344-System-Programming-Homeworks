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
#include<sys/time.h>
#include<math.h>
#include <time.h>


int fp,fp1,fp2;
struct flock lock;
char* input_file_1;
char* input_file_2;
char* output_file;
int n_number,m_number;
int arrived=0;
int arrived1=0;

char tempchar[200];

int byteswritten;
int** matrixA;
int** matrixB;
int** matrixC;
int size;
int** a;

clock_t start_t1, end_t1;
double total_t1;

pthread_t *threads;
pthread_mutex_t mutex;
pthread_cond_t cv;

typedef struct {
    double** Xr;
    double** Xi;
    int start_index;
    int end_index;
}resultMatrices;

resultMatrices* result_matrix;

int** read_from_file(char* file_name,int** matrix);
void *mult(void* arg);
void handle_sigint(int sig);
int main( int argc, char * argv [] ){

    if(argc!=11){
        perror("Invalid Input Please Enter Correctly ex: \n (./hw5 -i filePath1 -j filePath2 -o output -n 4 -m 2 \n");
        return 0;
    }

    input_file_1 = argv[2];

    input_file_2 = argv[4];

    output_file = argv[6];

    n_number = atoi(argv[8]);

    m_number = atoi(argv[10]);

    if(n_number < 3 ){
        perror("Invaid n number , try again! \n");
        return 0;
    }

    if(m_number < 2){
        perror("Invaid m number , try again! \n");
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

    mode_t mode = S_IRUSR | S_IWUSR;

    if((fp1 = open(output_file,O_WRONLY | O_CREAT | O_TRUNC ,mode)) == -1){

        perror("Failed To Open Output File !\n");

        exit(1);

    }


    int** temp_matrixA=NULL;
    int** temp_matrixB=NULL;

    start_t1 = clock();

    matrixA = read_from_file(input_file_1,temp_matrixA);
    matrixB = read_from_file(input_file_2,temp_matrixB);

    int total_bytes = 1;


    for (int i = 0; i < n_number; i++)
    {
      total_bytes*=2;
    }

    time_t seconds;
    seconds = time(NULL);
    snprintf(tempchar,200,"Seconds: %ld", seconds);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    snprintf(tempchar, 200, " Two matrices of size %dx%d have been read. The number of threads is %d .\n"
            ,total_bytes,total_bytes,m_number);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    matrixC=(int **) malloc(size * sizeof(int*));

    if(matrixC == NULL){

        perror("Memory allocation error ! \n");
        exit(1);
    }

    for (int i = 0; i < size; i++)
    {
        matrixC[i] = (int *) malloc(size *sizeof(int));

        if(matrixC[i] == NULL){

            perror("Memory allocation error ! \n");
            free(matrixC);
            exit(1);
        }

    }

    threads = (pthread_t*)malloc(m_number * sizeof(pthread_t));

    // if(total_bytes%m_number!=0){
    //     perror("invalid m number");
    //     return -1;
    // }

    int column_number = total_bytes / m_number;


    a= (int **) malloc(m_number * sizeof(int*));

    for (int i = 0; i < m_number; i++)
    {
        a[i] = (int *) malloc(2 *sizeof(int));

        if(a[i] == NULL){

            perror("Memory allocation error ! \n");
            free(a);
            exit(1);
        }

    }

    // if(m_number>total_bytes){
    //     perror("Thread number dont exceed the size of matrix\n");
    //     return -1;
    // }

    int temp=0;
    for (int i = 0; i < m_number; i++)
    {
        if(i+1==m_number && total_bytes-temp != column_number){
            a[i][0]=temp;
            //temp+=column_number;
            a[i][1]=total_bytes;
            a[i][2]=i;
            break;
        }
        a[i][0]=temp;
        temp+=column_number;
        a[i][1]=temp;
        a[i][2]=i;
    }

    result_matrix = (resultMatrices*)malloc(sizeof(resultMatrices));

    result_matrix->Xr= (double **) malloc(size * sizeof(double*));

    for (int i = 0; i < size; i++)
    {
        result_matrix->Xr[i] = (double *) malloc(size *sizeof(double));

        if(result_matrix->Xr[i] == NULL){

            perror("Memory allocation error ! \n");
            free(result_matrix->Xr);
            exit(1);
        }

    }

    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            result_matrix->Xr[i][j]=0;
        }

    }


    result_matrix->Xi= (double **) malloc(size * sizeof(double*));

    for (int i = 0; i < size; i++)
    {
        result_matrix->Xi[i] = (double *) malloc(size *sizeof(double));

        if(result_matrix->Xi[i] == NULL){

            perror("Memory allocation error ! \n");
            free(result_matrix->Xi);
            exit(1);
        }

    }


    pthread_mutex_init (&mutex, NULL);
    pthread_cond_init(&cv, NULL);

    for (int i = 0; i < m_number; i++) {

        if (pthread_create(&threads[i], NULL, &mult,(void*)a[i]) != 0) {
            perror("Failed to create thread");
        }
    }

    for (int i = 0; i < m_number; i++) {

        if (pthread_join(threads[i],NULL) != 0) {
            perror("Failed to join thread");
        }

    }

    for (size_t i = 0; i < size; i++)
    {

        for (size_t j = 0; j < size; j++)
        {
            snprintf(tempchar, 200, "(%lf) + j(%lf) , ",result_matrix->Xr[i][j],result_matrix->Xi[i][j]);
            while(((byteswritten = write(fp1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

        }
        snprintf(tempchar, 200, "\n");
        while(((byteswritten = write(fp1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    }

    end_t1 = clock();
    total_t1 = (double)(end_t1 - start_t1) / CLOCKS_PER_SEC;
    seconds = time(NULL);
    snprintf(tempchar,200,"Seconds: %ld", seconds);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));
    snprintf(tempchar, 200, " The process has written the output file. The total time spent is %lf seconds. \n"
            ,total_t1);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));



    close(fp1);

    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&mutex);


    for (size_t i = 0; i < size; i++)
    {
        free(matrixA[i]);
        free(matrixB[i]);
        free(matrixC[i]);
        free(result_matrix->Xr[i]);
        free(result_matrix->Xi[i]);
    }
    for (size_t i = 0; i < m_number; i++)
    {
        free(a[i]);
    }

    free(matrixA);
    free(matrixB);
    free(matrixC);
    free(a);
    free(threads);
    free(result_matrix->Xr);
    free(result_matrix->Xi);
    free(result_matrix);
}

void *mult(void* arg)
{

    clock_t start_t, end_t;
    double total_t;
    start_t = clock();

    int *val_p = (int *) arg;


    for (int i = val_p[0]; i < val_p[1]; i++){

        for (int j = 0; j < size; j++){
            for (int k = 0; k < size; k++){

                matrixC[j][i] += matrixA[j][k] * matrixB[k][i];

            }
        }
    }

    pthread_mutex_lock(&mutex);
    arrived++;

    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    time_t seconds;
    seconds = time(NULL);
    snprintf(tempchar,200,"Seconds: %ld", seconds);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    snprintf(tempchar, 200, " Thread %d has reached the rendezvous point in %lf seconds. \n"
            ,val_p[2]+1,total_t);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    if(arrived<m_number){

        pthread_cond_wait(&cv, &mutex);
    }
    else{
        pthread_cond_broadcast(&cv);
    }
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);
    seconds = time(NULL);
    snprintf(tempchar,200,"Seconds: %ld", seconds);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    snprintf(tempchar, 200, " Thread %d is advancing to the second part. \n"
            ,val_p[2]+1);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));
    pthread_mutex_unlock(&mutex);

    start_t = clock();

    for (int k = 0; k < size; k++) {

        for (int l = val_p[0]; l < val_p[1]; l++) {
            
            double real=0,imag=0;
            for (int m = 0; m < size; m++) {

                for (int n = 0; n < size; n++)
                {

                  real += matrixC[m][n] * (cos(2 * M_PI * k * m / size)*cos(2 * M_PI * l * n / size) - sin(2 * M_PI * k * m / size)*sin(2 * M_PI * l * n / size));
                  imag += matrixC[m][n] * -(cos(2 * M_PI * k * m / size)*sin(2 * M_PI * l * n / size) + sin(2 * M_PI * k * m / size)*cos(2 * M_PI * l * n / size));


                }
            }
            result_matrix->Xr[k][l] = real;
            result_matrix->Xi[k][l] = imag;
        }
    }
    
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    pthread_mutex_lock(&mutex);
    seconds = time(NULL);
    snprintf(tempchar,200,"Seconds: %ld", seconds);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));

    snprintf(tempchar, 200, " Thread %d has has finished the second part in %lf seconds. \n"
            ,val_p[2]+1,total_t);
    while(((byteswritten = write(1,&tempchar,strlen(tempchar))) == -1 ) && (errno ==EINTR));
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int** read_from_file(char* file_name,int** matrix){

    int total_bytes = 1;


    for (int i = 0; i < n_number; i++)
    {
      total_bytes*=2;
    }

    size=total_bytes;

    total_bytes=total_bytes*total_bytes;

    int counter=0;

    char buf[1];

    int read_bytes=0;

    matrix=(int **) malloc(size * sizeof(int*));

    if(matrix == NULL){

        perror("Memory allocation error ! \n");
        exit(1);
    }

    for (int i = 0; i < size; i++)
    {
        matrix[i] = (int *) malloc(size *sizeof(int));

        if(matrix[i] == NULL){

            perror("Memory allocation error ! \n");
            free(matrix);
            exit(1);
        }

    }

    int index1=0,index2=0;

    mode_t mode = S_IRUSR | S_IWUSR;

    if((fp = open(file_name,O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");
        for (size_t i = 0; i < size; i++)
        {
            free(matrix[i]);
        }
        free(matrix);
        exit(1);

    }

    for( ; ; ) {


        while (((read_bytes = read(fp, buf,1)) == -1)
            && (errno == EINTR)){
            continue;
        }
   

        if (read_bytes <= 0){
            //counter--;
            //printf("--%d--,%d\n",counter,total_bytes);
            if(counter<total_bytes){
                perror("Fatal Error. Exiting...\n");
                exit(1);
            }
            break;
        }
        if(index1!=size){
            matrix[index1][index2++]=*buf;
        }
        //else{continue;}
        // if(*buf=='\n'){

        //     continue;
        // }


        //index2++;
        counter++;

        if(index2==size){

            index1++;
            index2=0;

        }

    }

    close(fp);

    return matrix;
}

void handle_sigint(int sig){

    printf("EXITING GRACEFULLY \n");

    for (int i = 0; i < m_number; i++) {

        if (pthread_join(threads[i],NULL) != 0) {
            perror("Failed to join thread");
        }

    }

    close(fp);

    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&mutex);

    for (size_t i = 0; i < size; i++)
    {
        free(matrixA[i]);
        free(matrixB[i]);
        free(matrixC[i]);
        free(result_matrix->Xr[i]);
        free(result_matrix->Xi[i]);
    }
    for (size_t i = 0; i < m_number; i++)
    {
        free(a[i]);
    }

    close(fp1);

    free(matrixA);
    free(matrixB);
    free(matrixC);
    free(a);
    free(threads);
    free(result_matrix->Xr);
    free(result_matrix->Xi);
    free(result_matrix);
    pthread_exit(0);

    exit(1);

}
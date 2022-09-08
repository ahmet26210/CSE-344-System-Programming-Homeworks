#include"client.h"

int main(int argc , char* argv[]){

    if(argc != 7){
        perror("Invalid input Try again ! ex: ./client -r requestFile -q PORT -s IP \n");
        exit(1);
    }

    int  port_no;

    char* filename = argv[2];
    char* ip_addres = argv[6];
    port_no = atoi(argv[4]);

    read_from_file(filename);

    threads = (pthread_t*)malloc(counter1 * sizeof(pthread_t));

    pthread_mutex_init (&mutex, NULL);
    pthread_cond_init(&cv, NULL);

    int tempsockfd;
    tempsockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(tempsockfd == -1){
        perror("socket");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);

    inet_pton(AF_INET, ip_addres, &serv_addr.sin_addr);

   
    for (size_t i = 0; i < counter1; i++)
    {
        request1[i].id=i;
    }

    printf("Client: I have loaded %d requests and I'm creating %d threads.\n",counter1,counter1);
    fflush(stdout);

    for (size_t i = 0; i <counter1; i++)
    {
        if (pthread_create(&threads[i], NULL, &requests_handle,(void*)&request1[i]) != 0) {
            perror("Failed to create thread");
        }
    }
    
    for (int i = 0; i < counter1; i++) {

        if (pthread_join(threads[i],NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    
    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&mutex);

    printf("Client: All threads have terminated, goodbye.\n");
    fflush(stdout);
    close(tempsockfd);
    return 0;
}


void read_from_file(char* file_name){

    char buf[1];

    int read_bytes=0;

    request1[0].count = 0;

    mode_t mode = S_IRUSR | S_IWUSR;

    if((fp = open(file_name,O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");
        exit(1);

    }

    char buffer[200];
    int index_buffer=0;
    memset(buffer,0,200);
    for ( ; ;)
    {
        while (((read_bytes = read(fp, buf,1)) == -1)
            && (errno == EINTR)){
            continue;
        }
   
        if (read_bytes <= 0){
            break;
        }

        if(*buf=='\n'){

            int index1=0;
            char* start1 = strtok(buffer," ");

            while (start1 != NULL) {
                strcpy(request1[counter1].requests[index1++],start1);
               
                start1 = strtok(NULL, " ");
            }
            request1[counter1].count=index1;
            index_buffer=0;
            counter1++;
            printf("%d",index1);
            memset(buffer,0,200);
            continue;
        }
        
        buffer[index_buffer++]=*buf;
    }

    close(fp);

}   

void *requests_handle(void *arg)
{
    resultrequest *val_p = (resultrequest*) arg;

    int n=0;
    char buffer[50];

    printf("Client-Thread-%d: Thread-%d has been created \n",val_p->id,val_p->id);
    fflush(stdout);

    if(val_p->count==3){
        printf("Client-Thread-%d: I am requesting “%s %s %s %s” \n",val_p->id,val_p->requests[0],val_p->requests[1],val_p->requests[2],val_p->requests[3]);
        fflush(stdout);
    }
    else if(val_p->count==4){
        printf("Client-Thread-%d: I am requesting “%s %s %s %s %s” \n",val_p->id,val_p->requests[0],val_p->requests[1],val_p->requests[2],val_p->requests[3],val_p->requests[4]);
        fflush(stdout);
    }
    
    int sockfd;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if(sockfd < 0){
        perror("error opening socket\n");
        exit(1); 
    }
 
    pthread_mutex_lock(&mutex);
    arrived++;

    if(arrived<counter1){

        pthread_cond_wait(&cv, &mutex);
    }
    else{
        pthread_cond_broadcast(&cv);
    }

    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);

    while(connect(sockfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) == -1){
       close(sockfd);
       sockfd = socket(AF_INET , SOCK_STREAM , 0);
    }

    pthread_mutex_unlock(&mutex);

    int result;
    bzero(buffer,50);
    strcpy(buffer,"client");
    n=write(sockfd , buffer,50);
    if(n < 0){
        perror("error on writing");
        exit(1);
    }   
    n=write(sockfd , val_p,sizeof(resultrequest));
    if(n < 0){
        perror("error on writing");
        exit(1);
    }   
    n = read(sockfd , &result , sizeof(int));
    if(n < 0){
        perror("error on writing");
        exit(1);
    }
    if(val_p->count==4){
        if(result==-1){
            printf("ERROR THERE IS NO SUCH A CITY\n");
            fflush(stdout);     
        }
        else{
            printf("Client-Thread-%d: The server's response to “%s %s %s %s” is %d \n",val_p->id,val_p->requests[0],val_p->requests[1],val_p->requests[2],val_p->requests[3],result);
            fflush(stdout);     
        }
    }
    else if(val_p->count==5){
        if(result==-1){
            printf("ERROR THERE IS NO SUCH A CITY\n");
            fflush(stdout);     
        }
        else{
            printf("Client-Thread-%d: The server's response to “%s %s %s %s %s” is %d \n",val_p->id,val_p->requests[0],val_p->requests[1],val_p->requests[2],val_p->requests[3],val_p->requests[4],result);
            fflush(stdout);     
        }
    }    
    
   
    close(sockfd);
    printf("Client-Thread-%d: Terminating\n",val_p->id);
    fflush(stdout);
    return NULL;
}
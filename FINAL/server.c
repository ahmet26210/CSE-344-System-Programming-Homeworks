#include"server.h"

int main(int argc , char* argv[]){

    if(argc != 5){
        perror("Invalid input Try again ! ex: ./server -p PORT -t numberOfThreads\n");
        exit(1);
    }

    struct sigaction sa2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sa2.sa_handler = &handle_sigint;

    if(sigaction(SIGINT,&sa2,NULL) == - 1){

        perror("error");
        return 0;
    }
   
    int port_no;
    
    struct sockaddr_in serv_addr , cli_addr;

    socklen_t clilen;

    port_no = atoi(argv[2]);
    int number_of_threads=atoi(argv[4]);

    if(number_of_threads < 5){
        perror("Thread number must be greater than 5 ! \n");
        return -1;
    }

    pthread_mutex_init (&mutex, NULL);
    pthread_cond_init(&cv, NULL);

    threads = (pthread_t*)malloc(number_of_threads * sizeof(pthread_t));

    for (size_t i = 0; i <number_of_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, &requests_handle,NULL) != 0) {
            perror("Failed to create thread");
        }
    }

    bzero((char*)&serv_addr , sizeof(serv_addr));

    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if(sockfd < 0){
        perror("error opening socket\n");
        exit(1); 
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_no);

    if(bind(sockfd , (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        perror("error binding failed.\n");
        exit(1);
    }
    
    listen(sockfd , 12);
    clilen = sizeof(cli_addr);

   
    for ( ; ; )
    {

        newsockfd = accept(sockfd , (struct sockaddr*) &cli_addr,&clilen);

        if(newsockfd == -1 && sig_int_check==0){
            perror("error");
            exit(0);
        }

        if(sig_int_check==1){
            terminate_function();
            exit(0);
        }
         
        pthread_mutex_lock(&mutex);
        if(isFull()!=1){
            insert(newsockfd);
            pthread_cond_signal(&cv);
        }
        pthread_mutex_unlock(&mutex);
    }

     
    return 0; 
}

void *requests_handle(void* arg)
{
    char buffer[50];
    time_t curtime;
    bzero(buffer,50);
    resultrequest client_request;
    int newsockfd;
    servant_data temp1;
    for ( ; ; )
    {
        pthread_mutex_lock(&mutex);
        
        if(isEmpty()==1)      
            pthread_cond_wait(&cv, &mutex);
       
        pthread_mutex_unlock(&mutex);
        
        if(isEmpty()!=1){
            pthread_mutex_lock(&mutex);
            newsockfd=peek();
            removeData();
            pthread_mutex_unlock(&mutex);
            int n = read(newsockfd,buffer,50);
            if(n < 0){
                perror("error on writing");
                exit(1);
            } 
            if(strcmp("client",buffer)== 0){
                int n = read(newsockfd , &client_request , sizeof(resultrequest));
                total_request++;
                if(n < 0){
                    perror("error on writing");
                    exit(1);
                } 
                if(client_request.count==4){
                    

                    time(&curtime);
                    printf("Current time = %s ", ctime(&curtime));

                    printf("Request arrived “%s %s %s %s”\n",client_request.requests[0],client_request.requests[1],client_request.requests[2],client_request.requests[3]);
                    fflush(stdout);
                    int sum=0;
                    for (size_t i = 0; i < dir_index; i++)
                    {
                        int result;
                        struct sockaddr_in serv_addr_temp;
                        int tempsockfd;
                        int port_n =datas[i].port_num;

                        char ip_num[29];
                        strcpy(ip_num,datas[i].ip_num);

                        time(&curtime);
                        printf("Current time = %s ", ctime(&curtime));
                        printf("Contacting ALL Servants\n");
                        fflush(stdout);

                        
                        tempsockfd = socket(AF_INET, SOCK_STREAM, 0);
                        if(tempsockfd == -1){
                            perror("socket");
                        }

                        bzero(&serv_addr_temp, sizeof(serv_addr_temp));
                        serv_addr_temp.sin_family = AF_INET;
                        serv_addr_temp.sin_port = htons(port_n);

                        inet_pton(AF_INET, ip_num, &serv_addr_temp.sin_addr);
                        
                        pthread_mutex_lock(&mutex);

                        while(connect(tempsockfd , (struct sockaddr *)&serv_addr_temp , sizeof(serv_addr_temp)) == -1){
                            close(tempsockfd);
                            tempsockfd = socket(AF_INET , SOCK_STREAM , 0);
                        }
                        pthread_mutex_unlock(&mutex);

                        n=write(tempsockfd , &client_request,sizeof(resultrequest));


                        n = read(tempsockfd , &result , sizeof(int));
                        sum+=result;

                        close(tempsockfd);
                    }

                    time(&curtime);
                    printf("Current time = %s ", ctime(&curtime));
                    printf("Response received: %d, forwarded to client\n",sum);
                    fflush(stdout);

                    n=write(newsockfd , &sum,sizeof(int));

                
                }
                else if(client_request.count==5){
                    time(&curtime);
                    printf("Current time = %s ", ctime(&curtime));
                    printf("Request arrived “%s %s %s %s %s”\n",client_request.requests[0],client_request.requests[1],client_request.requests[2],client_request.requests[3],client_request.requests[4]);
                    fflush(stdout);

                    int port_n;
                    char ip_n[50];
                    int check=0;
                    int result;


                    for (size_t m = 0; m < dir_index; m++)
                    {
                        for (size_t l = 0; l <datas[m].num_of_dir; l++)
                        {
                            if(strcmp(datas[m].cities[l],client_request.requests[4])==0){
                                port_n =datas[m].port_num;
                                strcpy(ip_n,datas[0].ip_num);
                                check = 1;
                                break;
                            }
                        }
                        if(check==1) break;
                       
                    }

                    if(check==1){
                        struct sockaddr_in serv_addr_temp;
                        int tempsockfd;

                        tempsockfd = socket(AF_INET, SOCK_STREAM, 0);
                        if(tempsockfd == -1){
                            perror("socket");
                        }

                        bzero(&serv_addr_temp, sizeof(serv_addr_temp));
                        serv_addr_temp.sin_family = AF_INET;
                        serv_addr_temp.sin_port = htons(port_n);

                        inet_pton(AF_INET, ip_n, &serv_addr_temp.sin_addr);

                        time(&curtime);
                        printf("Current time = %s ", ctime(&curtime));
                        printf("Contacting servant %d\n",port_n);
                        fflush(stdout);

                        
                        pthread_mutex_lock(&mutex);
                        while(connect(tempsockfd , (struct sockaddr *)&serv_addr_temp , sizeof(serv_addr_temp)) == -1){
                            close(tempsockfd);
                            tempsockfd = socket(AF_INET , SOCK_STREAM , 0);
                        }
                        pthread_mutex_unlock(&mutex);
                        n=write(tempsockfd , &client_request,sizeof(resultrequest));
                        
                        n = read(tempsockfd , &result , sizeof(int));

                        time(&curtime);
                        printf("Current time = %s ", ctime(&curtime));
                        printf("Response received: %d, forwarded to client\n",result);
                        fflush(stdout);

                        n=write(newsockfd , &result,sizeof(int));

                        close(tempsockfd);

                    }
                    else{
                        result = -1;
                        n=write(newsockfd , &result,sizeof(int));
                    }
                    
                }
                
            }
            else{
                int n = read(newsockfd , &temp1 , sizeof(servant_data));
                if(n < 0){
                    perror("error on writing");
                    exit(1);
                } 
                time(&curtime);
                printf("Current time = %s ", ctime(&curtime));
                printf("Servant %d present at port %d handling cities %s-%s\n",temp1.process_id,temp1.port_num,temp1.cities[0],temp1.cities[temp1.num_of_dir-1]);
                fflush(stdout);

                pthread_mutex_lock(&mutex);
                datas[dir_index++]=temp1;
                pthread_mutex_unlock(&mutex);
                
            }
           
            
        }
    }


    close(newsockfd);

    return NULL;
}

int peek() {
   return intArray[front];
}

int isEmpty() {
   return itemCount == 0;
}

int isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}  

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       

      intArray[++rear] = data;
      itemCount++;
   }
}

int removeData() {
   int data = intArray[front++];
	
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}
void handle_sigint(int sig){

    sig_int_check=1;
   
}
void terminate_function(){
    printf("EXITING GRACEFULLY \n");
    printf("SIGINT has been received. I handled a total of %d requests. Goodbye.\n",total_request);
    fflush(stdout);
    for(int i=0; i<dir_index; ++i){
        kill(datas[i].process_id, SIGINT);
    }
    
    // pthread_cond_broadcast(&cv);
    // for(int i=0; i<number_of_threads; ++i){
    //     pthread_join(threads[i], NULL);
    // }
    if(threads != NULL)
        free(threads);

    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&mutex);
    close(sockfd);
    close(newsockfd);
    exit(0);
   
}
void get_current_time(){

    int mili_sec;
    char str_time[100];
    struct timeval cur_t1;

    gettimeofday(&cur_t1, NULL);
    mili_sec = ( cur_t1.tv_usec / 1000 );

    strftime(str_time, 100, "%H:%M:%S", localtime(&cur_t1.tv_sec));
    sprintf(currenttime, "%s:%03d", str_time, mili_sec);
}
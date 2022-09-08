#include"servant.h"

int main(int argc , char* argv[]){
    if(argc != 9){
        perror("Invalid input Try again ! ex: ./servant -d directoryPath -c 10-19 -r IP -p PORT \n");
        exit(1);
    }

    char* directory_path = argv[2];
    char* numbers = argv[4];
    char* ip_address = argv[6];
    int port_number = atoi(argv[8]);

    int number1,number2;
    int temp_index=0;
    char temp[10] , temp1[10];
    for (size_t i = 0; i < 20; i++)
    {
        if(numbers[i]=='-'){
            strcpy(temp,temp1);
            number1=atoi(temp);
            memset(temp1,0,10);
            temp_index=0;
            continue;
        }
        else if(numbers[i]=='\0'){
            strcpy(temp,temp1);
            number2=atoi(temp);
            break;
        }
        temp1[temp_index++] = numbers[i];
    }

    struct sigaction sa2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sa2.sa_handler = &handle_sigint;

   
    char message[100];

    struct dirent **namelist,**namelist1;
    int i,n1,n2;
    
    n1 = scandir(directory_path, &namelist, 0, alphasort);
    if (n1 < 0)
        perror("scandir");
    else {
        for (i = number1+1; i <= number2+1; i++) {
            strcpy(message,directory_path);
            strcat(message,"/");
            strcat(message,namelist[i]->d_name);
            n2 = scandir(message, &namelist1, 0, alphasort);
            if (n2 < 0)
                perror("scandir");
            else {
                index2=0;
                for (int j = 2; j < n2; j++) {
                    strcpy(directories.dir[index1].file[index2++].file_name,namelist1[j]->d_name);
                    free(namelist1[j]);
                }
            }
           
            strcpy(directories.dir[index1++].dir_name,namelist[i]->d_name);
            
        }
            
    }
    
    int servant_id = my_get_id();
    
    directories.num_of_dir=number2-number1+1;
    directories.num_of_file=n2-2;

    datas.num_of_dir=number2-number1+1;
    datas.num_of_file=n2-2;
    datas.process_id=servant_id;

    char temp_buf[200];
    for (size_t i = 0; i < directories.num_of_dir; i++)
    {
        
        for (size_t j = 0; j < directories.num_of_file; j++)
        {
            strcpy(temp_buf,directory_path);
            strcat(temp_buf,"/");
            strcat(temp_buf,directories.dir[i].dir_name);
            strcat(temp_buf,"/");
            strcat(temp_buf,directories.dir[i].file[j].file_name);
            read_from_file(temp_buf,i,j);
        }
        strcpy(datas.cities[i],directories.dir[i].dir_name);
    }

    item = (struct DataItem*) malloc(directories.num_of_dir*sizeof(struct DataItem));

    for (size_t i = 0; i < directories.num_of_dir ; i++)
    {
        insert(directories.dir[i].dir_name,directories.dir[i],&item[i]);
    }

    printf("Servant %d: loaded dataset, cities %s-%s\n",servant_id,directories.dir[0].dir_name,directories.dir[directories.num_of_dir-1].dir_name);
    fflush(stdout);

    pthread_mutex_init (&mutex, NULL);
    pthread_cond_init(&cv, NULL);
   
    int n;
    char buffer1[50];
    socklen_t clilen;
    struct sockaddr_in serv_addr,serv_addr1 ,cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("socket");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);

    inet_pton(AF_INET, ip_address, &serv_addr.sin_addr);
    
    pthread_mutex_lock(&mutex);
    while(connect(sockfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) == -1){
       close(sockfd);
       sockfd = socket(AF_INET , SOCK_STREAM , 0);
    }
    pthread_mutex_unlock(&mutex);
    
    int newport= createPort();
    
    
    bzero(buffer1,50);
    strcpy(buffer1,"servant");
    n=write(sockfd , buffer1,50);
    datas.port_num = newport;
    strcpy(datas.ip_num,ip_address);

    n=write(sockfd , &datas,sizeof(servant_data));
    if(n < 0){
        perror("error on writing");
        exit(1);
    }    
    
    close(sockfd);


    bzero((char*)&serv_addr1 , sizeof(serv_addr1));

    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if(sockfd < 0){
        perror("error opening socket\n");
        exit(1); 
    }

    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_addr.s_addr = INADDR_ANY;
    serv_addr1.sin_port = htons(newport);

    if(bind(sockfd , (struct sockaddr *) &serv_addr1,sizeof(serv_addr1)) < 0){
        perror("error binding failed.\n");
        exit(1);
    }
    
    listen(sockfd , 5);
    clilen = sizeof(cli_addr);

    printf("Servant %d: listening at port %d\n",servant_id,newport);
    fflush(stdout);

    for ( ; ; )
    {
        int *i = malloc(sizeof(int));
        newsockfd = accept(sockfd , (struct sockaddr*) &cli_addr,&clilen);
        if(newsockfd == -1 && sig_int_check==0){
            perror("error");
            exit(0);
        }
        pthread_mutex_lock(&mutex);
        total_request++;
        pthread_mutex_unlock(&mutex);
        if(sig_int_check==1){
            terminate_function();
            printf("Servant %d: termination message received, handled %d requests in total\n",servant_id,total_request);
            exit(0);
        }
        *i = newsockfd;
        pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));
        pthread_mutex_lock(&mutex);
        threads1[thread_number++] =thread;
        pthread_mutex_unlock(&mutex);
        if (pthread_create(thread, NULL, &requests_handle,(void*)i) != 0) {
            perror("Failed to create thread");
        }
    }
   
    close(newsockfd);
    close(sockfd);


    for (size_t i = 0; i < n1; i++)
    {
        free(namelist[i]);
    }
   
    
    free(namelist);
    free(namelist1);
    free(item);

    return 0;
}

struct DataItem *search(char* key) {

   int hashIndex = 0;  
	
   while(hashIndex!=index1) {
	
      if(strcmp(item[hashIndex].key,key)==0)
        return &item[hashIndex]; 
			
      ++hashIndex;

   }        
	
   return NULL;        
}

void insert(char* key,Directories data,struct DataItem* item) {

   
   strcpy(item->key,key);
   item->data=data;
	
}

void display() {
   int i = 0;
	
   for(i = 0; i<10; i++) {
	
      if(&item[i] != NULL){
        for (size_t j = 0; j < 10; j++)
        {
            //printf(" (%s,%s)",item[i].key,item[i].data[0].file[0].req[0].transaction_id);
        }
        
        printf("\n\n\n\n\n");
      }
   }
	
   printf("\n");
}

void* requests_handle(void *arg)
{
    int tempsockfd = *((int *) arg);

    resultrequest temp3;

    int n4 = read(tempsockfd , &temp3 , sizeof(resultrequest));
    if(n4 < 0){
        perror("error on writing");
        exit(1);
    }
  
    char start[50];
    char end[50];
    char type[50];

    strcpy(type,temp3.requests[1]);
    strcpy(start,temp3.requests[2]);
    strcpy(end,temp3.requests[3]);
   
    int start_number[3];
    int end_number[3];
    int date_index=0;
    char* context = NULL;
    char* start1 = strtok_r(start,"-",&context);

    while (start1 != NULL) {
        start_number[date_index++]=atoi(start1);
        start1 = strtok_r(NULL, "-",&context);
    }

    date_index = 0;
    char* end1 = strtok_r(end,"-",&context);

    while (end1 != NULL) {
        end_number[date_index++]=atoi(end1);
        end1 = strtok_r(NULL, "-",&context);
    }

    
    int transaction_count=0;
    struct DataItem* as;

    pthread_mutex_lock(&mutex);
    if(temp3.count == 5){
        as = search(temp3.requests[4]);
    }
    pthread_mutex_unlock(&mutex);
 
    
    if(as!=NULL){
        for (size_t i = 0; i < directories.num_of_file; i++)
        {
            int check =0;
            int file_number[3];
            int date_index1=0;
            char* context1 = NULL;
            char file_p[100];
            for (size_t l = 0; l <strlen(as->data.file[i].file_name); l++)
            {
                file_p[l]=as->data.file[i].file_name[l];
            }
            
            char* file_pars = strtok_r(file_p,"-",&context1);

            while (file_pars != NULL) {
                file_number[date_index1++]=atoi(file_pars);
                file_pars = strtok_r(NULL, "-",&context1);
            }
           
            check = check_function(file_number,start_number,end_number);

            if(check==1){
                for (size_t j = 0; j < directories.num_of_req; j++)
                {
                    if(strcmp(as->data.file[i].req[j].type,type)==0){
                        transaction_count++;
                    }
                }
                
            }
        
        }
    }
    else{
        
        for (size_t i = 0; i < directories.num_of_dir; i++)
        {
            for (size_t j = 0; j < directories.num_of_file; j++)
            {
                
                int check =0;
                int file_number[3];
                int date_index1=0;
                char* context1 = NULL;
                char file_p[100];
                for (size_t l = 0; l <strlen(directories.dir[i].file[j].file_name); l++)
                {
                    file_p[l]=directories.dir[i].file[j].file_name[l];
                }
                char* file_pars = strtok_r(file_p,"-",&context1);

                while (file_pars != NULL) {
                    file_number[date_index1++]=atoi(file_pars);
                    file_pars = strtok_r(NULL, "-",&context1);
                }
                
                check = check_function(file_number,start_number,end_number);

                if(check==1){
                
                    for (size_t k = 0; k < directories.num_of_req; k++)
                    {
                        
                        if(strcmp(directories.dir[i].file[j].req[k].type,type)==0){
                            transaction_count++;
                        }
                    }
                    
                }
               
            }                                                
       
            
        }
    }
    int n=write(tempsockfd , &transaction_count,sizeof(int));
    if(n < 0){
        perror("error on writing");
        exit(1);
    }
    free(arg);
    return NULL;
}


void read_from_file(char *file_name,int i,int j){

    char buf[1];

    int read_bytes=0;

    char buffer[100];
   
    mode_t mode = S_IRUSR | S_IWUSR;

    if((fp = open(file_name,O_RDONLY,mode)) == -1){
        perror("Failed To Open Input File !\n");
        exit(1);

    }
    int row_number=0;
    int index=0;
    int i1=0;
    for( ; ; ) {

        while (((read_bytes = read(fp, buf,1)) == -1)
            && (errno == EINTR)){
            continue;
        }

        if (read_bytes <= 0){
            break;
        }

        if(*buf==' '){
            if(i1==0) strcpy(directories.dir[i].file[j].req[row_number].transaction_id,buffer);
            else if(i1==1) strcpy(directories.dir[i].file[j].req[row_number].type,buffer);
            else if(i1==2) strcpy(directories.dir[i].file[j].req[row_number].name_of_street,buffer);
            else if(i1==3) strcpy(directories.dir[i].file[j].req[row_number].surface_meters,buffer);
            else if(i1==4) strcpy(directories.dir[i].file[j].req[row_number].price,buffer);

            memset(buffer,0,100);
            index=0;
            i1++;
            continue;
        }

        if(*buf=='\n'){
            if(i1==0) strcpy(directories.dir[i].file[j].req[row_number].transaction_id,buffer);
            else if(i1==1) strcpy(directories.dir[i].file[j].req[row_number].type,buffer);
            else if(i1==2) strcpy(directories.dir[i].file[j].req[row_number].name_of_street,buffer);
            else if(i1==3) strcpy(directories.dir[i].file[j].req[row_number].surface_meters,buffer);
            else if(i1==4) strcpy(directories.dir[i].file[j].req[row_number].price,buffer);
            memset(buffer,0,100);
            i1=0;           
            row_number++;
            index=0;
            continue;
        }

        buffer[index++]=*buf;
    }
    directories.num_of_req=row_number;
    datas.num_of_req=row_number;
    close(fp);
 
}
int createPort(){
    int sockfd;
    struct sockaddr_in serv_addr1;
    bzero((char*)&serv_addr1 , sizeof(serv_addr1));

    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if(sockfd < 0){
        perror("error opening socket\n");
        exit(1); 
    }

    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_addr.s_addr = INADDR_ANY;
    serv_addr1.sin_port = htons(0);

    if(bind(sockfd , (struct sockaddr *) &serv_addr1,sizeof(serv_addr1)) < 0){
        perror("error binding failed.\n");
        exit(1);
    }
    
    listen(sockfd , 5);

    socklen_t len = sizeof(serv_addr1);

    getsockname(sockfd,(struct sockaddr *) &serv_addr1 , &len);

    int myPort = ntohs(serv_addr1.sin_port);
    close(sockfd);
    return myPort;
}

void handle_sigint(int sig){


    sig_int_check=1;

}
void terminate_function(){

    printf("EXITING GRACEFULLY \n");
    
    //pthread_cond_broadcast(&cv);
    // for(int i=0; i<thread_number; ++i){
    //     pthread_join(threads1[i], NULL);
    // }

    if(item != NULL)
        free(item);

    for (size_t i = 0; i < thread_number; i++)
    {
        free(threads1[i]);
    }
    

    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&mutex);

    close(sockfd);
    close(newsockfd);
    close(fp);
    //pthread_exit(0);
    exit(0);
}
int my_get_id(){
    mode_t mode = S_IRUSR | S_IWUSR;

    char buf[20];
    char* token;
    int fd1; 
    if((fd1 = open("/proc/self/stat",O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");
        exit(1);

    }

    read(fd1,buf,sizeof(buf));
    token = strtok(buf," ");
    int pid=atoi(token);
    close(fd1);
    return pid;
}

int check_function(int file_number[3],int start_number[3],int end_number[3]){
    int check=0;
     if(file_number[2]>=start_number[2] && file_number[2]<=end_number[2]){
        if(file_number[2]>start_number[2] && file_number[2]<end_number[2]){
            check=1;
        }
        else{
            if(file_number[2]>start_number[2]){
                if(file_number[1]<end_number[1]){
                    
                    check=1;
                        
                }
                else if(file_number[1]==end_number[1]){
                    if(file_number[0]<=end_number[0]){
                        check=1;
                    }
                }
                    
                
            }
            else if(file_number[2]<end_number[2]){
                if(file_number[1]>start_number[1]){
                    
                    check=1;
                
                }
                else if(file_number[1]==end_number[1]){
                    if(file_number[0]>=start_number[0]){
                        check=1;
                    }
                }
                
            }
            else{
                if(file_number[1]>=start_number[1] && file_number[1]<=end_number[1]){
                    if(file_number[1]>start_number[1] && file_number[1]<end_number[1]){
                        check=1;
                    }
                    else{
                        if(file_number[1]>start_number[1]){
                            if(file_number[0]<=end_number[0]){
                                check=1;
                            }
                        }
                        else if(file_number[1]<end_number[1]){
                            if(file_number[0]>=start_number[0]){
                                check=1;
                            }
                        }
                        else{
                            
                            if(file_number[0]>start_number[0] && file_number[0]<end_number[0]){
                                check=1;

                            }
                        }
                    }
                        
                }

            }
        }
        
    }
    return check;
}
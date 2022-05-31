#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include"helper.h"
#include<dirent.h>
#include<fcntl.h>



int main( int argc, char * argv [] ) {
    
    if(argc!=3){
        perror("Invalid Input Please Enter File Name and Valid Parameter Ex: \n  (./hw1 '/str1/str2/' file.txt) \n"); // ' -> shift + 2
        return 0;
    }
    
    char** arr = InputPiecesSemiColon(argv[1],';');
    
    while(*arr!=NULL){
       
        char** arr1 = InputPiecesSemiColon(*arr,'/');
      
        functionTable(arr1 , argv[2]);
        
        arr++;
       
        free(arr1);
    }
   
    free(*arr);
    
    return 0;
}

char** InputPiecesSemiColon(char* input , char a){
  
    int count=0 , i =0;

    if(input == NULL ){
        return NULL;
    }
    
    char** arr;

    arr = (char **) malloc((count + 1) * sizeof(*arr));
    arr[count] = (char *)malloc(100*sizeof(char));
    
    if(arr==NULL){
        perror("Malloc Error\n");
        return NULL;
    }

    while(*input){
        
        if(*input==a){
            i=0;
            count++;
            arr = (char **) realloc(arr, (count + 1) * sizeof(*arr));

            if (arr == NULL)
                return NULL;
            arr[count] = (char *)malloc(100*sizeof(char));
            if (arr[count] == 0)
                return NULL;
            input++;
        }
        if(*input!=a){
            arr[count][i]=*input;
            i++;
        }
        input++;
        
    }
   
    return arr;
     
}
void functionTable(char** arr , char* path){
    int basicReplace=1,basicReplaceIncaseSensetive=0,BracketSituation=0,startReplace=0,EndReplace=0,starReplace=0; // 0 -> FALSE , 1 ->TRUE,

    char* str1;
    char* str2;
   
    if(*arr[3]=='i'){

        basicReplaceIncaseSensetive=1;
        basicReplace=0;

    }
    if(arr[1]!=NULL){

        str1=arr[1];
    }

    if(arr[2]!=NULL){
        str2=arr[2];
    }

    char* temp = str1; 

    while(*str1 !='\0' ){

        if(*str1=='^'){
            startReplace=1;
            basicReplace=0;
        }
        if(*str1=='*'){
            starReplace=1;
            basicReplace=0;
        }
        if(*str1=='$'){
            EndReplace=1;
            basicReplace=0;
        }
        if(*str1=='['){
            BracketSituation=1;
            basicReplace=0;
        }

        str1++;
    }

    StringArray(temp,str2,basicReplace,basicReplaceIncaseSensetive,BracketSituation,startReplace,EndReplace,starReplace,path);
    
}
void StringArray(char* str1 , char* str2,int basicReplace, int basicReplaceIncaseSensetive, int BracketSituation,int startReplace ,int EndReplace ,int StarReplace , char* path){

    struct flock lock;

    mode_t mode =S_IRUSR | S_IWUSR;

    int BLKSIZE=50000;
    int bytesread;
    int byteswritten=0;
    int fd,fd2;

    char* result;

    if((fd = open(path,O_RDONLY,mode)) == -1){

        perror("Failed To Open Input File !\n");

        return;

    }

    char *buf = (char *)malloc(50000*sizeof(char));

    if(buf==NULL){
        perror("Malloc Error\n");
        return;
    }

    

    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_RDLCK;

    fcntl(fd,F_SETLKW,&lock);

    while(((bytesread = read(fd,buf,BLKSIZE)) == -1 ) && (errno ==EINTR));
    
    if(bytesread < 0){

        perror("Error while reading the file !! \n ");

        return;

    }

    

    lock.l_type =F_UNLCK;

    fcntl(fd,F_SETLKW,&lock);

    int fd1 = close(fd);

    if(fd1==-1){

        perror("Failed to Close Input File !! \n");

        return;
    }

    int l = 0;

    char* str1new = (char*) malloc(sizeof(char)*1000);

    if(str1new==NULL){
        perror("Malloc Error\n");
        return;
    }

    if(basicReplaceIncaseSensetive==1){

        int m = 0;
        
        while(str1[m] !='\0'){
            
            if(str1[m]>=65 && str1[m] <=90){
                
                str1[m]=str1[m]+32;
            }

            m++;
        }

    }
    if(startReplace==1){

        for(int i=1;i<strlen(str1);i++){

            str1new[l]=str1[i];

            l++;
            
        }

    }
    if(EndReplace==1){

        l=0;

        for(int i=0;i<strlen(str1)-1;i++){

            str1new[l]=str1[i];

            l++;
        }
        
        str1new[l]= '\n';
        
    }
    
    if(startReplace==1 || EndReplace==1){

        result = starBracketFunction(buf,str1new,str2,basicReplaceIncaseSensetive,startReplace,EndReplace);
    }

    else{

        result = starBracketFunction(buf,str1,str2,basicReplaceIncaseSensetive,startReplace,EndReplace);
    }
          
    
    if((fd2 = open(path,O_WRONLY | O_TRUNC , mode)) == -1){

        perror("Failed To Open Output File !\n");

        return;

    }

    

    memset(&lock , 0 ,sizeof(lock));

    lock.l_type = F_WRLCK;

    fcntl(fd2,F_SETLKW,&lock);
    

    while(((byteswritten = write(fd2,result,strlen(result))) == -1 ) && (errno ==EINTR));

    if(byteswritten < 0){

        perror("Error while writing the file !! \n ");

        return;
    }

    

    lock.l_type =F_UNLCK;

    fcntl(fd2,F_SETLKW,&lock);

    int fd3 = close(fd2);

    if(fd3==-1){

        perror("Failed to Close Output File !! \n");

        return;
    }
    
   
}

char* starBracketFunction(char* buf , char* str1 ,char* str2,int b,int startReplace ,int EndReplace){

    size_t i = 0, j, k = 0, cursor=0,cursor1=0,lastcursor=0;
    char* result = (char*)malloc(sizeof(char)*(strlen(buf)+10));

    if(result==NULL){
        perror("Malloc Error\n");
        return NULL;
    }

    int control=0,control1=0,check=0,check1=0;
    int startcheck =0 ;

   
    
    char match=' ';
    
    while (buf[i] != '\0') {

        if(buf[i]=='\n' && startReplace == 1){
            startcheck=0;
            result[k++] = buf[i++];
            continue;
        }
        if(startcheck == 0){
            lastcursor=0;
            control=0;
            cursor=i;
            cursor1=0;
            control1=0;
            check=0;
            check1=0;
            for (j = 0; str1[j] != '\0'; j++) {
                
                if(str1[j]=='*'){
                   
            
                    cursor = i+j;
                    if(b==1){
                        while(buf[cursor]==str1[j-1] || buf[cursor]+32 == str1[j-1]){
                            cursor++;
                            lastcursor++;
                        }
                    }
                    else{
                        while(buf[cursor]==str1[j-1]){
                            cursor++;
                            lastcursor++;
                        }
                    }
                    
                    control=1;
                    continue;   
                }
                if(str1[cursor1]=='*'){
                    
                    if(control==0) {
                        cursor=i+j-1;    
                    }
                    if(b==1){
                        while(buf[cursor]==match || str1[cursor1-1]==buf[cursor] || buf[cursor]+32 == match || str1[cursor1-1] == buf[cursor]+32){
                            lastcursor++;
                            cursor++;
                        }
                    }
                    else{
                        while(buf[cursor]==match || str1[cursor1-1]==buf[cursor]){
                            lastcursor++;
                            cursor++;
                        }
                    }
                
                    
                    control=1;
                    cursor1++;
                    
                }
                if(str1[j]=='[' || str1[cursor1]=='['){
                    
                    if(control1==0){
                        cursor1=j+1;
                    }
                    
                    while(str1[cursor1]!=']'){
                        if(b==1){
                            if(str1[cursor1]==buf[i+j] || str1[cursor1] == buf[i+j]+32){
                                
                                match=buf[i+j];
                                control1=1;

                                
                            }
                            else if(str1[cursor1]==buf[cursor]  || str1[cursor1]==buf[cursor] +32 ){
                                match=buf[cursor];
                                control1=1;

                            }
                        }
                        else{
                            if(str1[cursor1]==buf[i+j]){
                                match=buf[i+j];
                                control1=1;
                               
                               
                            }
                            else if(str1[cursor1]==buf[cursor]){
                                match=buf[cursor];
                                control1=1;

                              
                            }
                        }
                        
                        cursor1++;
                    }
                    if(str1[cursor1+1]=='*'){
                        control1=1;
                        
                    }
                    
                    cursor1++;

                    if(str1[cursor1] =='\0' && control1 == 1){
                        check=1;
                    }
                    
                    if(control1==1){
                        cursor++;
                        continue;
                    }
                }
                if(control==1 ||control1 ==1){
                  
                    if(b==1){
                        
                        if ((buf[cursor] != str1[j] && buf[cursor] +32 != str1[j]) && control == 1 && control1==0){
                            
                            if(startReplace==1){
                                startcheck=1;
                            }
                            break;
                                

                        }
                        if((buf[i+j]!=str1[cursor1] && buf[i+j] +32 !=str1[cursor1]) && control1==1 && control == 0){
                            if(startReplace==1){
                                startcheck=1;
                            }
                            break;
                                
                        }
                        if((buf[cursor]!=str1[cursor1] && buf[cursor] +32 !=str1[cursor1]) && control1==1 && control == 1){
                            if(startReplace==1){
                                startcheck=1;
                            }
                        
                            break;
                                
                        }  
                    }
                    else{
                        
                        if (buf[cursor] != str1[j] && control == 1 && control1==0){
                            if(startReplace==1){
                                startcheck=1;
                            }
                            break;
                                

                        }
                        if(buf[i+j]!=str1[cursor1] && control1==1 && control == 0){
                            if(startReplace==1){
                                startcheck=1;
                            }
                            break;
                                
                        }
                        if(buf[cursor]!=str1[cursor1] && control1==1 && control == 1){
                            
                            if(startReplace==1){
                                startcheck=1;
                            }
                            break;
                                
                        }  
                    }
                        
                }
                else{
                   
                    if(b==1){
                       
                        if (buf[i + j] != str1[j] && buf[i + j] +32  != str1[j] ){
                            if(str1[j+1]!='\0' && str1[j+1]=='*'){
                                printf(" ");
                            }
                            else{
                                if(startReplace==1){
                                    startcheck=1;
                                }
                                break;
                            }       
                        }
                    }
                    else{
                        if (buf[i + j] != str1[j]){
                            if(str1[j+1]!='\0' && str1[j+1]=='*'){
                                printf(" ");
                            }
                            else{
                                if(startReplace==1){
                                    startcheck=1;
                                }    
                                break;
                            }
                                        
                        }
                    }
                    
                
                }
                
                if(control1==1 && control==0){
                    cursor1++;
                    if(str1[cursor1]=='\0'){
                        check=1;
                        break;
                    }
                    
                }
                if(control1==1 && control==1){
                    cursor1++;
                    if(str1[cursor1]=='\0'){
                        check1=1;
                        break;
                    }
                    
                }    
                cursor++;//-------------------------
            }
            if(check==1){
                i += j;//------------------------
                for (j = 0; str2[j] != '\0'; j++) 
                    result[k++] = str2[j];
                if(startReplace==1){
                    startcheck=1;
                }
                if(EndReplace==1){
                    result[k++] = '\n';
                }
                continue;
            }
            if(check1==1){
               
                i += j+lastcursor;//------------------------
                
                for (j = 0; str2[j] != '\0'; j++) 
                    result[k++] = str2[j];
                if(startReplace==1){
                    startcheck=1;
                }
                if(EndReplace==1){
                    result[k++] = '\n';
                }
                continue;
            }
            if (str1[j] == '\0' && j > 0) {
                if(control==1){
                    i=cursor;
                }
                else{
                    i += j;
                }
                for (j = 0; str2[j] != '\0'; j++) 
                    result[k++] = str2[j];
                if(startReplace==1){
                    startcheck=1;
                }
                if(EndReplace==1){
                    result[k++] = '\n';
                }
            } 
            else {
            
                result[k++] = buf[i++];
            }
        }
        else {
        
            result[k++] = buf[i++];
        }
    }
    result[k] = '\0';
    
    return result;
}
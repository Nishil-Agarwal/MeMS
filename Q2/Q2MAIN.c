#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

char optioncheck[]="-";

char ** segmentation(char* string){
    //Breaks command string into a list of strings
    char** slist=(char**)malloc(sizeof(char*)*4);
    char* ptr=strtok(string," ");
    int k=0;
    while (ptr!=NULL){
        slist[k]=(char*)malloc(sizeof(char)*strlen(ptr)+1);
        strcpy(slist[k],ptr);
        k++;
        ptr=strtok(NULL," ");
    }
    return slist;
}

int command_check(char* str){
    //Checks if command is either word(1), dir(2), date(3) or error(0)
    if (!strcmp(str,"word")){
        return 1;
    }else if(!strcmp(str,"dir")){
        return 2;
    }else if(!strcmp (str,"date")){
        return 3;
    }
    return 0;
}

int check_fname(char* name){
    //Returns 0 if file not exists
    FILE* fptr = fopen(name,"r");
    if (fptr==NULL){
        return 0;
    }
    fclose(fptr);
    return 1;
}

int check_option(char* option,int type){
    //Returns 0 if option syntax/option itself is incorrect
    if (option[0]!=optioncheck[0]){
        return 0;
    }else{
        if (strlen(option)==2){
            char op = option[1];
            if (type==1){
                if (op!='n' && op!='d'){
                    return 0;
                }
            }else if (type==2){
                if (op!='r' && op!='v'){
                    return 0;
                }
            }else if(type==3){
                if (op!='R' && op!='d'){
                    return 0;
                }
            }
        }else{
            return 0;
        }
    }
    return 1;
}

int error_message_printer(int error_type){
    if (error_type==2){
        fprintf(stderr,"Specified file doesn't exist.\n");
    }else if (error_type==3){
        fprintf(stderr,"Incorrect option usage.\n");
    }else if (error_type==10){
        fprintf(stderr,"Word option can't have both options -n and -d enabled.\n");
    }
}

int error_check(char** slist, int function){
    if (function==1){                //Word function
        if (check_option(slist[1],function)){
            if (check_option(slist[2],function)){
                return 10;              //Error when 2 options used on word command
            }else{
                if (check_fname(slist[2])){
                    if (slist[1][1]=='d' && check_fname(slist[3])){
                        return 1;
                    }else if (slist[1][1]=='n'){
                        return 1;
                    }else{
                        return 2;           //Error when file not found 
                    }
                }else{
                    return 2;              //Error when file not found
                }
            }
        }else{
            if (slist[2][0]=='-'){
                return 3;
            }else if(check_fname(slist[1])){
                return 1;           //Option incorrect usage
            }else{
                return 2;                 //Error when file not found
            }
        }
    }else if(function==2){
        if (check_option(slist[1],function) && slist[3][0]!=optioncheck[0]){
            return 1;
        }else if(slist[1][0]==optioncheck[0]){
            return 3;           //Option incorrect error
        }else{
            return 1;
        }
    }else if(function==3){               //For date command
        if (check_option(slist[1],function)){
            if (check_option(slist[2],function)){
                if (check_fname(slist[4])){
                    return 1;
                }else{
                    return 2;       //File not found error
                }
            }else if (slist[2][0]==optioncheck[0]){
                return 3;           //Option incorrect error
            }else{
                if (slist[1][1]=='d'){
                    if (check_fname(slist[3])){
                        return 1;
                    }else{
                        return 2;        //File not found error
                    }
                }else{
                    if (check_fname(slist[2])){
                        return 1;
                    }else{
                        return 2;       //File not found error
                    }
                }                    
            }
        }else{
            if (check_fname(slist[1])){
                return 1;
            }else if(slist[1][0]==optioncheck[0]){
                return 3;           //Option incorrect error
            }else{
                return 2;       //File not found error
            }
        }
    }
}

int counting_words(char* string,int newline_option){
    //Breaks sentence into a list of strings and counts elements
    char* ptr=strtok(string," ");
    int count=0;
    while (ptr!=NULL){
        if (newline_option==1){
            if (strcmp(ptr,"\n")!=0){
                count++;
            }
        }else{
            count++;
        }
        ptr=strtok(NULL," ");
    }
    return count;
}

int word_count_calc(char* file_name, int newline_option){
    //Calculates file word count sentence by sentence
    FILE* fptr=fopen(file_name,"r");
    char sentence[200];
    int total_words=0;
    while (fgets(sentence,200,fptr)){
        //int length=strlen(sentence);
        total_words+=counting_words(sentence,newline_option);
    }
    fclose(fptr);
    return total_words;
}

int* word(char** slist){
    int wordcount=-1;
    int difference=-1;
    if (!strcmp(slist[1],"-d")){
        char* fname1=slist[2];
        char* fname2=slist[3];
        int wordcount1=word_count_calc(fname1,0);
        int wordcount2=word_count_calc(fname2,0);
        if ((wordcount1-wordcount2)>0){
            difference=wordcount1-wordcount2;
        }else{
            difference=wordcount2-wordcount1;
        }
    }else{
        if (!strcmp(slist[1],"-n")){
            char* fname=slist[2];
            wordcount=word_count_calc(fname,1);
        }else{
            char* fname=slist[1];
            wordcount=word_count_calc(fname,0);
        }
    }
    int* list=(int*)malloc(sizeof(int)*2);
    list[0]=wordcount;
    list[1]=difference;
    return list;
}

void exec_command(char** slist, int function){
    //Executes all 3 functions
    if (function==1){
        int* list=(int*)malloc(sizeof(int)*2);
        list=word(slist);
        if (list[1]==-1){
            printf("It has %d words in it.\n",list[0]);
        }else{
            printf("The files have a difference of wordcount = %d\n",list[1]);
        }
        free(list);
    }else if (function==2){
        pid_t pid = fork();
        if (pid<0){
            fprintf(stderr,"Fork Failed.\n");
        }else if(pid==0){
            char* args[]={"/home/nishil_agarwal/Assignments/Submission/A1/Q2/Q2DIR",slist[1],slist[2],slist[3],NULL};
            execvp(args[0],args);
            exit(0);
        }else{
            waitpid(pid,NULL,0);
            FILE* fptr = fopen("path.txt","r");
            char path[200];
            fgets(path,sizeof(path),fptr);
            fclose(fptr);
            remove("./path.txt");
            chdir(path);
        }
    }else if (function==3){
        pid_t pid = fork();
        if (pid<0){
            fprintf(stderr,"Fork Failed.\n");
        }else if(pid==0){
            char* args[]={"/home/nishil_agarwal/Assignments/Submission/A1/Q2/Q2DATE",slist[1],slist[2],slist[3],slist[4],NULL};
            execvp(args[0],args);
            exit(0);
        }else{
            waitpid(pid,NULL,0);
        }
    }
}

int main(){
    char* str = (char*)malloc(sizeof(char)*50);
    char** segmented_str;
    while (1){
        char cwd[160];
        getcwd(cwd,sizeof(cwd));
        printf("%s>$ ",cwd);
        gets(str);
        segmented_str=segmentation(str);
        for (int i=0; i<5; i++){
            if (segmented_str[i]==NULL){
                segmented_str[i]=(char*)malloc(4);
                strcpy(segmented_str[i],"Null");
            }
        }
        int type=command_check(segmented_str[0]);
        if (type!=0){
            //Command is from any of the 3 instructions
            int error_status=error_check(segmented_str,type);
            if (error_status==1){
                //No error arose in command
                exec_command(segmented_str,type);
            }else{
                //print error messages accordingly
                error_message_printer(error_status);
            }
        }else{
            printf("Not a valid command.\n");
        }
    }
}
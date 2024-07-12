#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

char optioncheck[]= "-";

long long int newvalue(char* string){
    if ((strcmp(string,"\"yesterday\"")==0) || (strcmp(string,"\'yesterday\'")==0)){
        return -86400;
    }else if((strcmp(string,"\"tomorrow\"")==0) || (strcmp(string,"\'tomorrow\'")==0)){
        return 86400;
    }else if((strcmp(string,"\"nextweek\"")==0) || (strcmp(string,"\'nextweek\'")==0)){
        return 86400*7;
    }else if((strcmp(string,"\"lastweek\"")==0) || (strcmp(string,"\'lastweek\'")==0)){
        return -86400*7;
    }else if((strcmp(string,"\"nextmonth\"")==0) || (strcmp(string,"\'nextmonth\'")==0)){
        return 86400*30;
    }else if((strcmp(string,"\"lastmonth\"")==0) || (strcmp(string,"\'lastmonth\'")==0)){
        return -86400*30;
    }else if((strcmp(string,"\"nextweek\"")==0) || (strcmp(string,"\'nextweek\'")==0)){
        return 86400*7;
    }else if((strcmp(string,"\"nextyear\"")==0) || (strcmp(string,"\'nextyear\'")==0)){
        return 86400*365;
    }else if((strcmp(string,"\"lastyear\"")==0) || (strcmp(string,"\'lastyear\'")==0)){
        return -86400*365;
    }else if((strcmp(string,"\"nexthour\"")==0) || (strcmp(string,"\'nexthour\'")==0)){
        return 3600;
    }else if((strcmp(string,"\"lasthour\"")==0) || (strcmp(string,"\'lasthour\'")==0)){
        return -3600;
    }else if((strcmp(string,"\"now\"")==0) || (strcmp(string,"\'now\'")==0)){
        return 0;
    }else if((strcmp(string+3,"\"days\"")==0 && string[0]=='+') || (strcmp(string+3,"\'days\'")==0 && string[0]=='+')){
        return ((int)string[1])*24*3600;
    }else if((strcmp(string+3,"\"hours\"")==0 && string[0]=='+') || (strcmp(string+3,"\'hours\'")==0 && string[0]=='+')){
        return ((int)string[1])*3600;
    }else if((strcmp(string+3,"\"years\"")==0 && string[0]=='+') || (strcmp(string+3,"\'years\'")==0 && string[0]=='+')){
        return ((int)string[1])*24*3600*365;
    }else if((strcmp(string+3,"\"months\"")==0 && string[0]=='+') || (strcmp(string+3,"\'months\'")==0 && string[0]=='+')){
        return ((int)string[1])*24*3600*30;
    }else if((strcmp(string+3,"\"days\"")==0 && string[0]=='-') || (strcmp(string+3,"\'days\'")==0 && string[0]=='-')){
        return -((int)string[1])*24*3600;
    }else if((strcmp(string+3,"\"hours\"")==0 && string[0]=='-') || (strcmp(string+3,"\'hours\'")==0 && string[0]=='-')){
        return -((int)string[1])*3600;
    }else if((strcmp(string+3,"\"years\"")==0 && string[0]=='-') || (strcmp(string+3,"\'years\'")==0 && string[0]=='-')){
        return -((int)string[1])*24*3600*365;
    }else if((strcmp(string+3,"\"months\"")==0 && string[0]=='-') || (strcmp(string+3,"\'months\'")==0 && string[0]=='-')){
        return -((int)string[1])*24*3600*30;
    }else{
        fprintf(stderr,"Wrong STRING.\n");
        return -1;
    }
}

int main(int argc, char* argv[]){
    //argv format : [0]-waste, [1]-option1, [2]-option2, [3]-filename/STRING, [4]-filename
    //fill empty values with NULL
    struct stat datetime;
    if (argv[1][0]!=optioncheck[0]){
        stat(argv[1],&datetime);
    }
    
    char modified_datetime[100];
    if (strcmp(argv[1],"-d")==0 || strcmp(argv[2],"-d")==0){
        if (strcmp(argv[1],"-R")==0 || strcmp(argv[2],"-R")==0){
            stat(argv[4],&datetime);
            struct tm datetime2;
            localtime_r(&datetime.st_mtime,&datetime2);

            long long int new=newvalue(argv[3]);
            if (new==-1){
                exit(0);
            }
            time_t temptime=mktime(&datetime2)+new;
            struct tm *rfcdatetime = gmtime(&temptime);
            //RFC = Thu, 26 Aug 2023 12:34:56 +0000
            strftime(modified_datetime, 100,"%a, %d %b %Y %H:%M:%S %z\n", rfcdatetime);
        }else{
            stat(argv[3],&datetime);
            struct tm datetime2;
            localtime_r(&datetime.st_mtime,&datetime2);

            long long int new=newvalue(argv[2]);
            if (new==-1){
                exit(0);
            }
            time_t temptime=mktime(&datetime2)+new;
            strftime(modified_datetime, 100,"%a %b %d %H:%M:%S %Y\n", localtime(&temptime));
        }
    }else{
        if (strcmp(argv[1],"-R")==0 || strcmp(argv[2],"-R")==0){
            stat(argv[2],&datetime);

            struct tm *rfcdatetime = gmtime(&datetime.st_mtime);
            //RFC = Thu, 26 Aug 2023 12:34:56 +0000
            strftime(modified_datetime, 100,"%a, %d %b %Y %H:%M:%S %z\n", rfcdatetime);
        }else{
            strcpy(modified_datetime,ctime(&datetime.st_mtime));
        }
    }
    printf("%s",modified_datetime);
    exit(0);
}
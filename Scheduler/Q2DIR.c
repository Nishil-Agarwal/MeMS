#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

void recur_remove_directory(char* path,int v){
    //Removes data inside directory first before removal of it for functionality of -r option
    char pth[150];
    strcpy(pth,"./");
    strcat(pth,path);
    char pthtemp[150];
    strcpy(pthtemp,pth);
    char a[]="/";
    strcat(pth,a);

    DIR* directory = opendir(pth);
    struct dirent* entry;
    entry = readdir(directory);
    while (entry!=NULL){
        char* filename=(*entry).d_name;
        if (strcmp(filename,".")!=0 && strcmp(filename,"..")!=0){
            char temppath[150];
            strcpy(temppath,pth);
            strcat(temppath,filename);
            if ((*entry).d_type!=DT_DIR){
                remove(temppath);
                if (v==1){
                    printf("Removed file : %s \n",temppath);
                }
            }else{
                recur_remove_directory(temppath,v);
                if (v==1){
                    printf("Removed Directory : %s \n",temppath);
                }
            }
        }
        entry=readdir(directory);
    }
    closedir(directory);
    rmdir(pthtemp);
}

int directory_change_write(char* new_path){
    FILE* file=fopen("path.txt","w");
    fprintf(file, "%s", new_path);
    fclose(file);
}

int main(int argc, char* argv[]){
    //argv format : [0]-waste, [1]-option1, [2]-option2, [3]-filename
    //fill empty values with NULL
    if (strcmp(argv[1],"-v")!=0 && strcmp(argv[2],"-v")!=0){
        if (!strcmp(argv[1],"-r")){
            int status = mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (status==-1){
                recur_remove_directory(argv[2],0);
                mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                char directory[150];
                strcpy(directory,"./");
                strcat(directory,argv[2]);
                //chdir(directory);        To be used in main code instead
                directory_change_write(directory);         //Delete this temp file in main code after use
            }else{
                char directory[150];
                strcpy(directory,"./");
                strcat(directory,argv[2]);
                //chdir(directory);        To be used in main code instead
                directory_change_write(directory);         //Delete this temp file in main code after use
            }
        }else{
            int status = mkdir(argv[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (status==-1){
                char directory[3];
                strcpy(directory,"./");
                directory_change_write(directory);
                fprintf(stderr,"Directory already exists.\n");
            }else{
                char directory[150];
                strcpy(directory,"./");
                strcat(directory,argv[1]);
                //chdir(directory);        To be used in main code instead
                directory_change_write(directory);         //Delete this temp file in main code after use
            }
        }
    }else{
        if (strcmp(argv[1],"-r")==0 || strcmp(argv[2],"-r")==0){
            int status = mkdir(argv[3], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (status==-1){
                printf("Directory already exists, removing it...\n");
                recur_remove_directory(argv[3],1);
                printf("Completed removing directory.\n");
                mkdir(argv[3], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                printf("Created the directory.\n");
                char directory[150];
                strcpy(directory,"./");
                strcat(directory,argv[3]);
                //chdir(directory);        To be used in main code instead
                directory_change_write(directory);         //Delete this temp file in main code after use
                printf("Changed path to the directory.\n");
            }else{
                printf("Created the directory.\n");
                char directory[150];
                strcpy(directory,"./");
                strcat(directory,argv[3]);
                //chdir(directory);        To be used in main code instead
                directory_change_write(directory);         //Delete this temp file in main code after use
                printf("Changed path to the directory.\n");
            }
        }else{
            int status = mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (status==-1){
                char directory[3];
                strcpy(directory,"./");
                directory_change_write(directory);
                fprintf(stderr,"Directory already exists.\n");
            }else{
                printf("Created the directory.\n");
                char directory[150];
                strcpy(directory,"./");
                strcat(directory,argv[2]);
                //chdir(directory);        To be used in main code instead
                directory_change_write(directory);         //Delete this temp file in main code after use
                printf("Changed path to the directory.\n");
            }
        }
    }
    exit(0);
}
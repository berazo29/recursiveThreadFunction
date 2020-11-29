/**
 * C program to list all files and sub-directories in a directory.
 */

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


pthread_t IDS[100];
int INDEX=-1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Lists all files and sub-directories at given path.
 */
void* searchRecursively(void *arg){
    char path[256];
    char root_path[256];
    strcpy(root_path,(char *)arg);
    struct dirent *dp;
    DIR *dir = opendir(root_path);

    // Unable to open directory stream

    if (!dir) {
        return NULL;

    }
    while ((dp = readdir(dir)) != NULL){

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            pthread_mutex_lock(&mutex);

            // Construct of new path from our base path
            strcpy(path, root_path);
            strcat(path, "/");
            strcat(path, dp->d_name);
            INDEX++;
            pthread_create(&IDS[INDEX], NULL, searchRecursively, path);
            printf("path=%s\n",path);

            pthread_mutex_unlock(&mutex);
        }
    }
    closedir(dir);

    for (size_t i = 0; i < INDEX; i++){
        pthread_join(IDS[i], NULL);
    }
    pthread_exit(NULL);
}

void originalFunction(char *root_path){
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(root_path);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // Construct new path from our base path
            strcpy(path, root_path);
            strcat(path, "/");
            strcat(path, dp->d_name);
            printf("path=%s\n",path);
            originalFunction(path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[argc+1]){


    //searchRecursively(argv[1]);
    originalFunction(argv[1]);

    return 0;
}



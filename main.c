#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>

struct Node{
    char str[256];
    float occurrence_prob;
    struct Node* next;
};

struct File{
    char file_path[256];
    unsigned int total_tokens;
    struct Node *tokens;
    struct File *next;
};

// Global data-structure Linked-list of linked-list
struct File *file_head;

pthread_t IDS[100];
int INDEX=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// FUNCTION DECLARATION
// Functions for file handling
void insertNewFile(const char *file_path);
void insertNewNode(char const path_to_file[], char const token[]);
void updateProbabilities(char const path_to_file[]);
void fileInputs(const char *file_path);
void*fileHandling(void *arg);

// Utility functions for file handling
void printFilePaths(struct File *head);
void printNodes(struct Node *head);
void printAllDataStructure(struct File *head);

// Test samples for development
void insertSampleFiles();
void insertSampleTokens();


void insertNewFile(const char *file_path){
    struct File *new_file = (struct File*)malloc(sizeof(struct File));

    // File path allocation
    new_file->file_path[0] = '\0';
    strncat(new_file->file_path, file_path, sizeof(new_file->file_path) - 1);
    new_file->total_tokens = 0;
    new_file->tokens = NULL;
    // Insert in the new directory in the beginning
    new_file->next = file_head;
    file_head = new_file;
}

// Node is each token given from a file
void insertNewNode(char const path_to_file[], char const token[]){
    struct File *head = file_head;
    struct File *ptr = head;

    if ( ptr == NULL){
        perror("path_to_file does not exist");
        printf("Error: Path= %s file does not exist\n", path_to_file);
        return;
    }
    while (ptr != NULL) {
        if (strcmp(path_to_file, ptr->file_path) == 0) {

            if (ptr->tokens == NULL) {
                struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
                new_node->str[0] = '\0';
                strncat(new_node->str, token, sizeof(new_node->str) - 1);
                new_node->occurrence_prob = 1;
                ptr->tokens = new_node;
                ptr->total_tokens = 1;
                return;
            }
            struct Node *prev = NULL;
            struct Node *curr = ptr->tokens;

            while (curr != NULL) {
                prev = curr;
                if (strcmp(curr->str,token) == 0){
                    curr->occurrence_prob = curr->occurrence_prob + 1;
                    ptr->total_tokens = ptr->total_tokens + 1;
                    return;
                }
                curr = curr->next;
            }
            struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
            new_node->str[0] = '\0';
            strncat(new_node->str, token, sizeof(new_node->str) - 1);
            new_node->occurrence_prob = 1;
            prev->next = new_node;
            ptr->total_tokens = ptr->total_tokens + 1;
            return;
        }
        ptr = ptr->next;
    }
    perror("path_to_file does not exist");
    printf("Error: Path= %s file does not exist\n", path_to_file);
}

void updateProbabilities(char const path_to_file[]){
    struct File *head = file_head;
    struct File *ptr = head;

    if ( ptr == NULL){
        perror("path_to_file does not exist");
        printf("Error: Path= %s file does not exist\n", path_to_file);
        return;
    }
    while (ptr != NULL) {
        if (strcmp(path_to_file, ptr->file_path) == 0) {

            if (ptr->tokens == NULL) {
                perror("File is empty");
                return;
            }

            struct Node *curr = ptr->tokens;

            while (curr != NULL) {
                curr->occurrence_prob = curr->occurrence_prob / (float )ptr->total_tokens;
                curr = curr->next;
            }
            return;
        }
        ptr = ptr->next;
    }
    perror("path_to_file does not exist");
    printf("Error: Path= %s file does not exist\n", path_to_file);
}
void printFilePaths(struct File *head){
    struct File *ptr = head;
    if (ptr == NULL){
        printf("(NULL)\n");
        return;
    }
    printf("%d %s --> ", ptr->total_tokens, ptr->file_path);
    printFilePaths(ptr->next);
}

void printNodes(struct Node *head){
    struct Node *ptr = head;
    if (ptr == NULL){
        printf("(NULL)\n");
        return;
    }
    printf("%.3f %s --> ", ptr->occurrence_prob, ptr->str);
    printNodes(ptr->next);
}

void printAllDataStructure(struct File *head){
    struct File *ptr_file_path = head;
    printf("** SHARED DATA STRUCTURE **\n");
    if (ptr_file_path == NULL){
        printf("Data Structure empty\n");
        return;
    }
    while (ptr_file_path != NULL){
        struct Node *ptr_tokens = ptr_file_path->tokens;
        if ( ptr_tokens == NULL){
            printf("T:%d File %s is Empty\n", ptr_file_path->total_tokens, ptr_file_path->file_path);
            ptr_file_path = ptr_file_path->next;
            continue;
        }
        printf("T:%d File path= %s\n\t", ptr_file_path->total_tokens, ptr_file_path->file_path);
        while ( ptr_tokens != NULL){
            printf("n:%.3f token:%s\t", ptr_tokens->occurrence_prob, ptr_tokens->str);
            ptr_tokens = ptr_tokens->next;
        }
        printf("(NULL)\n");
        ptr_file_path = ptr_file_path->next;
    }
    printf("**\t END STRUCTURE \t**\n");
}
void insertSampleFiles(){
    insertNewFile("./a.txt");
    insertNewFile("./stuff/yark.txt");
    insertNewFile("./stuff/derp.txt");
    insertNewFile("./all/the/stuf/z.txt");
    //printFilePaths(file_head);
}
void insertSampleTokens(){
    insertNewNode("./a.txt","hi");
    insertNewNode("./a.txt","there");
    insertNewNode("./a.txt","hi");
    insertNewNode("./a.txt","there");
    updateProbabilities("./a.txt");

    insertNewNode("./stuff/yark.txt","cat");
    insertNewNode("./stuff/yark.txt","dog");
    insertNewNode("./stuff/yark.txt","doge");
    insertNewNode("./stuff/yark.txt","cat");
    insertNewNode("./stuff/yark.txt","cat");
    insertNewNode("./stuff/yark.txt","dog");
    insertNewNode("./stuff/yark.txt","cat");
    insertNewNode("./stuff/yark.txt","dog");
    updateProbabilities("./stuff/yark.txt");

    for (int i = 0; i < 200; ++i) {
        insertNewNode("./stuff/derp.txt","derp");
    }

    insertNewNode("./all/the/stuf/z.txt","hi");
    insertNewNode("./all/the/stuf/z.txt","there");
    insertNewNode("./all/the/stuf/z.txt","hi");
    updateProbabilities("./all/the/stuf/z.txt");
}

void fileInputs(const char *file_path){
    // Declare the read file
    if (file_path == NULL){
        perror("Error: no file");
        return;
    }
    FILE *fp;

    fp = fopen( file_path, "r");

    // Check if the file is empty
    if ( fp == NULL ){
        perror("Error: Unable to read file");
        return;
    }
    // Get the data
    char buffer[256];
    insertNewFile(file_path);
    while ( fscanf( fp, "%s", buffer ) != EOF ){
        //printf("%s\n", buffer);
        if( isupper(buffer[0]) ){
            buffer[0] = (char )tolower(buffer[0]);
        }
        unsigned long len = strlen(buffer);
        if ( ispunct(buffer[len-1]) ){
            buffer[len-1] = '\0';
        }
        insertNewNode(file_path,buffer);
    }

    // Close the file
    fclose(fp);

    // Update the probabilities
    updateProbabilities(file_path);
}

// PREVIOS PART
void*fileHandling(void *arg){
    char path[256];
    path[0]= '\0';
    strcpy(path, (char *)arg);

    if (arg == NULL){
        return NULL;
    }
    fileInputs(path);
    return NULL;
}

void* searchRecursively(void *arg){
    pthread_mutex_lock(&mutex);
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir((char *)arg);

    // Unable to open directory stream
    if (!dir){
        return NULL;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // Construct new path from our base path
            strcpy(path, (char *)arg);
            strcat(path, "/");
            strcat(path, dp->d_name);
            printf("path=%s\n",path);
            if (dp->d_type == DT_REG){
                printf("FILE: %s\n",path);
                INDEX++;
                pthread_create(&IDS[INDEX],NULL,fileHandling,(void *)path);
            } else if( dp->d_type == DT_DIR){
                INDEX++;
                pthread_create(&IDS[INDEX],NULL,searchRecursively,(void *)path);
            }
        }
    }

    closedir(dir);
    pthread_mutex_unlock(&mutex);
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
            if (dp->d_type == DT_REG && dp->d_type != DT_DIR){
                printf("FILE: %s\n",path);
                INDEX++;
                fileInputs(path);
            }
            originalFunction(path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[argc+1]){

    if (argc < 2){
        printf("Error: folder path only ");
        return 0;
    }
    originalFunction(argv[1]);
    //searchRecursively(argv[1]);

    for (size_t i = 1; i < INDEX; i++){
        pthread_join(IDS[i], NULL);
    }


    printAllDataStructure(file_head);


    return 0;
}



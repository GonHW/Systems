/**
 * @file tree.c
 * @author: Hench Wu(hhw14)
 * @brief 
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

struct Node {
    char* value;
    int folder;
    struct Node* next;
};

struct Node* insertToIndex(char* value, struct Node* ftree, int folder, int size, int index){
    struct Node *ptr=ftree;                                         // add node at specfic index
    struct Node *temp=(struct Node*) malloc(sizeof(struct Node));   // create temp node
	if((index>0&&size>=index)||(size==index&&size!=0)){             // insert when ftree is not null and index!=0
		for(int i=0;(ptr!=NULL)&&i<index-1;i++)
			ptr=ptr->next;
        temp->value=value;
        temp->folder=folder;
        temp->next=ptr->next;
		ptr->next=temp;
	}
	else{                       // insert in the front or when list is empty
        temp->value=value;
        temp->folder=folder;
        temp->next=ftree;
        ftree=temp;
    }
    return ftree;
}

struct Node* sortedinsert(char* key, struct Node* ftree, int folder, int size) {
    struct Node* ptr = ftree;     
    int i = 0; 
    if(ptr!=NULL)
        while (i < size && strcasecmp(key, ptr->value)>=0) {    //insert at a sorted position
            ptr = ptr->next;
            i++;
        }
    return insertToIndex(key, ftree, folder, size, i);
}

int intoFolder(char* directory, int folder, int sub){
    struct Node* ftree = NULL;
    DIR *pDir = opendir(directory);
    if (pDir==NULL) {       //when there are error accessing the folder
        fprintf(stderr, "%s [error opening dir]\n", directory);
        return --sub;
    }
    struct dirent *pDirent;
    char* filename;
    int size=0;
    while ((pDirent = readdir(pDir)) != NULL) {
        if(pDirent->d_name[0]!='.'){
        	folder=0;
            filename=(char*)malloc((strlen(pDirent->d_name)+1)*sizeof(char));   // create a string malloc length 
            strcpy(filename,pDirent->d_name);                       // create filename from direct so closedir does not affect it
            if(pDirent->d_type == DT_DIR)                           // check if file is a folder
            	folder=1;
            ftree=sortedinsert(filename, ftree, folder, size);      // insert each filename in the linkedlist 
            size++;                                                 // the size of the linkedlist
        }
    }
    closedir (pDir);

    struct Node *ptr = ftree;
    struct Node *prev = NULL;
    while(ptr!=NULL){
        for(int i=sub; i>0; i--)                //shift space for subfolders
            printf("  ");
        printf("- %s\n", ptr->value);           // print files

        if(ptr->folder){                        //if folder, go into recursion 
            char* path = malloc((strlen(ptr->value)+2+strlen(directory))*sizeof(char)); //create length of path
            strcpy(path, directory);            //build the path using previous directory
            strcat(path, "/");                  // and add /
            strcat(path, ptr->value);           // and add next folder name
  	        sub=intoFolder(path,folder, ++sub); // call for the recursion and add # of subfolder
            free(path);                         // free the path malloc
        }
        prev=ptr;                               //create previous for free
        ptr = ptr->next;                        //move ptr to next ptr
        free(prev->value);                      //free the prev-value which is filename on line 55
        free(prev);                             //free the prev node
    }  
    return --sub;                               //when finish, return to previous directory so substract subfolder
}

int main(int argc, char* argv[argc + 1]){
    DIR *pDir;
    char* directory = ".";                      //default at current directory
    if (argc!=1){                               //when there is input on commandline for folder
        directory = argv[1];                   
	    pDir = opendir (directory);             //for check if directory exist
		if (pDir == NULL) {         //check if directory exist
            fprintf(stderr, "%s [error opening dir]\n", directory);   //can't open the file
            closedir (pDir);                    //closedir for if it doesn't exist 
        	return EXIT_SUCCESS;
    	}
        closedir (pDir);
    } 
    
    printf("%s\n", directory);    
    intoFolder(directory, 0, 0);            //call to read all files and folder

    return EXIT_SUCCESS;
}

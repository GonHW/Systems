#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

struct Node {
    char* value;
    int folder;
    struct Node* next;
};

struct Node* insertToIndex(char* value, struct Node* ftree, int size, int index){
    struct Node *ptr=ftree;                                         // add node at specfic index
    struct Node *temp=(struct Node*) malloc(sizeof(struct Node));   // create temp node
	if((index>0&&size>=index)||(size==index&&size!=0)){             // insert when ftree is not null and index!=0
		for(int i=0;(ptr!=NULL)&&i<index-1;i++)
			ptr=ptr->next;
        temp->value=value;
        temp->next=ptr->next;
		ptr->next=temp;
	}
	else{                       // insert in the front or when list is empty
        temp->value=value;
        temp->next=ftree;
        ftree=temp;
    }
    return ftree;
}

struct Node* sortedinsert(char* key, struct Node* ftree, int size) {
    struct Node* ptr = ftree;     
    int i = 0; 
    if(ptr!=NULL)
        while (i < size && strcasecmp(key, ptr->value)>=0) {    //insert at a sorted position
            ptr = ptr->next;
            i++;
        }
    return insertToIndex(key, ftree, size, i);
}


void insertSort(char* list[], int size)
{
    for(int i = 1; i < size; i++)
    {
		char* temp = list[i];
		int j;
		for(j = i - 1; j >= 0 && strcasecmp(list[j],temp)>0; j--){
			list[j+1] = list[j]; 
		}
		list[j+1] = temp;
    }
} 

void getStats(char* fileName){
    struct stat fileStat;
    struct passwd *pws;
    struct group *grp;
    char date[100];
        
    if(stat(fileName, &fileStat) < 0){
        return;
    }

    //Permission String
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");

    //Username or UserID
    if ((pws = getpwuid(fileStat.st_uid)) != NULL){
        printf(" %s", pws->pw_name);
    }
    else{
        printf(" %d", fileStat.st_uid);
    }

    //groupID
    if ((grp = getgrgid(fileStat.st_gid)) != NULL){
        printf(" %s", grp->gr_name);
    }
    else{
        printf(" %d", fileStat.st_gid);
    }

    //File Size
    printf(" %ld", fileStat.st_size);

    //File Mod Time
    strftime(date, 20, "%b %d %H:%M", localtime(&(fileStat.st_mtime)));
    printf(" %s", date);

    //File Name
    printf(" %s", fileName);
    printf("\n");
}

int main(int argc, char* argv[])
{

    struct dirent* dir;                                 //diretory's entry
    int numfiles = 0;
    int count = 0;
    //Long Version
    if(argc == 2 && strcmp(argv[1], "-l") == 0){
        DIR* directory = opendir(".");                      //current directory
        DIR* directoryName = opendir(".");
        while((dir = readdir(directory)) != NULL){
            if(dir->d_name[0] != 46){
                numfiles++;
            }
        }
        char** fileArr = (char**) malloc(numfiles * sizeof(char*) );
        while((dir = readdir(directoryName)) != NULL){
            if(dir->d_name[0] != 46){
                fileArr[count] = dir->d_name;
                count++;
            }
        }insertSort(fileArr, numfiles);
        for(int i=0; i<numfiles; i++){
            getStats(fileArr[i]);
        }
        free(fileArr);
        closedir(directory);
        closedir(directoryName);
    }
    //LS short version
    else {
        DIR *pDir;
        char* input = ".";
        if (argc!=1){                      //when there is input on commandline for folder
            input = argv[1];
        }                   
	    pDir = opendir (input);             //for check if directory exist
		if (pDir == NULL) {         //check if directory exist
            fprintf(stderr, "%s [error opening dir]\n", input);   //can't open the file
            closedir (pDir);                    //closedir for if it doesn't exist 
        	return EXIT_SUCCESS;
    	}
       
     
    
        struct Node* front = NULL;
        struct dirent *pDirent;
        int size=0;
        while ((pDirent = readdir(pDir)) != NULL) {
            if(pDirent->d_name[0]!='.'){
                front=sortedinsert(pDirent->d_name, front, size);      // insert each filename in the linkedlist 
                size++;                                                 // the size of the linkedlist
            }
        }
        struct Node* ptr =front;
        while(ptr!=NULL){
            printf("%s\n", ptr->value);
            ptr = ptr->next;                        //move ptr to next ptr
        }  
        closedir(pDir);
        ptr =front;
        struct Node *prev = NULL;
        while(ptr!=NULL){
            prev = ptr;
            ptr = ptr->next;                        //move ptr to next ptr

            free(prev);                             //free the prev node
        }  
    }


    return 0;
}

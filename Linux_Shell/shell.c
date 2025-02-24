/**
 * @file shell.c
 * @author Group: Hench Wu(hhw14), Jonathan Davies(jpd244), Yiming Huang(yh714)
 * @brief 
 * @version 0.1
 * @date 2022-11-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* External variables */
extern int h_errno;    /* defined by BIND for DNS errors */ 
extern char **environ; /* defined by libc */

/* Misc constants */
#define	MAXLINE	 8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */
typedef void handler_t(int);

#define MAXARGS 512


struct Node{
	int jobID;
	pid_t pid;
	char cmd[MAXARGS];
	int state[2]; //Index 0 for complete, Index 1 for stop/run
	struct Node* next;

};

int numArgc(char* line)
{
	if(strlen(line)==0)
		return 0;
	int count = 1;
	for(int i=0; strlen(line)-1>i; i++)
		if(line[i]==' '&&line[i+1]!=' ')
			count++;
	return count;
}
static void sig_hup(int signo){
    printf("SIGHUP received, pid = %d\n", getpid());
}
static void sig_cont(int signo){
    printf("SIGCONT received, pid = %d\n", getpid());
}

int argc;
int listSize=0;
static pid_t childpid;
struct Node* headNode = NULL;
struct Node* tail = NULL;
void nullEndList()
{
	if(listSize==0)
	{
		headNode=NULL;
		tail = NULL;
	}
	else{	
		struct Node* ptr = headNode;
		for(int i=0; i<listSize-1; i++)
			ptr=ptr->next;
		ptr->next=NULL;
		tail=ptr;
	}
}
void unix_error(char *msg) /* Unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}
pid_t Fork(void) 
{
    pid_t pid;

    if ((pid = fork()) < 0)
	unix_error("Fork error");
    return pid;
}

void Kill(pid_t pid, int signum)
{
	int rc;
	if((rc=kill(pid,signum)) < 0)
		unix_error("Kill error");
}

int parseline(char *buf, char ** argv){
	char *delim;
	int bg;
	
	buf[strlen(buf)-1]=' ';
	while(*buf && (*buf == ' '))
		buf++;
		
	argc = 0;
	while((delim = strchr(buf, ' '))){
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while(*buf && (*buf==' '))
			buf++;
	}
	argv[argc] = NULL;
	if(argc==0)
		return 1;
	if(argc==1&& strlen(argv[0])>1)
	{
		if (argv[0][strlen(argv[0])-1]=='&'){
			argv[0][strlen(argv[0])-1]='\0';
			argc=2;
			argv[1]="&";
			argv[2]=NULL;
		}
	}
	
	if((bg=(*argv[argc-1]== '&')) != 0)
		argv[--argc]=NULL;
		
	return bg;
}
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}
void sigint_handler(int sig) {
    if(!kill(-childpid, 0))
        Kill(-childpid, sig);
}

struct Node* addJobNode(unsigned jobID, pid_t pid, char* cmd)
{
	struct Node* jobs = (struct Node*) malloc(sizeof(struct Node));

	jobs->jobID=jobID;
	jobs->pid=pid;
	jobs->state[0]=0;
	jobs->state[1]=0;
	jobs->next =NULL;

	strcpy(jobs->cmd, cmd);
	
	if(headNode==NULL){
		headNode=jobs;
		tail = jobs;
	}
	else{
		tail->next=jobs;
		tail=tail->next;
	}
	listSize++;
	return jobs;
}

void deleteJobNode(struct Node* job){
	if(listSize<=0){
		listSize=0;
		headNode=NULL;
		tail=headNode;
		fprintf(stderr, "No jobs in job list\n");
		return;
	}
	struct Node* prev=NULL;
	struct Node* ptr= headNode;
	while(ptr!=NULL){
		if(ptr->pid == job->pid){
			if(listSize==1){	//Removing the only job on the list
				headNode=NULL;
				tail=headNode;
			}
			else if(ptr==tail){	//Removing the last job on the list
				prev=tail;
				tail->next=NULL;
			}
			else if(headNode==ptr)	//Removing the first job on the list
			{
				headNode = ptr->next;
			}
			else{				//removing the job in between first and last
				prev->next=ptr->next;
			}
			listSize--;
			free(job);
			nullEndList();
			return;
		}
		prev=ptr;
		ptr=ptr->next;
	}
	fprintf(stderr, "Job %d not in bg job list\n", job->pid);
}

struct Node* getjobIDNode(int jobID){
	struct Node* ptr;
	for(ptr=headNode; ptr!=NULL; ptr=ptr->next)
		if(ptr->jobID==jobID)
			return ptr;
	return NULL;

}
struct Node* getpidNode(pid_t pid){
	struct Node* ptr;
	for(ptr=headNode; ptr!=NULL; ptr=ptr->next)
		if(ptr->pid==pid)
			return ptr;
	return NULL;

}




int updatingProcess(pid_t pid, int status) {
    struct Node* ptr;

    if (pid > 0) {
        /* Update the record for the process. */
        for (ptr = headNode; ptr!=NULL; ptr = ptr->next) {
            if (ptr->pid == pid) {
                if (WIFSTOPPED(status)) {
                    printf("job %d stopped\n", pid);
                    ptr->state[1] = 1;	//job stopped
                } else {
					ptr->state[0]=1;	//state is done
                    if (WIFSIGNALED(status))
                        fprintf(stderr, "[%d] %d terminated by signal %d\n", ptr->jobID, ptr->pid, WTERMSIG(status));
                    else
                        printf("[%d] %d Exited %s\n", ptr->jobID, ptr->pid, ptr->cmd);
                }
                return 0;
            }
        }
        fprintf(stderr, "No child process %d\n", pid);
        return -1;
    }
    else if (pid == 0 || errno == ECHILD)
        return -1;
    else {
        perror("waitpid");
        return -1;
    }
}

void updatingJobsList() {
    pid_t pid;
    int status;

    do{ 
        pid = waitpid(-1, &status, WUNTRACED|WNOHANG);
	}while (updatingProcess(pid, status)==0);

    struct Node* ptr = headNode;
	struct Node* prev = NULL;
	int i=0;
    while ( ptr!=NULL && i<listSize) {
		
        if (ptr->state[0]==1){	//if done
            deleteJobNode(ptr);
			ptr=prev;
		}
		if(i<listSize)
		{
			prev=ptr;
			ptr = ptr->next;
		}
		i++;
		
    }
}

int cmdKill(char** argv){
	if(argc<=1){
		printf("Please enter in this format:\nkill <jobID>\n");
		return 1;
	}
	struct Node *temp;
	if(argv[1][0]=='%'){
		unsigned jobID = atoi(argv[1]+1);
		temp=getjobIDNode(jobID);
	}
	else {
		pid_t pid = atoi(argv[1]);
		temp=getpidNode(pid);
	}
	if(temp==0){
		fprintf(stderr, "No job with this job ID or pid\n");
		return 1;
	}
	Kill(-temp->pid, SIGTERM);
	temp->state[1]=0;

		int status;
		if(waitpid(temp->pid, &status, WUNTRACED) > 0){
		
			if(WIFSTOPPED(status))
			{
				temp->state[1]=1;	//set stop
				printf("[%d] %d stopped by signal %d\n", temp->jobID, temp->pid, WSTOPSIG(status));
			}
			else {
            	if (WIFSIGNALED(status))
        			printf("[%d] %d terminated by signal %d\n", temp->jobID, temp->pid, WTERMSIG(status));
    			else if (!WIFEXITED(status))
                	printf("[%d] %d exit with an invalid status\n", temp->jobID, temp->pid);
            	deleteJobNode(temp);
			}
			
		}
		else
			unix_error("waitfg: waitpid error");
	return 1;
	
}



int cmdBG(char** argv){
	if(argc<=1){
		printf("Please enter in this format:\nbg <jobID>\n");
		return 1;
	}
	struct Node *temp;
	if(argv[1][0]=='%'){
		int jobID = atoi(argv[1]+1);
		temp=getjobIDNode(jobID);
	}
	else {
		pid_t pid = atoi(argv[1]);
		temp=getpidNode(pid);
	}
	if(temp==0){
		fprintf(stderr, "No job with this job ID or pid\n");
		return 1;
	}
	Kill(-temp->pid, SIGCONT);
	temp->state[1]=0;	//set state[1] to running
	return 1;
	
}

int cmdFG(char** argv){
	if(argc<=1){
		printf("Please enter in this format:\nfg <jobID>\n");
		return 1;
	}
	struct Node *temp;
	if(argv[1][0]=='%'){
		int jobID = atoi(argv[1]+1);
		temp=getjobIDNode(jobID);
	}
	else {
		pid_t pid = atoi(argv[1]);
		temp=getpidNode(pid);
	}

	if(temp==0){
		fprintf(stderr, "No job with specified jobID or pid\n");
		return 1;
	}
	Kill(-temp->pid, SIGCONT);
	temp->state[1]=0;	//set to running

	return 1;

}



int cmdJobs(char** argv)
{
	struct Node* ptr=headNode;;
	while(ptr!=NULL)
	{
		printf("[%d] %d %s %s", ptr->jobID, ptr->pid, ptr->state[1] ? "Stopped" : "Running", ptr->cmd);
		ptr=ptr->next;
	} 
	return 1;
}
int builtin_command(char** argv){
	if(argv[0] == NULL)
		return 1;
	if(!strcmp(argv[0], "exit")){
    	Signal(SIGHUP, sig_hup);
		Signal(SIGCONT, sig_cont);
		exit(0);
	}
	if(!strcmp(argv[0], "&"))
		return 1;
	if(!strcmp(argv[0], "kill")){
		return cmdKill(argv);
	}
	if(!strcmp(argv[0], "jobs")){
		return cmdJobs(argv);
	}
	if(!strcmp(argv[0],"cd") && argc > 1){
        //change directory fails
        if(chdir(argv[1]) != 0){
            perror("Error");
        }
        //change directory succeeds
        else{
            //set PWD
            setenv("PWD", argv[1], 1);
        }
        return 1;
    }
    //cd __ 
    if(!strcmp(argv[0],"cd") && argc == 1){
        //set home enviorment 
        char* home = getenv("HOME");
        //change directory fails
        if(chdir(home) != 0){
            perror("Error");
        }
        else{
            //set PWD
            setenv("PWD", home, 1);
        }
        return 1;
    }

	if(!strcmp(argv[0], "bg"))
		return cmdBG(argv);
	if(!strcmp(argv[0], "fg"))
		return cmdFG(argv);
	return 0;
}

void eval(char *cmdline)
{
	char* argv[MAXARGS];
	char buf[MAXLINE];

	static int jobID = 0;  

	strcpy(buf, cmdline);
	int bg = parseline(buf, argv);
	Signal(SIGINT, sigint_handler);
	Signal(SIGTSTP, sigint_handler);
	int bcmd=builtin_command(argv);
	if(!bcmd){
		if((childpid = Fork()) == 0) {
			setpgid(getpid(),0);		
			
			if((execve(argv[0], argv, environ))<0){
		
				if(execvp(argv[0], argv)<0)
				{
					if(argv[0][0]=='/'||argv[0][0]=='.')
						perror(argv[0]);
					else
						fprintf(stderr, "%s: Command not found.\n", argv[0]);
					exit(0);
				}
			}
			
		}
	}
	else{
		return;
	}


	jobID++;
	struct Node *ptr = addJobNode(jobID, childpid, cmdline); 
	
	
	if(!bg){
		int status;
		if(waitpid(ptr->pid, &status, WUNTRACED) > 0){
			if(WIFSTOPPED(status)){
				ptr->state[1]=1;	//set to stop
				printf("\n[%d] %d stopped by signal: %d\n",ptr->jobID, ptr->pid, WSTOPSIG(status));
			}
			else {
            			if (WIFSIGNALED(status))
                			printf("\n[%d] %d terminated by signal: %d\n",ptr->jobID, ptr->pid, WTERMSIG(status));
            			else if (!WIFEXITED(status))
                			printf("\n[%d] %d did not exit with a valid status\n", ptr->jobID, ptr->pid);
                		deleteJobNode(ptr);
			}
			
		}
		else
			unix_error("waitfg: waitpid error");
	}
	else{
		printf("[%d] %d %s", ptr->jobID, ptr->pid, ptr->cmd);
	}

	return;		
}




int main()
{
    char cmdline[MAXLINE];
    Signal(SIGINT, SIG_IGN);
    Signal(SIGTSTP, SIG_IGN);
    while(1){

        printf("> ");
        fgets(cmdline, MAXLINE, stdin);
        if(feof(stdin)){
        	Signal(SIGHUP, sig_hup);
    		Signal(SIGCONT, sig_cont);
		    exit(0);
		}
    	eval(cmdline);

		updatingJobsList();
	//printf("job # = %d\n", listSize);
    }
} 

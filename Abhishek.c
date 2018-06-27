/*
Title: Homework Assignment 2
Aim: Program to implement user designed system() function which also handles the SIGINT interrupt
Date: 10/26/2016
Authors:
			      Sriman Abhishek Yengaldas	 (G01010450)
*/


#include<stdio.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

int cs531_system(const char *cmd);
void ctrlc_handler(int sigval);
void child_term(int signum);

int main()
{
	char str[100];
	printf("==========User Defined SHELL==========\n");
  	cs531_system(str);		          // User defined system() function 
  	return EXIT_SUCCESS;
}

/* * * * Implementing User-defined system() function emulating SHELL COMMAND * * * */

int cs531_system(const char *comm)
{
	pid_t pid;				// pid_t is a datatype to store process IDs
  	int status,tty;
  	char *progname;
  	for(;;)
	{
    		printf("cmd:$");
    		gets(comm);        		    /* Gets the user input and passes as command within shell */
    		if(strcmp(comm,"exit")==0) 
    		{
      			printf("processing program termination\n");
      			sleep(2);		//purposely delaying exit, just to observe
      			exit(0);
    		}

    		tty=open("/dev/tty",1);            /* Opens the device terminal no.1 */
    		if(tty<0)
    		{
      			printf("%s:Can't open /dev/tty",progname);
      			return -1;
    		}
    		pid = fork();		     	   /* Creating a child process */	    
    		if(pid == 0)         		  /* pid for child process is always 0 */
  		{
    			close(0); dup(tty);      		  /* close(0) closes stdin stream whereas dup() replaces the file descriptor */
    			close(1); dup(tty);       		  /* close(1) closes stdout stream */
    			close(2); dup(tty);       		  /* close(2) closes stderr stream */
    			close(tty);
    			execlp("/bin/sh","sh","-c",comm,NULL);    /* command executed */
    			exit(127);
  		}
  		close(tty);             			/* terminal tty is now closed */
		if(pid>0)					/* pid for Parent process is greater than 0*/
		{
			int parent = getpid();
			int child = pid;
			printf("Here is the parent process with pid :%d and child pid =%d \n",parent,child);
		}
		if(pid<0)						/* negative pid means fork() failed */
		{
			perror("ERROR occurred while creating process\n");
			return EXIT_FAILURE;
		}
	

  		wait(NULL); 		/* wait() waits for child process to complete ensuring there are no zombie processes*/          
  		if (signal(SIGCHLD, child_term) == SIG_ERR)
  		{
    			perror("SIGCHLD ERROR");
    			exit(1);
  		}

  		else if(signal(SIGINT, ctrlc_handler) == SIG_ERR)    /* Signal handler used for process interruption ctrlc */
  		{
    			printf("No signal caught \n");
  		}

  		else if(WIFSTOPPED(status))         /* this returns a non-zero value if the child process is stopped */
  		{
    			printf("Stopped by signal:%d",WSTOPSIG(status));
  		}
		else if(WIFCONTINUED(status))       
  		{
    			printf("The process is continued\n");
  		}
}
return 0;
}


/*This is a signal handler function for CTRL + C interrupt*/

void ctrlc_handler(int sigval)
{
  	char stop[10];
	int stat,a;
	if WIFSIGNALED(a)					/*Returns true if interrupt signal is called */
	{
		printf("\nWARNING:Attempt to Interrupt the process detected!! \n");
		printf("\nThe signal number that caused interruption is: %d\nType a new command:\n",WTERMSIG(stat));
	}
}

/* child process termination*/  

void child_term(int signum)
{
  	int status;
  	if (signum==SIGCHLD)
  	{
    		if(WIFEXITED(status))
    		printf("Child process has terminated normally with exit status =%d & Parent is alive \n",WIFEXITED(status));
  	}
}
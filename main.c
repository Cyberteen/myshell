#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> //required for fork()

#include <readline/readline.h>
#include <readline/history.h>

#define MAXIMUM_COMMAND_LENGTH 16 	//in english words
#define DEFAULT_PATH_LEN 32 		//in bytes
#define PATH_LENGTH_INCREMENT 8 	//in bytes

int main(int argc, char *argv[], char *envp[])
{
	
	printf("Welcome to my shell! \n");
	printf("******************** \n");
	
	pid_t 	child;	    
	char 	*command[16], *tok, *lineptr = NULL; 	// Not going to use commands with more than 15 words; extra room for NULL term
	size_t 	i, n; 					// Number of bytes read into lineptr (mallocated internally by getline)
	size_t 	pathLen = DEFAULT_PATH_LEN ; 		//setting initial path length to 32 characters
	int 	status;
	char 	*cwdBuf = (char*)malloc(sizeof(char)*pathLen);

	while(1)
	{	
		while(1)
		{
			if( NULL == getcwd(cwdBuf,pathLen)) //getcwd returned NULL, so check for error
			{
				if(ERANGE == errno) //initial path length not sufficient, we increase pathLen and reallocate buf
				{
					free(cwdBuf);
					pathLen += PATH_LENGTH_INCREMENT;
					cwdBuf = (char*) malloc(sizeof(char)*pathLen);
					printf("getcwd returned insufficient path\n");
				}
				else
				{
					printf("getcwd returned error: %s",strerror(errno));
					free(cwdBuf);
					break;
				}
			}
			else
				break; //getcwd is success, so break
			
		} 

		printf("myshell:%s$ ",cwdBuf);
	
		if(NULL == (lineptr=readline("")))
			break;
		
		//strtok cannot parse empty char buffers
		if(strlen(lineptr) > 0)
			tok = strtok(lineptr, " \t\n\r"); // First call to strtok stores the line in a static variable; second arg is delimiters
		else
		{
			free(lineptr); //lineptr must be freed before readline() can use it
			continue;
		}

	  	for (i = 0; i < 16 && tok != NULL; i++) // strtok returns NULL once there are no more tokens

	  	{
	      		command[i] = tok;
	      		tok = strtok(NULL, " \t\n\r");
	       	}

		command[i] = NULL;

		//checks for built-in commands which run without creating a process
		
		if(0 == strcmp(command[0],"exit")) //exits the shell
			break;

		if(0 == strcmp(command[0],"cd")) //process cd commands
		{
			if(0 != chdir(command[1]))
			{
				printf("error: (%s) \n", strerror(errno));
			}

			continue;
		}


		child = fork(); //fork creates a child process. returns 0 to parent process and child id to child process  on success else a error num

		if(child == 0)
		{
			
			if(execvp(command[0],command))
			{
				perror("execvp");
				exit(EXIT_FAILURE);
			}
			
		}
		//else
		//{
		//	printf("fork failed! errno.(%s) \n", strerror(errno));				
		//}

		if(child > 0) 
		{
			//printf("Child process created with PID (%d)\n", child);
			wait(&status);
		}

		if(NULL != lineptr)
			free(lineptr);


	}

	putchar('\n');

	if(NULL != lineptr)	
		free(lineptr);

	if(NULL != cwdBuf)
		free(cwdBuf);

	printf("*****************************\n");
	printf("Thank you for using my shell!\n");
	
	exit(status);

}



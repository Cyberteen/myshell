#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> //required for fork()

//#include <readline/readline.h>
//#include <readline/history.h>

#define MAXIMUM_COMMAND_LENGTH 16 	//in words
#define DEFAULT_PATH_LEN 32 		//in bytes
#define PATH_LENGTH_INCREMENT 8 	//in bytes

int main(int argc, char *argv[], char *envp[])
{
	
	printf("Welcome to my shell! \n");
	
	pid_t child;	    
	char *command[16], *tok, *lineptr = NULL; // Not going to use commands with more than 15 words; extra room for NULL term
	size_t i, n; 	// Number of bytes read into lineptr (mallocated internally by getline)
	size_t pathLen = DEFAULT_PATH_LEN ; //setting initial path length to 32 characters
	int status;
	char *buf = (char*)malloc(sizeof(char)*pathLen);

	while(1)
	{	
		while(1)
		{
			if( NULL == getcwd(buf,pathLen)) //getcwd returned NULL, so check for error
			{
				if(ERANGE == errno) //initial path length not sufficient, we increase pathLen and reallocate buf
				{
					free(buf);
					pathLen += PATH_LENGTH_INCREMENT;
					buf = (char*) malloc(sizeof(char)*pathLen);
					printf("getcwd returned insufficient path\n");
				}
				else
				{
					printf("getcwd returned error: %s",strerror(errno));
					free(buf);
					break;
				}
			}
			else
				break; //getcwd is success, so break
			
		} 

		printf("myshell:%s$ ",buf);
		
	       	if (getline(&lineptr, &n, stdin) == -1)
			break;

	  	tok = strtok(lineptr, " \t\n\r"); // First call to strtok stores the line in a static variable; second arg is delimiters

	  	for (i = 0; i < 16 && tok != NULL; i++) // strtok returns NULL once there are no more tokens

	  	{
	      		command[i] = tok;
	      		tok = strtok(NULL, " \t\n\r");
	       	}

		command[i] = NULL;

		child = fork(); //fork creates a child process. returns 0 to parent process and child id to child process  on success else a error num

		if(child == 0)
		{
			
			if(execve(command[0],command,envp))
			{
				perror("execve");
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
		
		

		//printf("no.of tokens: %lu \n", i);
		//printf("command[0]: %s \n",command[0]);

	}

	putchar('\n');
	free(lineptr);
	if(buf!=NULL)
		free(buf);
	exit(status);

}



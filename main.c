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

int main(int argc, char *argv[], char *envp[])
{
	
	printf("Welcome to my shell! \n");
	
	pid_t child;	    
	char *command[16], *tok, *lineptr = NULL; // Not going to use commands with more than 15 words; extra room for NULL term
	size_t i, n; // Number of bytes read into lineptr (mallocated internally by getline)
	int status;

	while(1)
	{

		printf("myshell$");
		
	       	if (getline(&lineptr, &n, stdin) == -1)
			break;

	  	tok = strtok(lineptr, " \t\n\r"); // First call to strtok stores the line in a static variable; second arg is delimiters

	  	for (i = 0; i < 16 && tok != NULL; i++) // strtok returns NULL once there are no more tokens

	  	{
	      		command[i] = tok;
	      		tok = strtok(NULL, " \t\n\r");
	       	}

		command[i] = NULL;

		child = fork(); //fork creates a child process. returns 0 on success else a error num

		if(0 == child)
		{
			if(execve(command[0],command,envp))
			{
				perror("execve");
				exit(EXIT_FAILURE);
			}
		}
		if(0 > child) 
		{
			printf("Child process created with PID (%d)", child);
			wait(&status);
		}
		else
		{
			printf("fork failed! errno.(%s) \n", strerror(errno));				
		}
	

		//printf("no.of tokens: %lu \n", i);
		//printf("command[0]: %s \n",command[0]);

	}

	putchar('\n');
	free(lineptr);
	exit(status);

//	return(0);
}



/**
Name  : Shameen Shetty
ID    : 1001429743
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  int historyCount = 0;
  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check

	//This will store upto 15 commands that have been entered by the user
    char argumentHistory[16][55];


    char argumentString[50] = "";
    int token_index;
    for( token_index = 0; token_index < token_count - 1; token_index ++ )
    {
    	if(token[token_index] != NULL)
    	{
	    	strcat(argumentString, " ");
	    	strcat(argumentString, token[token_index]);

	    	if(token_index == (token_count - 2))
	    	{
	    		strcpy(argumentHistory[historyCount], argumentString);
	    		historyCount++;
	    		
	    		if(historyCount > 15)
	    		{
	    			//once historyCount reaches 15, the max num that
	    			//can be stored, we will reset historyCount and
	    			//so that the previous commands will be ovewritten
	    			// and we we see only the last 15 arguments entered
	    			historyCount = 0;
	    		}
	    	}
    	}
    }

  	pid_t pid = fork();
  	if(pid == 0)
  	{
    	int ret = execvp(token[0], token);

    	if(strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
        {
          	//if 'quit' or 'exit' is entered, we will exit with a status of zero
          	exit(0);
        }

    if(strcmp(token[0], "cd") == 0)
    {
    	//This will give the current directory
    	char newDir[50] = "/";
		char cwd[35];
	    getcwd(cwd, sizeof(cwd));
	    strcat(newDir, token[1]);

	    //We will concatenate the newDir to the current
	    //dir in order to get the path we want to 
	    //change directories to
	    strcat(cwd, newDir);
	    if (chdir(cwd) != 0)
	    {
    		perror("Directory does not exist.\n");
	    }
    	
    	ret = 0;
    } else {
    	int status;
    	wait( & status );
    }

    if(strcmp(token[0], "showpids") == 0)
    {
		pid_t firstpid = getpid();
		printf("pid: %ld\n", (long) firstpid);

		for(int i = 0; i < 10; i++)
		{
			pid_t pid = getpid();
			if(pid != firstpid)
				printf("pid: %ld\n", (long) pid);
		}

		ret = 0;
    }

    if(strcmp(token[0], "history") == 0)
    {
		for(int count = 0; count < historyCount; count++)
		{
			//print the command history only for the commands that are entered,
			//for example if the history is {"cd test", "ls"} then the elements
			//ls will be null, which will not be printed.
			if(argumentHistory[count] != NULL){
				printf("%d: %s\n", count, argumentHistory[count]);
			}
		}

		ret = 0;
    }

    if(token[0][0] == '!')
    {
    	int historyNum = token[0][1] - '0';

	    char cwd[35];
	    getcwd(cwd, sizeof(cwd));

	    //TODO !n not working, not able to do the command
	    if(historyNum > historyCount)
	    {
	    	printf("Command not in history.\n");
	    }
	    if(historyNum < historyCount)
	    {
	    	execvp(argumentHistory[historyNum], token);
	    }
    	ret = 0;
    }

    if(ret == -1)
    {
      fprintf(stderr, "%s : Command not found\n", token[0]);
    }
  }
  else
  {
    int status;
    wait(&status);
  }


    free( working_root );

  }
  return 0;
}

#include "ui.h"


int toArgv(char* source, char* dest[], int size){
	int cnt = 0;
	char* token = malloc(100);
	token = strtok(source," ");
	
	while (token != NULL && (cnt < size)){
		dest[cnt] = malloc(sizeof(char*));
		strcpy(dest[cnt], token);
		cnt++;
		//printf("%s\n", token);
		token = strtok(NULL, " ");
	}
	dest[cnt] = NULL;
	return cnt;
}


char** parse_command(char* command)
{
	const char whitespace[7] = " \t\n\v\f\r";
	char *token;

  char* copy = malloc(strlen(command)+1);
  strncpy(copy,command,strlen(command));
	//count tokens, start with first
	int count = 0;
	token = strtok(copy, whitespace);
   
	/* walk through other tokens */
	while( token != NULL ) 
	{
		token = strtok(NULL, whitespace);
		count++;
	}
	char** ret = malloc((count+1)*sizeof(char*));
	//make last cell of ret point to NULL
	ret[count] = NULL;

	//read first token into ret
	count = 0;
  strncpy(copy,command,strlen(command));
  token = strtok(copy, whitespace);
  ret[count] = token;
  count++;

	/* walk through other tokens */
	while( token != NULL ) 
	{
		token = strtok(NULL, whitespace);
  	ret[count] = token;
		count++;
	}
	return ret;
}
#include "main.h"
	
int main(void) 
{
	Drive* d;
	char* disk = malloc(sizeof(char*));
	bool opened = false;
	int exit = 0;
	printf("Tiny File System\n");
	printf(":> ");
	while (!exit)
	{
		size_t size = 256;
		char* command = malloc(size);
		getline(&command,&size,stdin);
		char** tokens = parse_command(command);
		exit = !(strcmp(tokens[0],"exit"));
		if (!strcmp(tokens[0], "create")){
			if(tokens[1] == NULL){
				printf("Invalid command\n");
			}
			else{
				if (fopen(tokens[1], "a") == NULL){
					printf("Invalid file path\n");
				}
				else{
					disk = tokens[1];
					d = newDrive();
					opened = true;
				}
			}
		}

		else if(!(strcmp(tokens[0],"open"))){
			if(tokens[1] == NULL){
				printf("Invalid command\n");
			}
			else{
				if (fopen(tokens[1], "r") == NULL){
					printf("Invalid file path\n");
				}
				else{
					disk = tokens[1];
					d = open(tokens[1]);
					opened = true;
				}
			}
		}
			
		else if (!(strcmp(tokens[0],"import"))){
			if(!opened){
				printf("No Open Drive\n");
			}
			else{
				if(tokens[1] == NULL || tokens[2] == NULL){
					printf("Invalid command\n");
				}
				else{
					FILE *infp = fopen(tokens[1], "r");
					
					if(infp == NULL){
						printf("Invalid command\n");
						fclose(infp);
					}
					else{
						fclose(infp);
						import(d, tokens[1], tokens[2]);
						save(d, disk);
					}
					
				}
			}
		}

		else if (!(strcmp(tokens[0],"mkDir"))){
			if(!opened){
				printf("No Open Drive\n");
			}
			else{
				if(tokens[1] == NULL){
					printf("Invalid command\n");
				}
				else{
					mkDir(d, tokens[1]);
					save(d, disk);
				}
			}
		}

		else if (!(strcmp(tokens[0],"rm"))){
			if(!opened){
				printf("No Open Drive\n");
			}
			else{
				if(tokens[1] == NULL){
					printf("Invalid command\n");
				}
				else{
					rm(d, tokens[1]);
					save(d, disk);
				}
			}
		}
			
		else if (!(strcmp(tokens[0],"export"))){
			if(!opened){
				printf("No Open Drive\n");
			}
			else{
				if(tokens[1] == NULL || tokens[2] == NULL){
					printf("Invalid command\n");
				}
				else{
					FILE *outfp = fopen(tokens[2], "r");
					
					if(outfp == NULL){
						printf("Invalid command\n");
					}
					else{
						fclose(outfp);
						export(d, tokens[2], tokens[1]);
						save(d, disk);
					}
				}
			}
		}
			
		else if (!(strcmp(tokens[0],"ls"))){
			if(!opened){
				printf("Drive does not exist\n");
			}
			else{
				if (tokens[1] != NULL){
					ls(d, tokens[1]);
				}
				else{
					lsRoot(d);
				}
			}
		}
			
		else if (!(strcmp(tokens[0],"display"))){
			if(!opened){
				printf("Drive does not exist\n");
			}
			else{
				printf("%s",displayDrive(d));
			}
		}
			
		else if (!exit){
				printf("command not recognized.\n");		
		}
		if (!exit){
			printf(":> ");
		}
	}
	printf("Program terminated.\n");
	save(d, disk);
}
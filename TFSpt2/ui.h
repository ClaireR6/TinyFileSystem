#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int toArgv(char*, char*[], int);
char** parse_command(char*);
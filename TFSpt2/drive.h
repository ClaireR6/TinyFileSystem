#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
unsigned char* block[16];
} Drive;

Drive* newDrive();
char* displayDrive(Drive*);
int isUsed(Drive* d, int pos);
unsigned char* dump(Drive*);
void import(Drive*, char*, char*);
void ls(Drive*, char*);
void lsRoot(Drive*);
void save(Drive*, char*);
Drive* open(char*);
void mkDir(Drive*, char*);
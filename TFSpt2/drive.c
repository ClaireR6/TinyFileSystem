#include "drive.h"

unsigned char* dump(Drive* d)
{
  unsigned char* ret = malloc(256);
  for (int i=0; i<16; i++)
  {
    for (int j=0; j<16; j++)
    {
      ret[16*i+j] = d->block[i][j];
    }
  }
  return ret;
}

Drive* newDrive()
{
  Drive* d = malloc(sizeof(Drive));
  for (int i=0; i<16; i++)
  {
    d->block[i] = malloc(16);
  }
  d->block[0][0] = 1; //0000 0001
  d->block[0][1] = 0; //0000 0000
  return d;
}

Drive* open(char* LP){
	Drive* d = newDrive();
	FILE* drive = fopen(LP, "r");
	for(int lcv=0;lcv<16;lcv++){
		for(int lcv2=0;lcv2<16;lcv2++){
			char c = fgetc(drive);
			if(c!='\n'){
				if(c=='-'){
					d->block[lcv][lcv2] = NULL;
				}
				else{
					d->block[lcv][lcv2] = c;
				}
			}
		}
	}
	fclose(drive);
	return d;
}

int isUsed(Drive* d, int pos)
{
  unsigned short bitmap =0;
  if (pos >= 0 && pos <= 16)
  memcpy(&bitmap,d->block[0],2);
  unsigned short mask = 1;
  if (pos >= 0 && pos <= 16)
    mask = mask << pos;
  else
    mask = 0;
  return  !((mask&bitmap)==0);
  
}

char* displayDrive(Drive* d)
{
  const int RETSIZE = 1024;
  char* temp  = malloc(RETSIZE);
  char* ret = malloc(RETSIZE);
  ret[0] = '\0';
  strncat(ret,"   ",1024);
  for(int i=0; i<16; i++)
  {
    sprintf(temp,"  %x",i);
    strncat(ret,temp,RETSIZE);
  }
  strncat(ret,"\n",RETSIZE);

  for(int row=0; row<16; row++)
  {
    sprintf(temp,"%x: ",row);
    strncat(ret,temp,RETSIZE);    
    for(int col=0; col< 16; col++)
    {
      sprintf(temp,"%3.2x",d->block[row][col]);
      strncat(ret,temp,RETSIZE);      
    }
    strncat(ret,"\n",RETSIZE);
  }
  return ret;
}

void import(Drive* d, char* lp, char* tp){
	if(tp[0]!='/'){
		printf("Invalid path\n");
		return;
	}
	FILE *infp = (fopen(lp, "r"));
	char c = fgetc(infp);;
	int index = 0;
	char* b = malloc(16);
	for(int lcv=0;lcv<16 && c!=EOF;lcv++){
		b[lcv] = c;
		c=fgetc(infp);
	}
	
	char* path = malloc(strlen(tp));
	strcpy(path, tp);
	//path = tp;
	unsigned long mask;
	int address = 0;
	char* root = d->block[address];
	char* token = malloc(sizeof(char*));
	token = strtok(path,"/");
	char* temp = root;
	while (token != NULL){
		if (strlen(token)>1) {
				printf("Invalid path\n");
				return;
			}
		mask = 0x01;
		if (token[0] >= 'A' && token[0] <= 'Z'){ // if token is a directory
			bool found = false;
			for(int lcv=3;lcv<11;lcv++){ // find token in block
				if (temp[lcv]==token[0]){ // token found
					found = true;
					if((root[0] & mask) == 0){ // check directory hasnt been removed
						printf("Invalid path\n");
						return;
					}
					if((lcv)%2==0){ // get next directory address
						address = (d->block[address][11+((lcv-1)/2)] & 0xF0) >> 4; // upper
					}
					else{
						address = (d->block[address][11+((lcv-1)/2)] & 0xF); // lower
					}
					temp = d->block[address];
					if (!isUsed(d, address)) {
						printf("Invalid path\n");
						return;
					} 
				}
				mask = mask << 1;
			}
			if (!found && address == 0) {
				printf("Invalid path\n");
				return;
			}
		}
		else{ //  token is a file
			for(int lcv=3;lcv<11;lcv++){
				if (temp[lcv] == token[0] && (temp[2] & (mask<<(lcv-3))) != 0){
					printf("File already exists\n");
					return;
				}
			}
			if(b[15] != NULL){
				printf("File size is too large\n");
				return;
			}
			else{
				for(int lcv=3;lcv<11;lcv++){
					if (temp[lcv] == token[0] && (temp[2] & (mask<<(lcv-3))) != 0){
						printf("File already exists\n");
						return;
					}
				}
			
				int lcv=3;
				while((temp[2] & mask) != 0 && lcv < 11){ // find open space
					mask = mask << 1;
					lcv++;
				}
				if (lcv > 10) printf("Directory is full\n");
				else{
					mask = 0xFF;
					if(root[0] != mask && d->block[address][2] != mask){ // check for a free block
						
						mask = 0x02;
						int lcv = 1;
						while((root[0] & mask) != 0){ // find free block location
							mask = mask << 1;
							lcv++;
						}
						root[0] = root[0] | mask;
						mask = 0x01;
						for(int lcv2=1;lcv2<16;lcv2++){
							d->block[lcv][lcv2] = b[lcv2-1];
						}
						unsigned long index = 3;
						
						while((temp[2] & mask) != 0){ // find file location within directory
							mask = mask << 1;
							index++;
						}
						mask = 0x01;
						temp[index] = token[0];
						temp[2] = temp[2] | mask << index-3;
						if((index)%2==0){ // set file directory address
							temp[11+((index-1)/2)] = (temp[11+((index-1)/2)] & 0x0F) | (lcv << 4); // upper
						}
						else{
							temp[11+((index-1)/2)] = (temp[11+((index-1)/2)] & 0xF0) | lcv; // lower
						}
					}
				}
			}
		}
		token = strtok(NULL, "/");
	} // end while
	return;
}

void export(Drive* d, char* lp, char* tp){
	if(tp[0]!='/'){
		printf("Invalid path\n");
		return;
	}
	char* path = malloc(strlen(tp));
	strcpy(path, tp);
	//path = tp;
	unsigned long mask;
	int address = 0;
	char* root = d->block[address];
	char* token = malloc(sizeof(char*));
	token = strtok(path,"/");
	char* temp = root;
	while (token != NULL){
		if (strlen(token)>1) {
				printf("Invalid path\n");
				return;
			}
		mask = 0x01;
		if (token[0] >= 'A' && token[0] <= 'Z'){ // if token is a directory
			bool found = false;
			for(int lcv=3;lcv<11;lcv++){ // find token in block
				if (temp[lcv]==token[0]){ // token found
					found = true;
					if((root[0] & mask) == 0){ // check directory hasnt been removed
						printf("Invalid path\n");
						return;
					}
					if((lcv)%2==0){ // get next directory address
						address = (d->block[address][11+((lcv-1)/2)] & 0xF0) >> 4; // upper
					}
					else{
						address = (d->block[address][11+((lcv-1)/2)] & 0xF); // lower
					}
					temp = d->block[address];
					if (!isUsed(d, address)) {
						printf("Invalid path\n");
						return;
					} 
				}
				mask = mask << 1;
			}
			if (!found && address == 0) {
				printf("Invalid path\n");
				return;
			}
			token = strtok(NULL,"/");
		}
		else{ //  token is a file
			for(int lcv=3;lcv<11;lcv++){
				if (temp[lcv] == token[0] && (temp[2] & (mask<<(lcv-3))) != 0){
					if((lcv)%2==0){ // get next directory address
						address = (d->block[address][11+((lcv-1)/2)] & 0xF0) >> 4; // upper
					}
					else{
						address = (d->block[address][11+((lcv-1)/2)] & 0xF); // lower
					}
					char* b = malloc(15);
					char* s = malloc(15);
					for(int lcv=1;lcv<16;lcv++){
						sprintf(s, "%c", d->block[address][lcv]);
						strcat(b, s);
					}
					FILE* outfp = fopen(lp, "w");
					fprintf(outfp, "%s", b);
					fclose(outfp);
					return;
				}
			}
		}
	return;
	}
}

void rm(Drive* d, char* tp){
	if(tp[0]!='/'){
		printf("Invalid path\n");
		return;
	}
	char* path = malloc(strlen(tp));
	strcpy(path, tp);

	unsigned long mask;
	int address = 0;
	int par = 0;
	
	char* root = d->block[address];
	char* token = malloc(sizeof(char*));
	char* tokPrev = malloc(sizeof(char*));
	
	token = strtok(path,"/");
	char* temp = root;
	
	while (token != NULL){
		temp = d->block[address];
		
		if (strlen(token)>1) {
				printf("Invalid path\n");
				return;
			}
		
		mask = 0x01;
		if ((token[0] >= 'A' && token[0] <= 'Z') || (token[0] >= 'a' && token[0] <= 'z')){
			bool found = false;
			for(int lcv=3;lcv<11;lcv++){ // find token in block
				if (temp[lcv]==token[0]){ // token found
					found = true;
					if((root[0] & mask) == 0){ // check directory hasnt been removed
						printf("Invalid path\n");
						return;
					}

					if((lcv)%2==0){ // get next directory address
						par = address;
						address = (d->block[address][11+((lcv-1)/2)] & 0xF0) >> 4; // upper
					}
					else{
						par = address;
						address = (d->block[address][11+((lcv-1)/2)] & (0xF)); // lower
					}
					if (!isUsed(d, address)) {
						printf("Invalid path\n");
						return;
					} 
				}
				mask = mask << 1;
			}
			if (!found && strtok(NULL, "/") != NULL) {
				printf("Invalid path\n");
				return;
			}
		}
		else{
			printf("Invalid path\n");
			return;
		}
		tokPrev = token;
		token = strtok(NULL, "/");
	} // end while
	
	char* pBlock = d->block[par];
	int spot;
	for(int lcv=3;lcv<11;lcv++){
		if(pBlock[lcv]==tokPrev[0]){
			pBlock[lcv] = NULL;
			pBlock[2] = pBlock[2] & ~(1 << (lcv-3));
			if((lcv)%2==0){ // get next directory address
				address = pBlock[11+((lcv-1)/2)] & 0xF0;
				pBlock[11+((lcv-1)/2)] = pBlock[11+((lcv-1)/2)] & 0xF0;
			}
			else{
				address = pBlock[11+((lcv-1)/2)] & 0xF;
				pBlock[11+((lcv-1)/2)] = pBlock[11+((lcv-1)/2)] & 0xF;
			}
			root[0] = root[0] & ~(0x01 << address);
			break;
		}
	}
	for(int lcv=0;lcv<16;lcv++){
		d->block[address][lcv] = 0;
	}
	return;
}

void mkDir(Drive* d, char* tp){
	if(tp[0]!='/'){
		printf("Invalid path\n");
		return;
	}
	char* path = malloc(strlen(tp));
	strcpy(path, tp);

	unsigned long mask;
	int address = 0;
	
	char* root = d->block[address];
	char* token = malloc(sizeof(char*));
	char* tokPrev = malloc(sizeof(char*));
	
	token = strtok(path,"/");
	char* temp = root;
	
	while (token != NULL){
		temp = d->block[address];
		
		if (strlen(token)>1) {
				printf("Invalid path\n");
				return;
			}
		
		mask = 0x01;
		if (token[0] >= 'A' && token[0] <= 'Z'){ // if token is a directory
			bool found = false;
			for(int lcv=3;lcv<11;lcv++){ // find token in block
				if (temp[lcv]==token[0]){ // token found
					found = true;
					if((root[0] & mask) == 0){ // check directory hasnt been removed
						printf("Invalid path\n");
						return;
					}

					if((lcv)%2==0){ // get next directory address
						address = (d->block[address][11+((lcv-1)/2)] & 0xF0) >> 4; // upper
					}
					else{
						address = (d->block[address][11+((lcv-1)/2)] & (0xF)); // lower
					}
					if (!isUsed(d, address)) {
						printf("Invalid path\n");
						return;
					} 
				}
				mask = mask << 1;
			}
			if (!found && strtok(NULL, "/") != NULL) {
				printf("Invalid path\n");
				return;
			}
		}
		else{ //  invalid token
			printf("Invalid path\n");
		}
		tokPrev = token;
		token = strtok(NULL, "/");
	} // end while
	
	mask = 0xFF;
	if(root[0] != mask && d->block[address][2] != mask){
		
		mask = 0x02;
		int lcv = 1;
		while((root[0] & mask) != 0){
			mask = mask << 1;
			lcv++;
		}
		root[0] = root[0] | mask;
		mask = 0x01;
		d->block[lcv][1] = address;
		unsigned long index = 3;
		char* pBlock = d->block[address];
		while((pBlock[2] & mask) != 0){
			mask = mask << 1;
			index++;
		}
		mask = 0x01;
		pBlock[index] = tokPrev[0];
		pBlock[2] = pBlock[2] | mask << index-3;
		if((index)%2==0){
			pBlock[11+((index-1)/2)] = (pBlock[11+((index-1)/2)] & 0x0F) | (lcv << 4); // upper
		}
		else{
			pBlock[11+((index-1)/2)] = (pBlock[11+((index-1)/2)] & 0xF0) | lcv; // lower
		}
	}
	
	return;
}

void ls(Drive* d, char* tp){
	if(tp[0]!='/'){
		printf("Invalid path\n");
		return;
	}
	char* path = malloc(strlen(tp));
	strcpy(path, tp);
	//path = tp;
	unsigned long mask = 0x01;
	int address = 0;
	char* root = d->block[address];
	char* token = malloc(sizeof(char*));
	token = strtok(path,"/");
	char* temp = root;
	while (token != NULL){
		if (token[0] >= 'A' && token[0] <= 'Z'){ // if token is a directory
			bool found = false;
			for(int lcv=3;lcv<11;lcv++){ // find token in block
				if (temp[lcv]==token[0]){ // token found
					found = true;
					if((root[0] & mask) == 0){ // check directory hasnt been removed
						printf("Invalid path\n");
						return;
					}
					if((lcv)%2==0){ // get next directory address
						address = (d->block[address][11+((lcv-1)/2)] & 0xF0) >> 4; // upper
					}
					else{
						address = (d->block[address][11+((lcv-1)/2)] & (0xF)); // lower
					}
					temp = d->block[address];
					if (!isUsed(d, address)) {
						printf("Invalid path\n");
						return;
					} 
				}
				mask = mask << 1;
			}
			if (!found && strtok(NULL, "/") != NULL) {
				printf("Invalid path\n");
				return;
				}
		}
		else{ //  token is a file
			printf("Invalid path\n");
			return;
		}
		token = strtok(NULL, "/");
		mask = 0x01;
	} // end while

	mask = 1;
	for(int lcv=3;lcv<11;lcv++){
		if((temp[2] & (mask << (lcv-3))) != 0){ // check file exists
			printf("%c ", temp[lcv]);
		}
	}
	printf("\n");
	return;
}

void lsRoot(Drive* d){
	unsigned long mask = 0x01;
	char* root = d->block[0];
	for(int lcv=3;lcv<11;lcv++){
		printf("%c ", root[lcv]);
		if((root[2] & mask) != 0){ // check file exists
			//printf("%c ", root[lcv]);
		}
		mask = mask << 1;
	}
	printf("\n");
	return;
}

void save(Drive* d, char* LP){
	FILE* drive = fopen(LP, "w");
	char* result = malloc(16);
	for(int lcv=0;lcv<16;lcv++){
		char* block = d->block[lcv];
		for(int lcv2=0;lcv2<16;lcv2++){
			if (block[lcv2] == NULL){
				result[lcv2] = '-';
			}
			else{
				result[lcv2] = block[lcv2];
			}
		}
		fprintf(drive, "%s", result);
	}
	fclose(drive);
}
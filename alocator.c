#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;

typedef struct{
	uchar *V;
	int size;
	int isEmpty;
	int start_bloc;
} Arena;
 

//creeam arena
void initialization (Arena *arena, int size_arena){
	arena->start_bloc = 0;
	arena->isEmpty = 1;
	arena->size = size_arena;
	arena->V = (uchar *) calloc(size_arena, sizeof(uchar)) ;
}

int getNum(uchar *v){
	int rez = 0,k,p=1;
	for(k = 0;k<4;k++,p*=256) rez+= v[k] * p;
	return rez;
}

void inNum(uchar *v, int n){
	int k;
	for(k=0;k<4;k++){
		v[k] = n%256;
		n/=256;
	}
}

void finalize (Arena *arena){
	free (arena->V);
}

void DUMP (Arena *arena){
	int i;
	for (i = 0;i < arena->size;i++) {
		if (i % 16 == 0) {
			if(i > 0 && (i+1) <= arena->size){
				printf("\n");
			}
			printf("%08X\t",i);
		}else{
		
			if(i % 8 == 0) {
				printf(" ");
			}
		}
		printf("%02X ",arena->V[i]);
	}
	printf("\n");
}

int alocare(Arena *arena,int alloc_size){
	if(arena->isEmpty){
		if(arena->size-12 < alloc_size) return 0;
		inNum(arena->V+8,alloc_size);
		arena->isEmpty=0;   
		arena->start_bloc=0;	//daca e gol
		return 12;
	}else 
	if(arena->start_bloc >= alloc_size + 12){   //verificam daca putem adauga inainte de primul bloc
		inNum(arena->V,arena->start_bloc); // primul va fi urmatorul
		inNum(arena->V+4,0);  //precedentul nu exista
		inNum(arena->V+8,alloc_size);
		arena->start_bloc=0; //primul devine curentul (0)
		return 12;
	}
	
	
	//cercetam spatiul dintre fiecare 2 blocuri
	int curent = arena->start_bloc;
	int next = getNum(arena->V + curent);
	
	while(next != 0){
		int av_size = next - (curent + 12 + getNum(arena->V+curent+8));
		if(alloc_size + 12 <= av_size){ //daca putem baga 
			int new_bloc = (curent + 12 + getNum(arena->V+curent+8));
			
			inNum(arena->V + curent,new_bloc);
			
			inNum(arena->V + new_bloc,next);
			inNum(arena->V + new_bloc+4,curent); // ap bagam
			inNum(arena->V + new_bloc+8,alloc_size);
			
			inNum(arena->V + next + 4,new_bloc);
			
			return new_bloc + 12;
		}else{ // daca nu
			curent = next; // cautam o borta mai mare
			next = getNum(arena->V + next);
		}
	}
	
	//daca nu am gasit borta incercam sa adaugam la capat
	
	// in caz ca next==0 ( curent e ultimul bloc) 
	
	int bloc_size = getNum(arena->V + curent+8);
	 
	
	if(arena->size - (curent + 12+bloc_size) < alloc_size + 12) return 0; // verificam daca putem adauga dupa ultimul bloc
	
	int new_bloc = curent+12+bloc_size;
	inNum(arena->V + new_bloc,0);
	inNum(arena->V + new_bloc+4,curent);
	inNum(arena->V + new_bloc+8,alloc_size);
	
	
	inNum(arena->V + curent,new_bloc);
	return new_bloc+12;
}
	
void Free(Arena *arena, int index){
	index-=12;
	
	
	int bloc_size = getNum(arena->V + index + 8);
	
	int succ = getNum(arena->V + index);
	
	if(index == arena->start_bloc){
		arena->start_bloc = succ; // daca stergem primul bloc, atunci primul deja devine urmatorul
		if(succ == 0) arena->isEmpty=1; // daca nu am avut un bloc urmator, atunci exista doar unul
	}
	
	int pred = getNum(arena->V + index + 4);
	
	int i;
	for(i=0;i<12+bloc_size;i++) arena->V[index + i] = 0;
	
	if(succ != 0) inNum(arena->V + succ + 4, pred);
	if(getNum(arena->V + pred + 8) > 0) inNum(arena->V + pred, succ);
	
}	

void Fill(Arena *arena, int index, int cnt, int val){
	int k,bloc_size = getNum(arena->V + index + 8);
	
	for(k = 0;k<bloc_size;k++){
		 arena->V[index+12+k] = val;
		 cnt=cnt-1;
		 if(cnt == 0) return;
	}
	
	if(getNum(arena->V + index) != 0) Fill(arena,getNum(arena->V + index),cnt,val);
}

void parse_command(char* cmd,Arena *arena){
    const char* delims = " \n";

    char* cmd_name = strtok(cmd, delims);
    if (!cmd_name) {
        goto invalid_command;
    }

    if (strcmp(cmd_name, "INITIALIZE") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);

		initialization(arena,size);

    } else if (strcmp(cmd_name, "FINALIZE") == 0) {
        
        finalize(arena);
        
    } else if (strcmp(cmd_name, "DUMP") == 0) {
		DUMP(arena);
    } else if (strcmp(cmd_name, "ALLOC") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        
        printf("%d\n",alocare(arena,size));
        
    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
       
		Free(arena,index);

    } else if (strcmp(cmd_name, "FILL") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* value_str = strtok(NULL, delims);
        if (!value_str) {
            goto invalid_command;
        }
        uint32_t value = atoi(value_str);
        Fill(arena,index-12,size,value);

    } else {
        goto invalid_command;
    }

    return;

invalid_command:
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(void){
	
	ssize_t read;
    char* line = NULL;
    size_t len;

	Arena arena;
	
    while ((read = getline(&line, &len, stdin)) != -1) {
        printf("%s", line);
        parse_command(line,&arena);
        
        free(line);
		line = NULL;
    }

    free(line);

    return 0;
}
	

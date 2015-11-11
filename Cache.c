#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

typedef struct{
	int order; //to help finding the least recently used
	int valid;
	int tag;
	int counter; // to help finding the least frequently used
	int * blocks; //define content of block
} Line;
 
typedef struct{
	int id;
	Line* lines;		
} Set;
	
typedef struct{
	Set* sets;
} Cache;
 
void initCache();
void readAddress();	
void hit(Line * l);
void miss(Set * set);
void statistics();
void insertLine(Line * l);
void freeMemory();

unsigned int address;

int S, E, B, m, h, p, t, s, b, tag, setIndex, blockOffset;

int hits, misses, counter;

char rpolicy[3];

Cache cache;
 
int main(){
	S=0, E=0, B=0, m=0, h=0, p=0, t=0, s=0, b=0, tag=0, setIndex=0, blockOffset=0;
	hits=0, misses=0, counter=0;
	scanf("%d %d %d %d",&S,&E,&B,&m);
	scanf("%s",rpolicy);
	scanf("%d %d",&h,&p);
	initCache();
	readAddress();
	int found, i;				
	while(address != -1){
		//set selection
		Set set = cache.sets[setIndex];
		//line matching
		found = 0;
		for(i=0;i<E;i++){
			if(set.lines[i].valid && set.lines[i].tag == tag){
				hit(&set.lines[i]);
				found = 1;
				break;
			}				
		}
		if(!found)
			miss(&set);
		readAddress();		
	}
	freeMemory();
	statistics();
	return 0;
}

void initCache(){
	s = log(S)/log(2);
	b = log(B)/log(2);
	t = m - (s+b);

	cache.sets = malloc(S * sizeof(Set));
	int i;
	for(i = 0; i<S; i++){
		cache.sets[i].id = i;
		cache.sets[i].lines = malloc(E * sizeof(Line));
		int j;
		for(j=0;j<E;j++){
			cache.sets[i].lines[j].blocks = malloc( B * sizeof(int)); //TODO define size of a word
			cache.sets[i].lines[j].valid = 0;
			cache.sets[i].lines[j].tag = 0;
			cache.sets[i].lines[j].counter = 0;
			cache.sets[i].lines[j].order = 0;
		}
	}	
}


void readAddress(){
	scanf("%x",&address); //t bits / s bits / b bits
	int tagMask = ((1 << t) - 1) << (s+b);
	tag = (address & tagMask) >> (s+b); //t bits
	int setIndexMask = ((1 << s) - 1) << b;
	setIndex = (address & setIndexMask) >> b; //s bits
	int blockMask  = (1 << b) - 1;
	blockOffset =  address & blockMask; // b bits 
}

void hit(Line * l){
	printf("%02X H\n", address);
	hits++;
	l->counter++;
	l->order = counter;
	counter++;
}

void miss(Set * set){
	printf("%02X M\n", address);
	misses++;
	if(strcmp(rpolicy,"LRU")==0){
		//Least-Recently-Used
		int i, minIndex=-1, min=INT_MAX, found = 0;
		for(i=0;i<E;i++){
			if(!set->lines[i].valid){
				insertLine(&set->lines[i]);
				found = 1;
				break;
			}
			if(set->lines[i].order<min){
				minIndex=i;
				min = set->lines[i].order;
			}
		}
		if (!found)
		{
			insertLine(&set->lines[minIndex]);
		}
	} else if (strcmp(rpolicy,"LFU")==0){
		//Least-Frequently-Used
		int i, minIndex=-1, min=INT_MAX, found = 0;
		for(i=0;i<E;i++){
			if(!set->lines[i].valid){
				insertLine(&set->lines[i]);
				found = 1;
				break;
			}
			if(set->lines[i].counter<min){
				minIndex=i;
				min = set->lines[i].counter;
			}
		}
		if (!found)
		{
			insertLine(&set->lines[minIndex]);
		}
	} else {
		printf("could not recognize the replacement policy\n");
	}
}

void statistics(){
	//miss rate - misses/references = misses/(misses+hits)
	printf("%d ", ((int) ((float) misses/(misses+hits)*100)));
	//total cycles
	printf("%d\n", hits*h+misses*p);
	//printf("hits: %d misses: %d h: %d p: %d \n",hits,misses,h,p);
}

void insertLine(Line * l){
	l->valid = 1;
	l->tag = tag;
	l->counter = 1;
	l->order = counter;
	counter++;
}

void freeMemory(){
	int i,j;
	for(i = 0; i<S; i++){
		for(j=0;j<E;j++){
			free(cache.sets[i].lines[j].blocks);
		}
		free(cache.sets[i].lines);
	}
	free(cache.sets);		
}

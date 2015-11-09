#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

unsigned int address = 0;

unsigned int S=0, E=0, B=0, m=0, h=0, p=0, t=0, s=0, m=0, b=0, tag=0, setIndex=0, blockOffset=0;

int hits=0, misses=0, counter=0;

char * r-policy;

Cache cache;

typedef struct{
	Set* sets;
} Cache;
 
typedef struct{
	int id;
	Line* lines;		
} Set;	
 
typedef struct{
	int order; //to help finding the least recently used
	int valid = 0;
	int tag=0;
	int counter = 0; // to help finding the least frequently used
	*int blocks; //define content of block
} Line; 
 
int main(){
	scanf("d% d% d% d% s% d% d%",S,E,B,m,r-policy,h,p);
	initCache();
	readAddress();	
	scanf("%x",address);			
	while(address!="-1"){
		//set selection
		Set set = cache.set[setIndex];
		//line matching
		int found = 0, i;
		Line line;
		for(i=0;i<E;i++){
			if(set.lines[i].valid && set.lines[i].tag == tag){
				hit(set.lines[i]);
				found = 1;
			}				
		}
		if(!found)
			miss(set);
		scanf("%x",address);			
	}
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
		cache.sets[i] = malloc(sizeof(Set));
		cache.sets[i].id = i;
		cache.sets[i].lines = malloc(E * sizeof(Line));
		int j;
		for(j=0;j<E;j++){
			cache.sets[i].lines[j] = malloc(sizeof(Line));
			cache.sets[i].lines[j].blocks = malloc( B * sizeof(int)); //TODO define size of a word
			cache.sets[i].lines[j].tag = 0;
		}
	}
}


void readAddress(){	
	scanf("%x",address); //t bits / s bits / b bits
	int tagMask = ((1 << t) - 1) << s+b;
	tag = address & tagMask; //t bits
	int setIndexMask = ((1 << s) - 1) << b;
	setIndex = address & setIndexMask; //s bits
	int blockMask  = (1 << b) - 1;
	blockOffset =  address & blockMask; // b bits 
}

void hit(Line * l){
	stdout("%x H", address);
	hits++;
	l->counter++;
	l->order = counter;
	counter++;
}

void miss(Set * set){
	stdout("%x M", address);
	misses++;
	if(r-policy == "LRU"){
		//Least-Recently-Used
		int minIndex=0, min=INT_MAX;
		for(i=0;i<E;i++){
			if(!set.lines[i].valid){
				insertLine(set.lines[i]);
				break;
			}
			if(set.lines[i].order<min){
				minIndex=i;
				min = set.lines[i].order;
			}
		}
		insertLine(set.lines[minIndex]);
	} else if (r-policy == "LFU"){
		//Least-Frequently-Used
		int minIndex=0, min=INT_MAX;
		for(i=0;i<E;i++){
			if(!set.lines[i].valid){
				insertLine(set.lines[i]);
				break;
			}
			if(set.lines[i].counter<min){
				minIndex=i;
				min = set.lines[i].order;
			}
		}
		insertLine(set.lines[minIndex]);
	} else {
		stdout("could not recognize the replacement policy");
	}
}

void statistics(){
	//TODO miss rate - misses/references = misses/(misses+hits)
	//TODO total cycles
}

void insertLine(Line * l){
	l->valid = 1;
	l->tag = tag;
	l->counter = 1;
	l->order = counter;
	counter++;
}

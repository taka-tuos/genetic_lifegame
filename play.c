#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define __PLAYER__
#include "eval.h"

int main(int argc, char *argv[])
{
	int dna[SIZE * SIZE];
	
	char s[32];
	sprintf(s, PATHFMT, atoi(argv[1]));
	
	FILE *fp = fopen(s, "rt");
	
	int p = 0;
	
	char s1[512];
	
	while(fgets(s1, 512, fp)) {
		char *sz = s1;
		p(sz);
		while(*sz) {
			if(*sz == '*' | *sz == ' ') {
				dna[p++] = (*sz == '*' ? 1 : 0);
			}
			sz++;
		}
	}
	
	fclose(fp);
	
	eval(dna);
	
	return 0;
}

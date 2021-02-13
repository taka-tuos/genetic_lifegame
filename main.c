#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "eval.h"

typedef struct {
	int score;
	int id;
} eval_score_t;

int compare_score(const void *a, const void *b)
{
	return ((eval_score_t*)b)->score - ((eval_score_t*)a)->score;
}


#define PNUM 32
#define TNUM 4

int main(int argc, char *argv[])
{
	int *dna_a[PNUM];
	int *dna_b[PNUM];
	int *dna_t[TNUM];
	
	for(int p = 0; p < TNUM; p++) dna_t[p] = (int *)malloc(SIZE * SIZE * sizeof(int));
	
	for(int p = 0; p < PNUM; p++) {
		dna_a[p] = (int *)malloc(SIZE * SIZE * sizeof(int));
		dna_b[p] = (int *)malloc(SIZE * SIZE * sizeof(int));
		int *dna = dna_a[p];
		for(int i = 0; i < SIZE * SIZE; i++) dna[i] = ((rand() % atoi(argv[1])) == 1) ? 1 : 0;
	}
	
	int gen = 0;
	
	p("Gen 0 generate complete\n");
	
	while(1) {
		eval_score_t es[PNUM];
		#pragma omp parallel for
		for(int p = 0; p < PNUM; p++) {
			int *dna = dna_a[p];
			#pragma omp critical
			p("Gen %d id %d start\n", gen, p, es[p].score);
			es[p].score = eval(dna);
			es[p].id = p;
			#pragma omp critical
			p("Gen %d id %d finished : score %d\n", gen, p, es[p].score);
		}
		
		qsort(es, PNUM, sizeof(eval_score_t), compare_score);
		
		int dna_t[TNUM][SIZE * SIZE];
		for(int i = 0; i < TNUM; i++) memcpy(dna_t[i], dna_a[es[i].id], SIZE * SIZE * sizeof(int));
		
		p("*** Gen %d : score %d (id=%d)***\n", gen, es[0].score, es[0].id);
		
		char s[16];
		
		sprintf(s, PATHFMT, gen);
		
		FILE *fp = fopen(s, "wt");
		int *dna_x = dna_a[es[0].id];
		for(int i = 0; i < SIZE * SIZE; i++) {
			fputc(" *"[dna_x[i]], fp);
			if((i % SIZE) == SIZE - 1) fputc('\n', fp);
		}
		fclose(fp);
		
		gen++;
		
		for(int i = 0; i < TNUM; i++) memcpy(dna_b[i], dna_t[i], SIZE * SIZE * sizeof(int));
		
		for(int i = 0; i < TNUM; i++) {
			for(int j = 0; j < TNUM; j++) {
				int *dna_pa = dna_t[i];
				int *dna_pb = dna_t[j];
				for(int c = 0; c < SIZE * SIZE; c++) {
					dna_b[TNUM+i*TNUM+j][c] = rand() & 1 ? dna_pa[c] : dna_pb[c];
				}
			}
		}
		
		int last = PNUM - (TNUM+TNUM*TNUM);
		int lhead = TNUM+TNUM*TNUM;

		for(int p = 0; p < last; p++) {
			int ch = rand() % PNUM;
			int dna_px[SIZE * SIZE];
			memcpy(dna_px, dna_a[ch], SIZE * SIZE * sizeof(int));
			for(int i = 0; i < SIZE * SIZE; i++) {
				if((rand() % 1000) < 10) {
					dna_px[i] = rand() & 1;
				}
			}
			memcpy(dna_b[lhead+p], dna_px, SIZE * SIZE * sizeof(int));
		}
		
		for(int i = 0; i < PNUM; i++) memcpy(dna_a[i], dna_b[i], SIZE * SIZE * sizeof(int));
	}
	
	return 0;
}

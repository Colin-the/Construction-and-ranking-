#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned long long fact;

// Construction functions
char * genBitString(int n);
int * generateUniversalCycle(int n);

// Ranking function
int rankLehmer(int *U, int L, int n, int start);
int rank7Order(int *perm, int n);
int rankRuskeyWilliams(int *p, int n);

// Helper functions
unsigned long long factorial(unsigned int n);
void rotate_n(int *p, int n);
void rotate_n_minus_1(int *p, int n);
void visit(int *perm, int n, int *sp_cycle, int *count);
void shift(int *a, int i, int j);
void bell7(int *perm, int n, int m, int *sp_cycle, int *count);
int is_duplicate(int *cycle, int pos, int len, int n);


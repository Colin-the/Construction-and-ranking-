#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned long long fact;

// Construction functions

// Ranking function
int rankLehmer(int *U, int L, int n, int start);
int rank7Order(int *perm, int n);
int rankRuskeyWilliams(int *p, int n);

// Helper functions
unsigned long long factorial(unsigned int n);
void rotate_n(int *p, int n);
void rotate_n_minus_1(int *p, int n);
char * genBitString(int n);
int * generateUniversalCycle(int n);

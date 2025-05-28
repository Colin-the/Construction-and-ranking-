#include "constructAndRank.h"

// Helper function to rotate a string of size n to the left
void rotate_n(int *p, int n){
    int first = p[0];
    for(int i = 0; i < n-1; i++) p[i] = p[i+1];
    p[n-1] = first;
}
  
// Helper function to rotate a string of size n while 
// holding the last element constant
void rotate_n_minus_1(int *p, int n){
    int first = p[0];
    for (int i = 0; i < n-2; i++) p[i] = p[i+1];
    p[n-2] = first;
}
  
// Generate the bitstring Sₙ for n ≥ 2, using the 
// loopless algorithm prestented in the Ruskey–Williams paper
char * genBitString(int n){
    // Set up arrays for the algorithm
    int *a = calloc(n + 2, sizeof(int));
    int *d = malloc((n + 2) * sizeof(int));
    int *f = malloc((n + 1) * sizeof(int));

    char *bitstring = malloc((fact + 1) * sizeof(char));

    // Check if memory allocation was successful
    if (!bitstring || !a || !d || !f){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    unsigned long long bitlen = 0;
    int j;

    // Initially do dₙ...d₁ <- 1...1
    // and fₙ, fₙ₋₁...f₁ <- n + 1, n-1...1 
    for (int i = 1; i < n; i++){
        d[i] = 1;
        f[i] = i;
    }
    d[n] = 1;
    f[n] = n + 1;

    // This is a error in the original paper, d[n+1] is never defined
    // in the pseudocode however it is used by the algorithm. The max 
    // value of j is n+1 so we need to ensure that d[n+1] exists when 
    // the program attempts to access it. 
    d[n+1] = 1;

    do{
        // Grab and then reset the first element of f
        j = f[1];
        f[1] = 1;

        // Now if j is even XOR (a[j] - d[j] ≤ 0 OR a[j] - d[j] ≥ n - j), then
        // the next bit is 0, otherwise it is 1
        int diff = a[j] - d[j];
        int flip = ((j % 2 == 0) ^ (diff <= 0 || diff >= (n - j)));
        bitstring[bitlen++] = flip ? '0' : '1';
        a[j] = a[j] + d[j];

        // Check to see if we need to update d[j] and f[j]
        if (a[j] == 0 || a[j] == n - j){
        d[j] = -d[j];
        f[j] = f[j + 1];
        f[j + 1] = j + 1;
        }
    } while (j < n);

    // Add null terminator for ease of use 
    bitstring[bitlen] = '\0';

    // If desired print the bitstring and its length
    //printf("bitstring: %s\n", bitstring);
    //printf("bitlen: %lld\n", bitlen);

    // Clean up allocated memory
    free(a);
    free(d);
    free(f);

    return bitstring;
}

// Generate the shorthand universal cycle for Π(n),
// using the loopless σₙ/σₙ₋₁ algorithm of Ruskey–Williams
int * generateUniversalCycle(int n){
    if (n < 2){
        int *result = malloc(sizeof(int));
        result[0] = n;
        return result;
    }

    // Generate Sₙ into bitstring[] 
    char * bitstring = genBitString(n);
    if (bitstring == NULL) return NULL;


    // Initialize the starting permutation to n, n-1, ..., 1
    int *perm = malloc(n * sizeof(int));
    for(int i = 0; i < n; i++) perm[i] = n-i;


    // Allocate space for the universal cycle
    int * UC = malloc((fact + 1) * sizeof(int));

    // Check if memory allocation was successful
    if (!UC || !perm) return NULL;

    // Walk through the bitstring and apply the σₙ/σₙ₋₁
    // rotations to the starting permutation to generate
    // the universal cycle
    for (unsigned long long i = 0; i < fact; i++){
        UC[i] = perm[0];
        if (bitstring[i] == '0') rotate_n(perm, n);
        else rotate_n_minus_1(perm, n);
    }


    // Clean up
    free(perm);

    // Note:  Enabling this free results in a munmap_chunk(): 
    // invalid pointer error.  I am not sure why this is happening
    // as we only read from this memory in this function. The code 
    // works fine without this free with a small memory leak caused 
    // by the bitstring
    free(bitstring);

    return UC;
}

void visit(int *perm, int n, int *sp_cycle, int *count) {
    for (int i = 0; i < n - 1; ++i)
        sp_cycle[*count * (n - 1) + i] = perm[i];
    (*count)++;
}

void bell7(int *perm, int n, int m, int *sp_cycle, int *count) {
    if (m == n + 1) {
        visit(perm, n, sp_cycle, count);
        return;
    }

    bell7(perm, n, m + 1, sp_cycle, count);

    for (int i = m - 2; i >= 0; --i) {
        // Perform prefix-rotation: rotate perm[0..m-1] to the left by 1
        int tmp = perm[0];
        for (int j = 0; j < m - 1; ++j)
            perm[j] = perm[j + 1];
        perm[m - 1] = tmp;

        bell7(perm, n, m + 1, sp_cycle, count);
    }
}
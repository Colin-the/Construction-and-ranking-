#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rankRuskeyWilliams(int *p, int n);
int rankLehmer(int *U, int L, int n, int start);

// Helper function to compute n!
unsigned long factorial(unsigned int n){
  unsigned long f = 1;
  for (unsigned int i = 2; i <= n; i++) f *= i;
  return f;
}
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
  int *d = malloc((n + 1) * sizeof(int));
  int *f = malloc((n + 1) * sizeof(int));

  char *bitstring = malloc((factorial(n) + 1) * sizeof(char));

  // Check if memory allocation was successful
  if (!bitstring || !a || !d || !f){
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  int bitlen = 0, j;

  // Initially do dₙ...d₁ <- 1...1
  // and fₙ, fₙ₋₁...f₁ <- n + 1, n-1...1 
  for (int i = 1; i < n; i++){
    d[i] = 1;
    f[i] = i;
  }
  d[n] = 1;
  f[n] = n + 1;

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
  //printf("bitlen: %d\n", bitlen);

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
  int * UC = malloc((factorial(n) + 1) * sizeof(char));

  // Check if memory allocation was successful
  if (!UC || !perm) return NULL;

  // Walk through the bitstring and apply the σₙ/σₙ₋₁
  // rotations to the starting permutation to generate
  // the universal cycle
  for (int i = 0; i < strlen(bitstring); i++){
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
  //free(bitstring);

  return UC;
}

// Return 1 if U[0..] of length n! is a valid shorthand U‑cycle for Π(n), 0 otherwise
int isUniversalCycle(int *U, int L, int n){
  unsigned long len = factorial(n);
  if (len <= 0 || len != L) return 0;

  // Mark which ranks we've seen
  char *seen = malloc(len);
  if (!seen) {
      fprintf(stderr, "Error: memory allocation failed\n");
      return 0;
  }
  memset(seen, 0, len);

  int algo = 1;
  for (int i = 0; i < len; i++) {
    // Call a ranking function
    int rank;
    if (algo) rank = rankLehmer(U, len, n, i);
    else{
      int perm[n], sum = 0;
      //printf("{");
      for (int j=0; j< n-1; j++){ 
        perm[j] = U[(i+j) % len];
        sum += perm[j];
        //printf("%d,",perm[j]);
      }
      // Find the missing symbol 1..n in the permutation using 
      // the fact that ∑n = n(n+1)/2 and ∑window = ∑n - missing
      // So missing = ∑n - ∑window
      int missing = (n * (n + 1) / 2) - sum;
      perm[n-1] = missing;

      //printf("}\n");
      rank = rankRuskeyWilliams(perm, n);
      //printf("r: %d\n", rank);
    }
    

    // If the rank is invalid or we have seen this rank before
    // then the given string is not a universal cycle
    if (rank < 0 || rank >= len || seen[rank]) {
        free(seen);
        return 0;
    }
    // Otherwise mark this rank as seen
    seen[rank] = 1;
  }

  free(seen);
  return 1;
}
  
int main(void){
  int n;
  printf("Enter n: ");
  scanf("%d", &n);

  int *UC = generateUniversalCycle(n);
  if (UC == NULL){
    printf("Error generating universal cycle\n");
    return 0;
  }

  // Print the universal cycle
  printf("UC: ");
  for (int i = 0; i < (factorial(n)); i++) printf("%d", UC[i]);
  printf("\n DONE \n");
  free(UC);


  // int test1[] = {3,2,1,3,1,2};               
  // int test2[] = {4,3,2,1,4,2,1,3,4,1,3,2,4,3,1,2,4,1,2,3,4,2,3,1};

  // struct {
  //     int *U;
  //     int L;
  //     int  n;
  // } tests[] = {
  //     { test1, sizeof(test1) / sizeof(test1[0]), 3 },
  //     { test2, sizeof(test2) / sizeof(test2[0]), 4 },
  //     { NULL,    0 }   
  // };

  // for (int i = 0; tests[i].U; i++) {
  //     char valid = 1;
  //     int *U = tests[i].U;
  //     int L = tests[i].L; 
  //     int  n = tests[i].n;
  //     unsigned long len = factorial(n);

  //     // print U in “{3,2,1,3,1,2}” style
  //     printf("U={");
  //     for (int j = 0; j < len; j++) {
  //         printf("%d", U[j]);
  //         if (j + 1 < len) printf(",");
  //     }
  //     printf("}  n=%d  ->  %s\n",
  //            n,
  //           isUniversalCycle(U, L, n) ? "YES" : "no");
  // }

  // fprintf(stderr,"start\n");


  //   int perms[6][3] = {
  //     {3,2,1},
  //     {2,1,3},
  //     {1,3,2},
  //     {3,1,2},
  //     {1,2,3},
  //     {2,3,1},
  //   };

  //   for (int i = 0; i < 6; i++) {
  //       int *p = perms[i];
  //       int rk = rankRuskeyWilliams(p, 3);
  //       printf("%d: %d %d %d  → rank %d\n",
  //              i, p[0],p[1],p[2], rk);
  //   }



  // fprintf(stderr,"Done\n");

  return 0;
}



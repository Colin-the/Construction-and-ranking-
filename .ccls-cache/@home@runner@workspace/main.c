#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to compute n!
unsigned long factorial(unsigned int n){
  unsigned long f = 1;
  for (unsigned int i = 2; i <= n; i++) f *= i;
  return f;
}
// Helper function to rotate a string of size n
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

  char *bitstring = malloc((factorial(n) + 5) * sizeof(char));

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
  printf("bitstring: %s\n", bitstring);
  printf("bitlen: %d\n", bitlen);

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
    result[0] = 1;
    return result;
  }

  // Generate Sₙ into bitstring[] 
  char * bitstring = genBitString(n);
  int bitlen = strlen(bitstring);
  printf("bitlen: %s\n", bitstring);

  
  // Step 2: walk through Sₙ and apply σₙ/σₙ₋₁

  // Initialize the starting permutation to n, n-1, ..., 1
  int *perm = malloc(n * sizeof(int));
  for(int i = 0; i < n; i++) perm[i] = n-i;

  int * UC = malloc((factorial(n) + 1) * sizeof(char));

  // Step 3: output first symbol of each permutation
  for (int i = 0; i < bitlen; i++){
    UC[i] = perm[0];
    if (bitstring[i] == '0') rotate_n(perm, n);
    else rotate_n_minus_1(perm, n);
  }
  
  
  //printf("\n");
  
  // Clean up
  free(perm);
  //free(bitstring);

  return UC;
}

// Given U of length L and parameters n, rank the substring of length n-1
// starting at index 'start' (circularly).  Returns a rank in [0..n!-1] or 
// -1 if the string is invalid.
int rankLehmer(char *U, int L, int n, int start){
  //fprintf(stderr,"Rank called\n");
  char used[n]; // tracks which of 1..n appear in the substring
  memset(used, 0, sizeof(char) * n); // initialize to false
  int window[n]; // holds the n-1 symbols in numeric form
  int w = 0;

  // 1) Collect the n-1 symbols, check they’re in 1..n and distinct.
  for (int t = 0; t < n - 1; t++){
    char c = U[(start + t) % L];
    if (c < '1' || c >= '1' + n) return -1; // out of range     
    int x = c - '1'; // in 0..n-1
    if (used[x]) return -1; // duplicate symbol     
    used[x] = 1;
    window[w++] = x + 1; // store as 1..n
  }

  // 2) Find the one missing symbol in 1..n
  int missing = 0;
  for (int k = 0; k < n; k++){
    if (!used[k]){
      missing = k + 1;
      break;
    }
  }

  // 3) Build the full permutation pi[0..n-1]
  int pi[n];
  for (int i = 0; i < n - 1; i++) pi[i] = window[i];
  pi[n - 1] = missing;

  // 4) Compute its Lehmer‐code rank in [0..n!-1]
  int rank = 0;
  long fact = factorial(n - 1);
  for (int i = 0; i < n; i++) {
    int smaller = 0;
    for (int j = i + 1; j < n; j++)
      if (pi[j] < pi[i]) smaller++;
    rank += smaller * fact;
    if (i < n - 1) fact /= (n - 1 - i);
  }
  return rank;
}

// Return true if U of length L is a valid shorthand U-cycle for Π(n)
char isUniversalCycle(char *U, int L, int n){
  //fprintf(stderr,"in isUniversalCycle\n");
  long must = factorial(n);
  if (L != must) return -1;
  
  // System error: want to use calloc here however for some reason this results
  // in a malloc(): corrupted top size error. Swtich malloc to calloc in
  // final version if it does not result in errors.
  char *seen = (char*) malloc(L * sizeof(char));
  for (int i = 0; i < L; i++) seen[i] = 0;

  
  if (!seen) {
    printf("Memory allocation failed\n");
    return -1;
  }
  fprintf(stderr,"in isUniversalCycle\n");
  for (int i = 0; i < L; i++) {
    int r = rankLehmer(U, L, n, i);
    if (r < 0 || seen[r]) {
      free(seen);
      return 0;
    }
    seen[r] = 1;
  }
  free(seen);
  return 1;
}

int main(void){
  int n;
  printf("Enter nn: ");
  if (scanf("%d", &n) != 1)
    return 0;
  // generateSPCycle(n);
  int *UC = generateUniversalCycle(n);
  printf("UC: ");
  for (int i = 0; i < (factorial(n)); i++) printf("%d", UC[i]);
  printf("\n");
  
  const char *test[] = {
      "432143124123412342314231", "4",
      "432142134132431241234231", "4",
      NULL};
  //printf("U=\"%s\"  n=%d  -> %s\n", test[0], n, isUniversalCycle(test[0], strlen(test[0]), n) ? "YES" : "no");
  
  for (int i = 0; test[i]; i+=2) {
    printf("U=\"%s\"  n=%d  -> %s\n", test[i], atoi(test[i+1]),
           isUniversalCycle(test[i], strlen(test[i]), atoi(test[i+1])) ? "YES" : "no");
  }

  return 0;
}
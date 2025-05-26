#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Recursive implementation of the Ruskey–Williams ranking algorithm.
// Given a permutation of [0..n] will return a rank [0..n!-1]
int rankRuskeyWilliams(int *perm, int n){
    // This algorithm presumes that perm is a 
    // permutation of {1..n} and it will then split the perm
    // into αnβ where α and β are permutations of {1..n-1}
    // and n is the largest element in the permutation.

    // Base case α = β = ε  
    if (n <= 1) return 0;

    // Find the position of n in permutation
    int pos = 0;
    while (pos < n && perm[pos] != n) pos++;

    // If n is in position 0 then we have α = ε != β
    if (pos == 0) return n * rankRuskeyWilliams(perm + 1, n - 1);
    
    // If the permutation is αnβ
    int m = n - 1;
    int lenBetta = m - pos;     
    int *newPerm = malloc(m * sizeof(int));
    if (!newPerm){
        printf("Memory allocation failed\n");
        return -1;
    }

    // Set the first part of the new permutation to be σ(β)
    // where σ(β) is β rotated one position to the right
    newPerm[0] = perm[n-1];
    for (int i = 1; i < lenBetta; i++) {
        newPerm[i] = perm[pos + i];    
    }

    // Now append α to the new permutation
    for (int i = lenBetta; i < m; i++) {
        newPerm[i] = perm[i - lenBetta];
    }

    // Find the rank(σ(β)α)
    int r = rankRuskeyWilliams(newPerm, m);
    free(newPerm);

    // Now we can compute the rank of the original permutation
    return n - pos + n * r;
}

// Given U of length L and parameters n, rank the substring of length n-1
// starting at index 'start' (circularly).  Returns a rank in [0..n!-1] or 
// -1 if the string is invalid
int rankLehmer(int *U, int L, int n, int start){
    char used[n]; // tracks which of 1..n appear in the substring
    memset(used, 0, sizeof(char) * n); // initialize to false
    int window[n-1],  sum = 0; 

    for (int t = 0; t < n - 1; t++) {
        int x = U[(start + t) % L];

        // Make sure that they are within 1...n
        if (x < 1 || x > n) {
            return -1;
        }
        // Make sure this is not a duplicate symbol 
        if (used[x-1]) {
            return -1;
        }
        used[x-1] = 1; // Mark as seen
        window[t] = x; // Store as 1..n
        sum += x; // Add it to the running sum of the permutation
    }

    // Find the missing symbol 1..n in the permutation using 
    // the fact that ∑n = n(n+1)/2 and ∑window = ∑n - missing
    // So missing = ∑n - ∑window
    int missing = (n * (n + 1) / 2) - sum;

    // Now we can build the full permutation pi[0..n-1]
    // by appending the missing symbol to the window
    int pi[n];
    for (int i = 0; i < n - 1; i++) pi[i] = window[i];
    pi[n - 1] = missing;

    // Compute its Lehmer‐code rank in [0..n!-1]
    int rank = 0;
    unsigned long fact = factorial(n - 1);
    for (int i = 0; i < n; i++) {
        int smaller = 0;
        // Count how many of the remaining symbols are smaller than pi[i]
        for (int j = i + 1; j < n; j++) if (pi[j] < pi[i]) smaller++;

        // Update the rank
        rank += smaller * fact;
        if (i < n - 1) {
            fact /= (n - 1 - i);
        }
    }

    return rank;
}


/*int rankLehmer(char *U, int L, int n, int start){
  char used[n]; // tracks which of 1..n appear in the substring
  memset(used, 0, sizeof(char) * n); // initialize to false
  int window[n], w = 0, sum = 0; 


  // Collect the n-1 symbols from U and verify that 
  // they are in 1..n and distinct
  for (int t = 0; t < n - 1; t++){
    char c = U[(start + t) % L];

    // Make sure that they are within 1...n
    if (c < '1' || c >= '1' + n) return -1;     

    // Convert to 0..n-1 to make indexing easier
    int x = c - '1'; 
    // Make sure this is not a duplicate symbol 
    if (used[x]) return -1; 
    used[x] = 1; // mark as seen
    sum += x + 1; // sum of the symbols of the permutation
    window[w++] = x + 1; // store as 1..n
  }

  // Find the missing symbol 1..n in the permutation using 
  // the fact that ∑n = n(n+1)/2 and ∑window = ∑n - missing
  // So missing = ∑n - ∑window
  int missing = (n * (n + 1) / 2) - sum;

  // Now we can build the full permutation pi[0..n-1]
  // by appending the missing symbol to the window
  int pi[n];
  for (int i = 0; i < n - 1; i++) pi[i] = window[i];
  pi[n - 1] = missing;

  // Compute its Lehmer‐code rank in [0..n!-1]
  int rank = 0;
  unsigned long fact = factorial(n - 1);
  for (int i = 0; i < n; i++) {
    int smaller = 0;

    // Count how many of the remaining symbols are smaller than pi[i]
    for (int j = i + 1; j < n; j++) if (pi[j] < pi[i]) smaller++;

    // Update the rank
    rank += smaller * fact;
    if (i < n - 1) fact /= (n - 1 - i);
  }

  return rank;
}*/

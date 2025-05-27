#include "constructAndRank.h"

unsigned long long fact;

// Helper function to compute n!
// By storing n! as an unsigned long long we can compute
// and store values of n factorial that are less than or equal 
// to 20. This is more than enough for our purposes as to store 
// the bitstring for n = 20 we would need 20! = 2,432,902,008,176,640,000 
// bytes which is 2.4 exabytes, far more than any current computer 
// can store in memory 
unsigned long long factorial(unsigned int n){
  unsigned long f = 1;
  for (unsigned int i = 2; i <= n; i++) f *= i;
  return f;
}



// Return 1 if U[0..] of length n! is a valid shorthand U‑cycle for Π(n), 0 otherwise
int isUniversalCycle(int *U, unsigned long long L, int n){
  unsigned long long len = factorial(n);
  // If the length of U is not equal to n! then it cannot be a universal cycle
  if (len != L) return 0;
  //printf("L = %llu, n! = %llu, n = %d\n", L, len,n);

  // To keep track of which ranks we've seen
  char *seen = malloc(L * sizeof(char));
  if (!seen) {
      fprintf(stderr, "Error memory allocation failed\n");
      return 0;
  }
  memset(seen, 0, L * sizeof(char));

  // Can be used to select the ranking algorithm
  // 2 for 7-order, 1 for Lehmer code, 0 for Ruskey-Williams
  int algo = 2;

  // Loop through the universal cycle U and compute the rank of each
  // substring of length n-1 starting at index i
  // If the rank is invalid or we have seen this rank before
  // then the given string is not a universal cycle
  for (unsigned long long i = 0; i < L; i++) {
    // Call a ranking function
    int rank;
    if (algo == 2){
      // If we are using the 7-order ranking algorithm then we need to
      // create a permutation of length n before we call the ranking 
      // algorithm. This means that we will have to find the missing 
      // symbol in the current substring of of length n-1 and append it
      // to the end of the substring to create a full permutation of length n

      // Copy the current substring of length n-1 into a permutation
      // and compute the sum of the symbols in the substring
      int perm[n], sum = 0;
      for (int j = 0; j < n-1; j++){ 
        perm[j] = U[(i+j) % L];
        sum += perm[j];
      }
      // Find the missing symbol 1..n in the permutation using 
      // the fact that ∑n = n(n+1)/2 and ∑perm = ∑n - missing
      // So missing = ∑n - ∑perm
      int missing = (n * (n + 1) / 2) - sum;
      perm[n-1] = missing;

      // Now that we have constructed the full permutation we can compute its rank
      rank = rank7Order(perm, n);
      
    }

    else if (algo == 1) rank = rankLehmer(U, L, n, i);
    else{
      // If we are using the Ruskey-Williams algorithm then we need to
      // create a permutation of length n before we call the ranking 
      // algorithm. This means that we will have to find the missing 
      // symbol in the current substring of of length n-1 and append it
      // to the end of the substring to create a full permutation of length n

      // Copy the current substring of length n-1 into a permutation
      // and compute the sum of the symbols in the substring
      int perm[n], sum = 0;
      for (int j = 0; j < n-1; j++){ 
        perm[j] = U[(i+j) % L];
        sum += perm[j];
      }
      // Find the missing symbol 1..n in the permutation using 
      // the fact that ∑n = n(n+1)/2 and ∑perm = ∑n - missing
      // So missing = ∑n - ∑perm
      int missing = (n * (n + 1) / 2) - sum;
      perm[n-1] = missing;

      // Now that we have constructed the full permutation we can compute its rank
      rank = rankRuskeyWilliams(perm, n);
    }
    
    //printf("rank %d\n", rank);

    // If the rank is invalid or we have seen this rank before
    // then the given string is not a universal cycle
    if (rank < 0 || rank >= L || seen[rank]){
        free(seen);
        return 0;
    }
    // Otherwise mark this rank as seen and continue
    seen[rank] = 1;
  }
  // If we have seen all ranks from 0 to n! - 1 then the given string is a universal cycle
  free(seen);
  return 1;
}
  
void outputUC(int * UC, int n, FILE *fptr){
  // If n is less then 10 output the UC as normal to the desired output stream
  if (n < 10) for (unsigned long long i = 0; i < fact; i++) fprintf(fptr,"%d", UC[i]);

  // If n is greater than or equal to 10 we will have conflicts with overlaping numbers
  // (ie '1112' could be '1,11,2' or '11,12') and there won't be a good way to distinguish  
  // between these all of the different possibilities. So as a solution to this problem we    
  // will output the UC as a string of characters, where 0-9 are represented by '0'-'9' 
  // and 10-n are represented by A,B,C... This way we can represent all numbers from 0 
  // to n without any ambiguity. 
  else{
    for (unsigned long long i = 0; i < fact; i++){
      if (UC[i] < 10) fprintf(fptr,"%d", UC[i]);
      else fprintf(fptr,"%c",UC[i]-10+'A');
    }
  }
}

int main(int argc, char **argv){
  int n;
  printf("Enter n: ");
  scanf("%d", &n);
  
  // Compute and store n! in global memory so
  // we only have to calculate it one time
  fact = factorial(n);

  int *UC = generateUniversalCycle(n);
  if (UC == NULL){
    printf("Error generating universal cycle\n");
    return 0;
  }

  // if the user entered '-f' to have the UC outputed to a file
  // then open the output file and output the UC to it
  if (argc > 1 && (strcmp(argv[1],"-f") == 0 || (argc > 2 && strcmp(argv[2],"-f") == 0))){
    FILE *fptr = fopen("UC", "w");
    outputUC(UC, n, fptr);
    fclose(fptr);
  }

  // If no '-f' arg was given then just output the UC to stdout 
  else{
    printf("UC: ");
    outputUC(UC, n, stdout);
    printf("\n");
  }
  
  free(UC);
  


  int test1[] = {3,2,1,3,1,2};               
  int test2[] = {4,3,2,1,4,2,1,3,4,1,3,2,4,3,1,2,4,1,2,3,4,2,3,1};

  struct {
      int *U;
      unsigned long long L;
      int  n;
  } tests[] = {
      { test1, sizeof(test1) / sizeof(test1[0]), 3 },
      { test2, sizeof(test2) / sizeof(test2[0]), 4 },
      { NULL,    0 }   
  };

  for (int i = 0; tests[i].U; i++){
      int *U = tests[i].U;
      int L = tests[i].L; 
      int n = tests[i].n;
      unsigned long len = factorial(n);

      // print U in “{3,2,1,3,1,2}” style
      printf("U={");
      for (int j = 0; j < len; j++){
          printf("%d", U[j]);
          if (j + 1 < len) printf(",");
      }
      printf("}  n=%d  ->  %s\n",
             n,
            isUniversalCycle(U, L, n) ? "YES" : "no");
  }

  fprintf(stderr,"start\n");


    int perms[6][3] = {
      {3,2,1},
      {2,1,3},
      {1,3,2},
      {3,1,2},
      {1,2,3},
      {2,3,1},
    };

    for (int i = 0; i < 6; i++){
        int *p = perms[i];
        int rk = rank7Order(p, 3);
        printf("%d: %d %d %d  → rank %d\n",
               i, p[0],p[1],p[2], rk);
    }

  fprintf(stderr,"Done\n");

  return 0;
}



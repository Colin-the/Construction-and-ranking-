
#include <stdio.h>
#include <stdlib.h>

// The max value of n we could have. This is because we are storing 
// n! in a unsigned long long which enables us to go up to 20! which 
// is more than enough as we will hit memory limits before we hit this limit
#define maxN 20 

int  n;            
unsigned long long fact;  
int  a[maxN];     
int *U;            
long write_pos = 0;

// Helper function to rotate a from 0...k to the right
static void rotateRight(int k){
    int tmp = a[k];
    for (int i = k; i > 0; i--) a[i] = a[i-1];
    a[0] = tmp;
}

// Helper function to append a[n-1] into U
void visit(){
  // If we have not filled the array yet then append 
  if (write_pos < fact) U[write_pos++] = a[n-1];
}

// Recursive implementation of the bell7 algorithm
// as presented in the Holroyd-Ruskey-Williams paper
void Bell7(int m){
    if (m == n){
        visit();
    }else{
        Bell7(m+1);
        for (int i = 0; i < m; i++){
            rotateRight(m);
            Bell7(m+1);
        }
        rotateRight(m);
    }
}

int main(int argc, char **argv){
    printf("Enter n: ");
    scanf("%d", &n);
  
    if (n < 1 || n > maxN) {
        fprintf(stderr, "Error: n must be between 1 and %d\n", maxN);
        return 1;
    }

    fact = factorial(n);
    U = malloc(fact * sizeof(int));
    if (!U) {
        fprintf(stderr, "Error allocating memory -> terminating program\n\n");
        return 1;
    }

    // Initialize a to n,n-1...1
    for (int i = 0; i < n; i++) a[i] = n - i;
    
    Bell7(1);

    // output the UC that we just generated
    for (unsigned long long i = 0; i < fact; i++) printf("%d", U[i]);
    
    printf("\n");

    free(U);
    return 0;
}
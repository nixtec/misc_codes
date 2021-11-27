#include<stdio.h>

/* Utilities for static and dynamic two-dimensional arrays in C 

 Static arrays:  Stored as blocks in memory in "row-major" order.
 That is, given int A[3][3], the data is laid out in memory as:
 A[0][0], A[0][1], A[0][2], A[1][0], A[1][1], etc ...
 You need to be aware of this when makeing pointer calculations.

 "Static" means that memory for the array is allocated at compile
 time - you must know the dimensions of your array when declaring them.
 You can only use constants or macros for constants when stating the
 sizes of arrays.

 With static arrays it's easy to calculate the array index, and you
 can send/receive an entire matrix using a single block transfer.  But
 you do need to know your pointers.  Given the declaration double A[3][3],
 what is A+2?  You might think that it's the address of element
 A[0][2], the third element.  But that's not correct.  Each row
 consists of 3 elements, so A+2 should really be thought of as
 A+(2*3), which is the address of the third row, or of A[2][0].  Got
 it?  OK, but how do you get the address of A[0][2] then?  You can use
 A[0]+2, or type cast A from type double[][] to double*:
 ((double*)A)+2 will give you the address of the 2nd double value
 after the memory address A.  Pointer arithemetics depend on type
 info.

 Another thing to remember is that very large arrays should not be
 allocated on the stack, as that will overflow the stack.  That is,
 instead of 

  {
     double A[1000][1000];

 Do instead:

     static double A[1000][1000];

 Or, if this is the principal structure of your program, you can declare
 it as a global (but don't get carried away with globals!).  Declaring 
 the variable static means that all manifestations of the current scope
 (function call) will share the same structure.

 However, you should understand the following if you declare a static
 or global static array, the memory will be allocated for the array 
 regardless of whether the program really needs it.

 Another not-so-nice thing about static arrays is that, when defining a
 function, you must specify the array dimensions (at least columns if
 not rows), otherwise there'll be no way for the function to calculate
 the correct indices.  But then how do you write generic procedures to
 handle arrays of any size?  You can use dynamic arrays (below), or
 macros such as follows.  Macros are applied to your code before the
 compiler does any real work - it's just a form of automatic cutting
 and pasting.  Specifically, parameters to macros are dynamically scoped,
 so be careful that the names of these params are separate from all others.

*/

// for static, multi-dimensional arrays in C, sometimes a macro is better
// than a function.  Note that i, j are still local vars because they're
// inside {}'s.

// prints array AA, size rzXcz, using formated string fs.
// use as in PRINT2M(A,4,5,"%f ")
#define PRINT2M(AA,rz,cz,fs) { \
 int i, j; \
 for (i=0;i<rz;i++) \
   { for (j=0;j<cz;j++) printf(fs,AA[i][j]); \
     printf("\n"); } \
}

// randomize values
#define RANDOMIZE(AA,rz,cz) { \
   int i, j; \
   for (i=0;i<rz;i++) \
     for(j=0;j<cz;j++) \
       AA[i][j] = (random()%10000)*3.14; \
}

// *************************************************************
// Dynamic array: a two-dim array is a one dim array in which
// each element is a pointer to another array.  A dynamic array
// is thus larger than a static array, and the data is not contiguous.
// It's larger because a separate row is used for storing the pointers,
// and not contiguous because the pointers can point anywhere. 
// In particular, you must send/receive each row individually when
// transmitting the matrix between distributed processes in MPI.

double **make2d(int r, int c)
{ int i;
  double **m;
  m = (double**)malloc(r*sizeof(double*));
  for (i=0;i<r;i++) 
    m[i] = (double*)malloc(c*sizeof(double));
  return m;
}

void free2d(double ** M, int r)
{ int i;
  if (!M) return;
  for(i=0;i<r;i++) free(M[i]);
  free(M);
}

// only works for dynamic arrays: 
void printm(double **A, int r, int c)
{
  int i, j;
  for (i=0;i<r;i++)
    {
      for (j=0;j<c;j++) printf("%f ",A[i][j]);
      printf("\n");
    }
}


/* For testing:  matrix multiplication */

#define ROWS 4
#define COLS 5
// parameter must specify at least column size
void mmult(double A[ROWS][COLS], double B[COLS][ROWS], double AB[ROWS][ROWS])
{  int i, j, k;
   
   for(i=0;i<ROWS;i++);
     for(j=0;j<ROWS;j++)
	 {  AB[i][j] = 0;
	    for(k=0;k<COLS;k++)
		AB[i][j] += A[i][k] * B[k][j];
	 } 
} // mmult



/*  **********************************************************************
    As you should see, dynamically allocated arrays are more flexible.
    You don't have to know the sizes of the arrays at compile time, and
    memory for them is not allocated until needed.  But the 2D dynamic
    arrays of the previous section are not contiguous.  That presents a
    problem with efficiency, especially when transmitting such structures
    across media.  Fortunately, it is also possible to create a
    dynamically allocated, contiguous multi-dimensional array:

    // to allocate a 30x20 array:

    typedef double ARRAYROWS[20];
    ...
    ARRAYROWS *M;  // pointer to 2D matrix
    M = (ARRAYROWS *)malloc(sizeof(double)*30*20);  
    **********************************************************************/

    typedef double ARRAYROWS[20];
    ARRAYROWS *M;  // pointer to 2D matrix, each row must have 20 columns 

int main()
{
  int i, j, k, r, c;
  double A[ROWS][COLS];
  double B[COLS][ROWS];   // used by all procs
  double AB[ROWS][ROWS]; // holds result
  double **C;
  C = make2d(ROWS,COLS);
  RANDOMIZE(A,ROWS,COLS);
  PRINT2M(A,ROWS,COLS,"%f ");

  printf("%f\n",*(double*)(A+3)); // prints 1st element of 4th row

  printf("%f\n",*((double*)A+3)); // prints 4th element of 1st row
  // A is casted first before 3 is added, forming a pointer   

  printf("%f\n",*(A[1]+3)); // prints 4th element of 1st row

  RANDOMIZE(C,ROWS,COLS);
  printm(C,ROWS,COLS);
  //   printm(A,ROWS,COLS);

  // dynamic, contiguous 2d array:
  M = (ARRAYROWS *)malloc(sizeof(double)*20*30); // 30x20 matrix
  M[29][19] = 99;
  printf("\n%f\n", *((double*)M+(20*29)+19));
   exit(0);
} // main


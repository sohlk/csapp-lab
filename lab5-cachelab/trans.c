/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include <stdlib.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    /*
     * 1.use 8 x 8 submatrix because A[i][j] and A[i + 8][j] will cause conflict miss(exceed 300)
     * 2.A and B may cause conflict miss,read a 8-int line every time
     */
    if( M == 32 )
    {
        int i,j,k;
        for( i = 0;i < 32;i += 8 )
        {
            for( j = 0;j < 32;j += 8 )
            {
                for( k = 0;k < 8;k ++ )
                {
                    int d0 = A[i + k][j + 0];
                    int d1 = A[i + k][j + 1];
                    int d2 = A[i + k][j + 2];
                    int d3 = A[i + k][j + 3];
                    int d4 = A[i + k][j + 4];
                    int d5 = A[i + k][j + 5];
                    int d6 = A[i + k][j + 6];
                    int d7 = A[i + k][j + 7];
                    B[j + 0][i + k] = d0;
                    B[j + 1][i + k] = d1;
                    B[j + 2][i + k] = d2;
                    B[j + 3][i + k] = d3;
                    B[j + 4][i + k] = d4;
                    B[j + 5][i + k] = d5;
                    B[j + 6][i + k] = d6;
                    B[j + 7][i + k] = d7;
                }
            }
        }
    }
    /*
     * use 4 x 4 submatrix(exceed 1300)
     * need better algorithm
     */
    else if( M == 64 )
    {
        int i,j,k;
        for( i = 0;i < 64;i += 4 )
        {
            for( j = 0;j < 64;j += 4 )
            {
                for( k = 0;k < 4;k ++ )
                {
                    int d0 = A[i + k][j + 0];
                    int d1 = A[i + k][j + 1];
                    int d2 = A[i + k][j + 2];
                    int d3 = A[i + k][j + 3];
                    B[j + 0][i + k] = d0;
                    B[j + 1][i + k] = d1;
                    B[j + 2][i + k] = d2;
                    B[j + 3][i + k] = d3;
                }
            }
        }
    }
    /*
     * try 8x8,16x16,24x24
     */
    else if( M == 61 )
    {
        int i,j,k,l;
        for( i = 0;i < 67;i += 16 )
        {
            for( j = 0;j < 61;j += 16 )
            {
                for( k = 0;k < 16 && ((i + k) < 67);k ++ )
                {
                    for( l = 0;l < 16 && ((j + l) < 61);l ++ )
                        B[j + l][i + k] = A[i + k][j + l];
                }
            }
        }
    }
    else
        exit( 1 );
    return;
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


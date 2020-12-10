//------------------------------------------------------------------------------
// GB_sort.h: definitions for sorting functions
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// All of the GB_qsort_* functions are single-threaded, by design.  Both
// GB_msort_* functions are parallel.  None of these sorting methods are
// guaranteed to be stable, but they are always used in GraphBLAS with unique
// keys.

#ifndef GB_SORT_H
#define GB_SORT_H
#include "GB.h"

#define GB_BASECASE (64 * 1024)

GB_PUBLIC   // accessed by the MATLAB tests in GraphBLAS/Test only
void GB_qsort_1b    // sort array A of size 2-by-n, using 1 key (A [0][])
(
    int64_t *GB_RESTRICT A_0,      // size n array
    GB_void *GB_RESTRICT A_1,      // size n array
    const size_t xsize,         // size of entries in A_1
    const int64_t n
) ;

void GB_qsort_1b_size1  // GB_qsort_1b with A1 with sizeof = 1
(
    int64_t *GB_RESTRICT A_0,       // size n array
    uint8_t *GB_RESTRICT A_1,       // size n array
    const int64_t n
) ;

void GB_qsort_1b_size2  // GB_qsort_1b with A1 with sizeof = 2
(
    int64_t *GB_RESTRICT A_0,       // size n array
    uint16_t *GB_RESTRICT A_1,      // size n array
    const int64_t n
) ;

void GB_qsort_1b_size4  // GB_qsort_1b with A1 with sizeof = 4
(
    int64_t *GB_RESTRICT A_0,       // size n array
    uint32_t *GB_RESTRICT A_1,      // size n array
    const int64_t n
) ;

void GB_qsort_1b_size8  // GB_qsort_1b with A_1 with sizeof = 8
(
    int64_t *GB_RESTRICT A_0,       // size n array
    uint64_t *GB_RESTRICT A_1,      // size n array
    const int64_t n
) ;

typedef struct
{
    uint8_t stuff [16] ;            // not accessed directly
}
GB_blob16 ;                         // sizeof (GB_blob16) is 16.

void GB_qsort_1b_size16 // GB_qsort_1b with A_1 with sizeof = 16
(
    int64_t *GB_RESTRICT A_0,       // size n array
    GB_blob16 *GB_RESTRICT A_1,     // size n array
    const int64_t n
) ;

GB_PUBLIC   // accessed by the MATLAB tests in GraphBLAS/Test only
void GB_qsort_2     // sort array A of size 2-by-n, using 2 keys (A [0:1][])
(
    int64_t *GB_RESTRICT A_0,      // size n array
    int64_t *GB_RESTRICT A_1,      // size n array
    const int64_t n
) ;

GB_PUBLIC   // accessed by the MATLAB tests in GraphBLAS/Test only
void GB_qsort_3     // sort array A of size 3-by-n, using 3 keys (A [0:2][])
(
    int64_t *GB_RESTRICT A_0,      // size n array
    int64_t *GB_RESTRICT A_1,      // size n array
    int64_t *GB_RESTRICT A_2,      // size n array
    const int64_t n
) ;

GB_PUBLIC   // accessed by the MATLAB tests in GraphBLAS/Test only
void GB_msort_2     // sort array A of size 2-by-n, using 2 keys (A [0:1][])
(
    int64_t *GB_RESTRICT A_0,   // size n array
    int64_t *GB_RESTRICT A_1,   // size n array
    int64_t *GB_RESTRICT W_0,   // size n array, workspace
    int64_t *GB_RESTRICT W_1,   // size n array, workspace
    const int64_t n,
    int nthreads                // # of threads to use
) ;

GB_PUBLIC   // accessed by the MATLAB tests in GraphBLAS/Test only
void GB_msort_3     // sort array A of size 3-by-n, using 3 keys (A [0:2][])
(
    int64_t *GB_RESTRICT A_0,   // size n array
    int64_t *GB_RESTRICT A_1,   // size n array
    int64_t *GB_RESTRICT A_2,   // size n array
    int64_t *GB_RESTRICT W_0,   // size n array, workspace
    int64_t *GB_RESTRICT W_1,   // size n array, workspace
    int64_t *GB_RESTRICT W_2,   // size n array, workspace
    const int64_t n,
    int nthreads                // # of threads to use
) ;

GB_PUBLIC   // accessed by the MATLAB tests in GraphBLAS/Test only
GrB_Info GB_msort_3b    // sort array A of size 3-by-n, using 3 keys (A [0:2][])
(
    int64_t *GB_RESTRICT A_0,   // size n array
    int64_t *GB_RESTRICT A_1,   // size n array
    int64_t *GB_RESTRICT A_2,   // size n array
    const int64_t n,
    int nthreads                // # of threads to use
) ;

void GB_msort_3b_create_merge_tasks
(
    // output:
    int64_t *GB_RESTRICT L_task,        // L_task [tfirst...ntasks] computed
    int64_t *GB_RESTRICT R_task,        // R_task [tfirst...ntasks] computed
    int64_t *GB_RESTRICT S_task,        // S_task [tfirst...ntasks] computed
    // input:
    const int tfirst,                   // first task tid to create
    const int ntasks,                   // # of tasks to create
    const int64_t pS_start,             // merge into S [pS_start...]
    const int64_t *GB_RESTRICT L_0,     // Left = L [pL_start...pL_end-1]
    const int64_t *GB_RESTRICT L_1,
    const int64_t *GB_RESTRICT L_2,
    const int64_t pL_start,
    const int64_t pL_end,
    const int64_t *GB_RESTRICT R_0,     // Right = R [pR_start...pR_end-1]
    const int64_t *GB_RESTRICT R_1,
    const int64_t *GB_RESTRICT R_2,
    const int64_t pR_start,
    const int64_t pR_end
) ;

//------------------------------------------------------------------------------
// # of threads to use in parallel mergesort
//------------------------------------------------------------------------------

#if defined ( _OPENMP ) && GB_HAS_OPENMP_TASKS

    // With OpenMP v4.0: use all available threads in a parallel mergesort.
    #define GB_MSORT_NTHREADS(nthreads) nthreads

#else

    // OpenMP tasks are not available, so just use a sequential quicksort
    // with a single thread.  OpenMP tasks requires OpenMP v4.0 or later.
    // Microsoft Visual Studio only supports OpenMP 2.0, so the parallel
    // mergesort is not available when using that compiler.
    #define GB_MSORT_NTHREADS(nthreads) 1

#endif

//------------------------------------------------------------------------------
// GB_lt_1: sorting comparator function, one key
//------------------------------------------------------------------------------

// A [a] and B [b] are keys of one integer.

// GB_lt_1 returns true if A [a] < B [b], for GB_qsort_1b

#define GB_lt_1(A_0, a, B_0, b) (A_0 [a] < B_0 [b])

//------------------------------------------------------------------------------
// GB_lt_2: sorting comparator function, two keys
//------------------------------------------------------------------------------

// A [a] and B [b] are keys of two integers.

// GB_lt_2 returns true if A [a] < B [b], for GB_qsort_2 and GB_msort_2

#define GB_lt_2(A_0, A_1, a, B_0, B_1, b)                                   \
(                                                                           \
    (A_0 [a] < B_0 [b]) ?                                                   \
    (                                                                       \
        true                                                                \
    )                                                                       \
    :                                                                       \
    (                                                                       \
        (A_0 [a] == B_0 [b]) ?                                              \
        (                                                                   \
            /* primary key is the same; tie-break on the 2nd key */         \
            (A_1 [a] < B_1 [b])                                             \
        )                                                                   \
        :                                                                   \
        (                                                                   \
            false                                                           \
        )                                                                   \
    )                                                                       \
)

//------------------------------------------------------------------------------
// GB_lt_3: sorting comparator function, three keys
//------------------------------------------------------------------------------

// A [a] and B [b] are keys of three integers.

// GB_lt_3 returns true if A [a] < B [b], for GB_qsort_3 and GB_msort_2

#define GB_lt_3(A_0, A_1, A_2, a, B_0, B_1, B_2, b)                         \
(                                                                           \
    (A_0 [a] < B_0 [b]) ?                                                   \
    (                                                                       \
        true                                                                \
    )                                                                       \
    :                                                                       \
    (                                                                       \
        (A_0 [a] == B_0 [b]) ?                                              \
        (                                                                   \
            /* primary key is the same; tie-break on the 2nd and 3rd key */ \
            GB_lt_2 (A_1, A_2, a, B_1, B_2, b)                              \
        )                                                                   \
        :                                                                   \
        (                                                                   \
            false                                                           \
        )                                                                   \
    )                                                                       \
)

//------------------------------------------------------------------------------
// GB_eq_3: sorting comparator function, three keys
//------------------------------------------------------------------------------

// A [a] and B [b] are keys of three integers.

// GB_eq_3 returns true if A [a] == B [b]

#define GB_eq_3(A_0, A_1, A_2, a, B_0, B_1, B_2, b)                         \
(                                                                           \
    (A_0 [a] == B_0 [b]) &&                                                 \
    (A_1 [a] == B_1 [b]) &&                                                 \
    (A_2 [a] == B_2 [b])                                                    \
)

//------------------------------------------------------------------------------
// random number generator for quicksort
//------------------------------------------------------------------------------

// return a random GrB_Index, in range 0 to 2^60
#define GB_RAND_MAX 32767

// return a random number between 0 and GB_RAND_MAX
static inline GrB_Index GB_rand15 (uint64_t *seed)
{ 
   (*seed) = (*seed) * 1103515245 + 12345 ;
   return (((*seed) / 65536) % (GB_RAND_MAX + 1)) ;
}

// return a random GrB_Index, in range 0 to 2^60
static inline GrB_Index GB_rand (uint64_t *seed)
{ 
    GrB_Index i = GB_rand15 (seed) ;
    i = GB_RAND_MAX * i + GB_rand15 (seed) ;
    i = GB_RAND_MAX * i + GB_rand15 (seed) ;
    i = GB_RAND_MAX * i + GB_rand15 (seed) ;
    return (i) ;
}

#endif


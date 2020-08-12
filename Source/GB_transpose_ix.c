//------------------------------------------------------------------------------
// GB_transpose_ix: transpose the values and pattern of a matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// The values of A are typecasted to C->type, the type of the C matrix.

// If A is sparse or hypersparse
//      The pattern of C is constructed.  C is sparse.
//      Rowcounts and A_slice are non-NULL.
//      This method is parallel, but not highly scalable.  It uses only
//      nthreads = nnz(A)/(A->vlen) threads.

// If A is full:
//      The pattern of C is not constructed.  C is full.
//      Rowcounts and A_slice are NULL.
//      This method is parallel and fully scalable.

// If A is bitmap:
//      C->b is constructed.  C is bitmap.
//      Rowcounts and A_slice are NULL.
//      This method is parallel and fully scalable.

#include "GB_transpose.h"
#ifndef GBCOMPACT
#include "GB_unop__include.h"
#endif

void GB_transpose_ix            // transpose the pattern and values of a matrix
(
    GrB_Matrix C,                       // output matrix
    const GrB_Matrix A,                 // input matrix
    // for sparse case:
    int64_t *GB_RESTRICT *Rowcounts,    // Rowcounts, size naslice
    const int64_t *GB_RESTRICT A_slice, // how A is sliced, size naslice+1
    int naslice,                        // # of slices (and # threads to use)
    // for full or bitmap case:
    int nthreads                        // # of threads to use
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    ASSERT (!GB_ZOMBIES (A)) ;
    ASSERT (GB_JUMBLED_OK (A)) ;
    ASSERT (!GB_PENDING (A)) ;

    GrB_Info info ;
    GrB_Type ctype = C->type ;
    GB_Type_code code1 = ctype->code ;          // defines ztype
    GB_Type_code code2 = A->type->code ;        // defines atype

    //--------------------------------------------------------------------------
    // built-in worker: transpose and typecast
    //--------------------------------------------------------------------------

    #ifndef GBCOMPACT

        //----------------------------------------------------------------------
        // define the worker for the switch factory
        //----------------------------------------------------------------------

        #define GB_unop_tran(zname,aname)                           \
            GB_unop_tran__identity ## zname ## aname

        #define GB_WORKER(ignore1,zname,ztype,aname,atype)          \
        {                                                           \
            info = GB_unop_tran (zname,aname)                       \
                (C, A, Rowcounts, A_slice, naslice, nthreads) ;     \
            if (info == GrB_SUCCESS) return ;                       \
        }                                                           \
        break ;

        //----------------------------------------------------------------------
        // launch the switch factory
        //----------------------------------------------------------------------

        // switch factory for two built-in types, controlled by code1 and code2
        #include "GB_2type_factory.c"

    #endif

    //--------------------------------------------------------------------------
    // generic worker: transpose and typecast
    //--------------------------------------------------------------------------

    GB_BURBLE_MATRIX (A, "(generic transpose) ") ;

    size_t asize = A->type->size ;
    size_t csize = C->type->size ;
    GB_cast_function cast_A_to_X = GB_cast_factory (code1, code2) ;

    // Cx [pC] = (ctype) Ax [pA]
    #define GB_CAST_OP(pC,pA)  \
        cast_A_to_X (Cx +((pC)*csize), Ax +((pA)*asize), asize) ;

    #define GB_ATYPE GB_void
    #define GB_CTYPE GB_void

    #define GB_PHASE_2_OF_2
    #include "GB_unop_transpose.c"
}


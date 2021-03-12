//------------------------------------------------------------------------------
// GB_mx_put_global: put the GraphBLAS status in MATLAB workspace
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_mex.h"

void GB_mx_put_global
(
    bool cover
)
{

    //--------------------------------------------------------------------------
    // free the complex type and operators
    //--------------------------------------------------------------------------

    Complex_finalize ( ) ;

    //--------------------------------------------------------------------------
    // return the time to MATLAB, if it was computed
    //--------------------------------------------------------------------------

    GB_mx_put_time ( ) ;

    //--------------------------------------------------------------------------
    // log the statement coverage
    //--------------------------------------------------------------------------

    #ifdef GBCOVER
    if (cover) GB_cover_put ( ) ;
    #endif

    //--------------------------------------------------------------------------
    // finalize GraphBLAS
    //--------------------------------------------------------------------------

    GrB_finalize ( ) ;

    // disable the memory pool, in case a @GrB method is called next
    for (int k = 0 ; k < 64 ; k++)
    {
        // TODO:: add this as an option for GxB_set/get
        GB_Global_free_pool_limit_set (k, 0) ;
    }

#if 0
        // dump the memory pool
        GB_Global_free_pool_dump (3) ;
        for (int k = 0 ; k < 64 ; k++)
        {
            int64_t t = GB_Global_free_pool_limit_get (k) ;
            if (t > 0) printf ("pool %2d: %ld\n", k, t) ;
        }
#endif

    //--------------------------------------------------------------------------
    // check nmemtable and nmalloc
    //--------------------------------------------------------------------------

    int nmemtable = GB_Global_memtable_n ( ) ;
    if (nmemtable != 0)
    {
        printf ("in GB_mx_put_global: GraphBLAS nmemtable %d!\n", nmemtable) ;
        GB_Global_memtable_dump ( ) ;
        mexErrMsgTxt ("memory leak in test!") ;
    }

    int64_t nblocks = GB_Global_free_pool_nblocks_total ( ) ;
    if (nblocks != 0)
    {
        printf ("in GB_mx_put_global: GraphBLAS nblocks "GBd" in free_pool!\n",
            nblocks) ;
        mexErrMsgTxt ("memory leak in test!") ;
    }

    int64_t nmalloc = GB_Global_nmalloc_get ( ) ;
    if (nmalloc != 0)
    {
        printf ("in GB_mx_put_global: GraphBLAS nmalloc "GBd"!\n", nmalloc) ;
        mexErrMsgTxt ("memory leak in test!") ;
    }

    //--------------------------------------------------------------------------
    // allow GrB_init to be called again
    //--------------------------------------------------------------------------

    GB_Global_GrB_init_called_set (false) ;
}

